#include "laser.h"
#include "math.h"
#include "transforms.h"

/**
* Transformation von lokalen Koordinaten in Kartenkoordinaten
* \param[in] x		Die X-Koordinate in lokalen Koordinaten
* \param[in] y		Die Y-Koordinate in lokalen Koordinaten
* \param[in] pos	Die Roboterpose im global Frame
* \param[out] mapx	Die X-Koordinate in Kartenkoordinaten
* \param[out] mapy	Die Y-Koordinate in Kartenkoordinaten
*/
void transformLocalToMap(double x, double y, playerc_position2d_t *pos, double *mapx, double *mapy)
{
	double theta = pos->pa;
	*mapx =  cos(theta)*x - sin(theta)*y + pos->px;
	*mapy =  sin(theta)*x + cos(theta)*y + pos->py;
}

/**
* Transformation von Lasermessungen in Kartenkoordinaten
* \param[in] angle  Der gemessene Winkel in Radians
* \param[in] radius Die gemessene Distanz in Metern
* \param[in] pos	Die Roboterpose im global Frame
* \param[out] mapx	Die X-Koordinate in Kartenkoordinaten
* \param[out] mapy	Die Y-Koordinate in Kartenkoordinaten
* \return 0 wenn erfolgreich, 1 wenn die Messung verworfen werden soll
*/
int transformLaserToMap(double angle, double radius, playerc_position2d_t *pos, double *mapx, double *mapy)
{
	/* Entfernungen gleich maximaler Distanz entsorgen */
	if (radius >= LASER_RANGE_MAX - LASER_RANGE_EPSILON)
	{
		return 0;
	}

	/* Transformation von Polarkoordinaten in karthesische Koordinaten */
	double lx = radius * cos(angle);
	double ly = radius * sin(angle);

	/* Transformation in globalen Frame */
	transformLocalToMap(lx, ly, pos, mapx, mapy);

	return 1;
}
