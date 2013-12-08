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

double average_ranges(const playerc_ranger_t *const ranger, double start_angle, double end_angle, double *sum)
{
	if (start_angle > end_angle)
	{
		double temp = end_angle;
		end_angle = start_angle;
		start_angle = temp;
	}

	int start_index = LASER_RANGEINDEX_FROM_ANGLE_DEG(start_angle);
	int end_index   = LASER_RANGEINDEX_FROM_ANGLE_DEG(end_angle);
	double count = end_index - start_index + 1.0;
	double s = 0;
	for (int i=start_index; i <= end_index; ++i)
	{
		s += ranger->ranges[i];
	}	

	if (sum != 0)
		*sum = s;

	return s/count;
}

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

		/* Fahrlogik */
		if (ranger->ranges_count > 0)
		{
			/* Summe aller Entfernungen */
			double sum = 0;
			for (int i=0; i < ranger->ranges_count; ++i)
			{
				sum += ranger->ranges[i];
			}	

			/* Bahngeschwindigkeit ermitteln */
			double front_exact = ranger->ranges[LASER_RANGEINDEX_FROM_ANGLE_DEG(0)];
			double front      = average_ranges(ranger, -22.5, 22.5, NULL);
			double front_wide = average_ranges(ranger, -45.0, 45.0, NULL); 
			double v = LERP(LASER_RANGE_MIN*2, LASER_RANGE_MAX*3/4, front_wide, 0, 0.4);
			
			/* Bouncer rechts */
			double right_front_exact = ranger->ranges[LASER_RANGEINDEX_FROM_ANGLE_DEG(50)];
			double right_front = average_ranges(ranger, 22.5, 67.5, NULL); 
			double right       = average_ranges(ranger, 67.5, 112.5, NULL); 
			double right_back  = average_ranges(ranger, 112.5, LASER_MAX_ANGLE_DEG, NULL); 

			/* Bouncer links */
			double left_front_exact = ranger->ranges[LASER_RANGEINDEX_FROM_ANGLE_DEG(-50)];
			double left_front  = average_ranges(ranger, -22.5, -67.5, NULL); 
			double left        = average_ranges(ranger, -67.5, -112.5, NULL); 
			double left_back   = average_ranges(ranger, -112.5, LASER_MIN_ANGLE_DEG, NULL); 

			double w = 0;

			/* Wenn Gefahr vorne rechts, drift links */
			w -= LERP(LASER_RANGE_MIN, LASER_RANGE_MAX, right_front, 0, 1);
			w -= LERP_SATURATE(LASER_RANGE_MIN, 1, right_front_exact, 0, 1);

			/* Wenn Gefahr vorne links, drift rechts */
			w += LERP(LASER_RANGE_MIN, LASER_RANGE_MAX, left_front, 0, 1);
			w += LERP_SATURATE(LASER_RANGE_MIN, 1, left_front_exact, 0, 1);

			/* Wenn rechts frei - fahre rechts.
			*  Ein sehr freies Feld sorgt für starken Rechtsdrall.
    		*/
			w += LERP(LASER_RANGE_MIN, 2, right, 0, 0.4);

			/* Tendenz zum Linksabbiegen hinzufügen 
			*  Gewichten mit dem Bestreben, rechts abzubiegen, wenn dort frei ist.
			*  Hierdurch gewinnt das Rechtsabbiegen.
			*/
			w -= LERP_SATURATE(LASER_RANGE_MIN, 1, front, 0.5, 0) * LERP(LASER_RANGE_MIN, 2, right, 0, 0.4);

			/* Hindernis exakt voraus vermeiden durch Linksabbiegen. 
			*  Grad des Unterschreitens der "Fluchtdistanz" bestimmt Stärke.
			*/
			w -= LERP_SATURATE(LASER_RANGE_MIN, 1, front_exact, 0.5, 0);
			
			/* Linksabbiegen vermeiden, wenn kein Hindernis.
			*  Dieser Term korrigiert die vorherige Interpolation für Messwerte
			*  die hinter die "Fluchtdistanz" liegen.
			*/
			w += LERP_SATURATE(1, 1+LASER_RANGE_MIN, front_exact, 0, 0.5);

			/* Pose und Zustände ausgeben */
			printf("x=%7.5f, y=%7.5f, theta=%7.5f°, v=%7.5fm/s, omega=%7.5frad/s\n", position2d->px, position2d->py, position2d->pa*180/M_PI, v, w);

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
