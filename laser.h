/**
* Defines für den Laser-Ranger
*
* Hokuyo URG-04LX: fov 240°, 681 samples
*/

#ifndef LASER_H
#define LASER_H

#include <math.h>

/**
* Field-Of-View in Grad
*/
#define LASER_FOV_DEG (240.0)

/**
* Samples in FOV
*/
#define LASER_SAMPLES (681)

/**
* Maximale Messung in Metern
*/
#define LASER_RANGE_MIN (0.35)

/**
* Minimale Messung in Metern
*/
#define LASER_RANGE_MAX (4.0)

/**
* Epsilonwert für Distanzvergleiche
*/
#define LASER_RANGE_EPSILON (0.001)

/**
* Umwandlung von Winkel in Sample
*/
#define LASER_RANGEINDEX_FROM_ANGLE_DEG(degree) \
	((int)((degree/(LASER_FOV_DEG/2)) * LASER_SAMPLES/2 + LASER_SAMPLES/2))

/**
* Umwandlung von Sample in Winkel
*/
#define LASER_DEGREE_FROM_RANGEINDEX(index) \
	((double)(index*LASER_ANGULAR_RESOLUTION_DEG+LASER_MIN_ANGLE_DEG))

/**
* Field-Of-View in Radians
*/
#define LASER_FOV_RAD (LASER_FOV_DEG * M_PI / 180.0)

/**
* Winkelauflösung in Grad
*/
#define LASER_ANGULAR_RESOLUTION_DEG (LASER_FOV_DEG / LASER_SAMPLES)

/**
* Winkelauflösung in Radians
*/
#define LASER_ANGULAR_RESOLUTION_RAD (LASER_FOV_RAD / LASER_SAMPLES)

/**
* Maximaler Öffnungswinkel in Radians
*/
#define LASER_MAX_ANGLE_RAD (LASER_FOV_RAD/2.0)

/**
* Minimaler Öffnungswinkel in Radians
*/
#define LASER_MIN_ANGLE_RAD (-LASER_MAX_ANGLE_RAD)

/**
* Maximaler Öffnungswinkel in Degree
*/
#define LASER_MAX_ANGLE_DEG (LASER_FOV_DEG/2.0)

/**
* Minimaler Öffnungswinkel in Degree
*/
#define LASER_MIN_ANGLE_DEG (-LASER_MAX_ANGLE_DEG)

#endif
