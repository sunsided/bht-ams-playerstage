#include "map.h"
#include "opencv/cv.h"
#include "stdio.h"
#include "math.h"

#include "laser.h"
#include "transforms.h"

#ifndef MAX_GRAY
#define MAX_GRAY 255
#endif 

static const char* mapwin = "Robot Map";
static IplImage* mapimg=NULL;     // Image of the map
static int initialized=0;

int map_init();
int map_show(void);

int map_init()
{
   if (initialized) { return 1; }
   mapimg=cvCreateImage(cvSize(MAP_SIZE_X,MAP_SIZE_Y),8,3);
   cvZero(mapimg);
   // create window
   cvNamedWindow( mapwin, 1 );
   initialized=1;
   return 0;
}

int map_iswall(double x, double y)
{
	if (!initialized) { if (map_init()) return 1; }

	CvScalar s = cvGet2D( mapimg,
            MAP_OFFS_Y-(int)(MAP_SCALE*y),
            MAP_OFFS_X+(int)(MAP_SCALE*x)
          );

	return s.val[0] > 0;
}

/**
* Addiert einen Farbwert {r,g,b} auf den gegebenen Pixel {x,y}
* \param[in] x Die X-Koordinate
* \param[in] y Die Y-Koordinate
* \param[in] r Der R-Wert
* \param[in] g Der G-Wert
* \param[in] b Der B-Wert
*/
void setze_wand_dick(double x, double y)
{
	for (int pady = -1; pady < 2; ++pady) 
	{
		for (int padx = -1; padx < 2; ++padx)
		{
			cvSet2D( mapimg,
			    MAP_OFFS_Y-(int)(MAP_SCALE*y)+pady,
			    MAP_OFFS_X+(int)(MAP_SCALE*x)+padx,
			    CV_RGB(MAX_GRAY, MAX_GRAY, MAX_GRAY)
			  );
		}
	}
}

/**
* Addiert einen Farbwert {r,g,b} auf den gegebenen Pixel {x,y}
* \param[in] x Die X-Koordinate
* \param[in] y Die Y-Koordinate
* \param[in] r Der R-Wert
* \param[in] g Der G-Wert
* \param[in] b Der B-Wert
*/
void setze_gesehen_dick(double x, double y)
{
	for (int pady = -1; pady < 2; ++pady) 
	{
		for (int padx = -1; padx < 2; ++padx)
		{
			CvScalar s = cvGet2D( mapimg,
    	        MAP_OFFS_Y-(int)(MAP_SCALE*y)+pady,
	            MAP_OFFS_X+(int)(MAP_SCALE*x)+padx
				);

			// teste auf Wand
			if (s.val[1] == MAX_GRAY)
				continue;

			cvSet2D( mapimg,
			    MAP_OFFS_Y-(int)(MAP_SCALE*y)+pady,
			    MAP_OFFS_X+(int)(MAP_SCALE*x)+padx,
			    CV_RGB(s.val[2], 92, s.val[0])
			  );
		}
	}
}


int map_draw(playerc_ranger_t *ranger, playerc_position2d_t *pos)
{
   if (!initialized) { if (map_init()) return 1; }

   // Hier kommt der Code zum Zeichnen der Waende hinein
   // --------------------------------------------------

	for (int a=0; a < (int)ranger->ranges_count; ++a)       
	{
		/* Polarkoordinaten beziehen */
		double angle = a * LASER_ANGULAR_RESOLUTION_RAD + LASER_MIN_ANGLE_RAD;
		double radius = ranger->ranges[a];

		/* Lasermessung transformieren */
		double x, y;
		if (!transformLaserToMap(angle, radius, pos, &x, &y))
		{
			continue;
		}

		/* Dicke Wände */
		setze_wand_dick(x, y);

		/* Sichtlinie abgreifen */
		double r = 0;
		const double deltaRadius = 0.1;
		x = y = -1;
		do 
		{
			r += deltaRadius;
			double newX, newY;
			transformLaserToMap(angle, r, pos, &newX, &newY);
			if (newX == x && newY == y)
				continue;
			x = newX;
			y = newY;
			setze_gesehen_dick(x, y);
		} while (r < radius - deltaRadius);
	}


   // --------------------------------------------------

   cvSet2D( mapimg,
            MAP_OFFS_Y-(int)(MAP_SCALE*pos->py),
            MAP_OFFS_X+(int)(MAP_SCALE*pos->px),
            CV_RGB(MAX_GRAY,0,0)
          );
   return map_show();
}

int map_show()
{
   if (!initialized) { return 1; }
   cvShowImage( mapwin, mapimg );
   cvWaitKey(40);
   return 0;
}
 
int map_shutdown()
{
   if (!initialized) { return 1; }
   cvDestroyWindow( mapwin );
   initialized=0;
   return 0;
}

