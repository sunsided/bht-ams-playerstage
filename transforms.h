#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include <libplayerc/playerc.h>

/**
* Lineare Interpolation von einer gegebenen Domain in einen neuen Wertebereich
* \param[in] domainMin Minimaler Wert von x
* \param[in] domainMax Maximaler Wert von x
* \param[in] x Zu interpolierender Wert
* \param[in] targetMin Minimaler Wert in Zieldomain
* \param[in] targetMax Maximaler Wert in Zieldomain
*/
#define LERP(domainMin, domainMax, x, targetMin, targetMax) \
	((x-domainMin)/(domainMax-domainMin) * targetMax + targetMin)

/**
* Lineare Interpolation mit Sättigung von einer gegebenen Domain in einen neuen Wertebereich
* \param[in] domainMin Minimaler Wert von x
* \param[in] domainMax Maximaler Wert von x
* \param[in] x Zu interpolierender Wert
* \param[in] targetMin Minimaler Wert in Zieldomain
* \param[in] targetMax Maximaler Wert in Zieldomain
*/
#define LERP_SATURATE(domainMin, domainMax, x, targetMin, targetMax) \
	((((x > domainMax) ? domainMax : ((x < domainMin) ? domainMin : x)) -domainMin)/(domainMax-domainMin) * targetMax + targetMin)

/**
* Transformation von lokalen Koordinaten in Kartenkoordinaten
* \param[in] x		Die X-Koordinate in lokalen Koordinaten
* \param[in] y		Die Y-Koordinate in lokalen Koordinaten
* \param[in] pos	Die Roboterpose im global Frame
* \param[out] mapx	Die X-Koordinate in Kartenkoordinaten
* \param[out] mapy	Die Y-Koordinate in Kartenkoordinaten
*/
void transformLocalToMap(double x, double y, const playerc_position2d_t *const pos, double *mapx, double *mapy);

/**
* Transformation von Lasermessungen in Kartenkoordinaten
* \param[in] angle  Der gemessene Winkel in Radians
* \param[in] radius Die gemessene Distanz in Metern
* \param[in] pos	Die Roboterpose im global Frame
* \param[out] mapx	Die X-Koordinate in Kartenkoordinaten
* \param[out] mapy	Die Y-Koordinate in Kartenkoordinaten
* \return 0 wenn erfolgreich, 1 wenn die Messung verworfen werden soll
*/
int transformLaserToMap(double angle, double radius, const playerc_position2d_t *const pos, double *mapx, double *mapy);

#endif
