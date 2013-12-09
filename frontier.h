/**
* Erkennung von noch nicht erforschten Bereichen.
*/

#ifndef FRONTIER_H
#define FRONTIER_H

/**
* Überprüft, ob die Karte offene Bereiche beinhaltet.
*
* Dieser Algorithmus ist eine Abwandlung des Queue-Linear Flood Fill,
* wobei Wände ("weiß") und gesehene Bereiche ("grün") als positiv, 
* leere Bereiche ("schwarz") hingegen als Fehlschlag gewertet werden. 
* Wird kein leerer Bereich gefunden, ist die gesamte (erreichbare) Karte 
* gesehen worden.
*
* \param[in] startX Die X-Koordinate des Startpunktes in Weltkoordinaten
* \param[in] startY Die Y-Koordinate des Startpunktes in Weltkoordinaten
* \return Null, wenn die Karte voll abgedeckt ist oder nicht-Null, 
*         wenn offene Bereiche existieren.
*/
int checkForOpenSpaces(const double startX, const double startY, double *outNearestX, double* outNearestY);

#endif
