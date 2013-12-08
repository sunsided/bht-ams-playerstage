#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <wait.h>
#include <errno.h>
#include <math.h>
#include <libplayerc/playerc.h>

#include "map.h"
#include "laser.h"
#include "transforms.h"


int main(int argc, char *argv[])
{
	playerc_client_t *client;
	playerc_position2d_t *position2d;
	playerc_ranger_t *ranger;

	if (argc<2)
	{
		printf("Usage: %s <hostname>\n",basename(argv[0]));
		return 1;
	}

	/* Create a client and connect it to the server. */
	client = playerc_client_create(NULL, argv[1], 6665);
	if (0 != playerc_client_connect(client)) {
		return -1;
	}

	/* Create and subscribe to a position2d device. */
	position2d = playerc_position2d_create(client, 0);
	if (playerc_position2d_subscribe(position2d, PLAYER_OPEN_MODE)) {
		return -1;
	}
	playerc_position2d_enable(position2d,1);

	/* Laser-Sensor-Modell */
	ranger = playerc_ranger_create(client, 0);
	if (playerc_ranger_subscribe(ranger, PLAYER_OPEN_MODE) != 0) { 
		printf("ranger error!\n");
		exit(1);
	}

	// Make the robot go foreward!
#if 0
	if (0 != playerc_position2d_set_cmd_vel(position2d, 0.5, 0.0, 0.0, 1))
		return -1;
#endif

	// Make the robot spin!
#if 0
	if (0 != playerc_position2d_set_cmd_vel(position2d, 0, 0, DTOR(40.0), 1))
		return -1;
#endif

	// ---------- In Endlosschleife Labyrinth abfahren
	while(1) {

		// Wait for new data from server
		playerc_client_read(client);

#if 0
		/* Test auf Wand in 1m vor uns */
		double x,y;
		transformLocalToMap(1, 0, position2d, &x, &y);
		int iswall = map_iswall(x, y);
#else
		int iswall = 0;
#endif

		/* Pose und Wand-Check ausgeben */
		printf("x=%7.5f, y=%7.5f, theta=%7.5f, iswall=%d\n", position2d->px, position2d->py, position2d->pa*180/M_PI, iswall);

		/* Fahrlogik */
		if (ranger->ranges_count > 0)
		{
			double front   = ranger->ranges[LASER_RANGEINDEX_FROM_ANGLE_DEG(  0)];
			double left45  = ranger->ranges[LASER_RANGEINDEX_FROM_ANGLE_DEG( 45)];
			double right45 = ranger->ranges[LASER_RANGEINDEX_FROM_ANGLE_DEG(-45)];
			double left90  = ranger->ranges[LASER_RANGEINDEX_FROM_ANGLE_DEG( 90)];
			double right90 = ranger->ranges[LASER_RANGEINDEX_FROM_ANGLE_DEG(-90)];
			double right120 = ranger->ranges[LASER_RANGEINDEX_FROM_ANGLE_DEG(-120)];
			//printf("left90=%7.5f, left45=%7.5f, front sensing: %7.5f, right45=%7.5f, right90=%7.5f\n", left90, left45, front, right45, right90);

#if 1
			/* Bahngeschwindigkeit ermitteln */
			double v       = LERP(LASER_RANGE_MIN, LASER_RANGE_MAX, front, 0, 0.2);

#if 0
			/* Winkelgeschwindigkeit ermitteln:
			 * - Wenn vorne Hindernis, drehen nach links. - je näher Hindernis, desto schneller.
			 * - Wenn vorne kein Hindernis und rechts frei, drehen nach Rechts. - je freier, desto schneller.
 			 */
			double wlinks  = -LERP(LASER_RANGE_MIN, LASER_RANGE_MAX, left90,  0, DTOR(45));
			double wrechts = (front > 0.5 ? 1 : 0) * LERP(LASER_RANGE_MIN, LASER_RANGE_MAX, right90, 0, DTOR(45));
			double w = wlinks + wrechts;

			double s       = LERP(LASER_RANGE_MIN, 2*LASER_RANGE_MIN, front,   0, 1);

			printf("v=%7.5f, wlinks=%7.5f, wrechts=%7.5f, w=%7.5f\n", v, wlinks, wrechts, w);
#endif

			double push_angle = 0;
			for (int i=0; i < ranger->ranges_count; ++i)
			{
				const double degree = LASER_DEGREE_FROM_RANGEINDEX(i);
				const double distance = ranger->ranges[i];
				const double offset = 0.5;
				const double factor = 1.0/(distance + offset);
				push_angle += degree * factor / ranger->ranges_count;
			}		
			printf("target angle=%7.5f\n", push_angle);


			double w = push_angle * M_PI/180.0;
			double werr = w;
			w = werr * 0.1;

#if 0			
			double w_right90 = 0;
			double w_right45 = 0;
			double w_left90 = 0;
			double w_left45 = 0;
			double w_front = 0;

			if (right90 < LASER_RANGE_MAX)
				w_right90 += 0.05*(LERP(LASER_RANGE_MIN, LASER_RANGE_MAX, right90, 1, 0.5)-0.5);
			if (right45 < LASER_RANGE_MAX)
				w_right45 -= 0.05*LERP(LASER_RANGE_MIN, LASER_RANGE_MAX, right45, 0, 1);
			if (left90 < LASER_RANGE_MAX)
				w_left90 = 0.05*LERP(LASER_RANGE_MIN, LASER_RANGE_MAX, left90, 0, 1);
			if (left45 < LASER_RANGE_MAX)
				w_left45 = 0.05*LERP(LASER_RANGE_MIN, LASER_RANGE_MAX, left45, 0, 1);
			if (front < LASER_RANGE_MAX)
				w_front -= 0.1*LERP(LASER_RANGE_MIN, LASER_RANGE_MAX, front, 0, 1);
		
			w += 0.1; // w_right90 + w_right45 + w_left90 + w_left45 + w_front;
			printf("l90=%7.5f, l45=%7.5f, f=%7.5f, r45=%7.5f, r90=%7.5f\n", w_left90, w_left45, w_front, w_right45, w_right90);
#endif
#else
			double v = 0.0;
			double w = 0.2;
#endif

			if (0 != playerc_position2d_set_cmd_vel(position2d, v, 0.0, -w, 1))
				return -1;
		}

		/* Karte zeichnen */
		map_draw(ranger, position2d);
	}

	/* Shutdown */
	playerc_position2d_unsubscribe(position2d);
	playerc_position2d_destroy(position2d);

	playerc_ranger_unsubscribe(ranger);
	playerc_ranger_destroy(ranger);

	playerc_client_disconnect(client);
	playerc_client_destroy(client);

	map_shutdown();

	return 0;
}
