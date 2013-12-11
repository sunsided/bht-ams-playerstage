/**
* Erkennung von noch nicht erforschten Bereichen.
*
* Es wird eine abgewandelte Form des Queue-Linear Flood Fill-Algorithmus verwendet,
* wie er z.B. unter http://www.codeproject.com/Articles/16405/Queue-Linear-Flood-Fill-A-Fast-Flood-Fill-Algorith
* beschrieben wird.
*
* Hierbei wird ein zusätzliches OpenCV-Bild als Backend verwendet, in welchem auf Grenzen 
* abgescannte Bereiche blau, sowie gefundene Grenzen weiß markiert werden.
*/

#include "map.h"
#include "opencv/cv.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "limits.h"

extern IplImage* maptest;     /* Bild für den Scan-Algorithmus */

/**
* Beschreibung eines Scanline-Segmentes
*/
typedef struct {
	int y;				/*! Y-Koordinate der Scanline */
	int startx;			/*! X-Startkoordinate der Scanline */
	int endx;			/*! X-Startkoordinate der Scanline */
	int rightUncharted;	/*! Linke Koordinate ist unkartiert */
	int leftUncharted;	/*! Rechte Koordinate ist unkartiert */
} scanlinerange_t;

/**
* Beschreibung einer Scanline-Kette
*/
typedef struct _scanlinerange_chain {
	scanlinerange_t scanline;		/*! Die Scanline */
	_scanlinerange_chain *next;	/*! Zeiger auf die nächste Scanline */
} scanlinerange_chain_t;


/**
* Ermittelt, ob eine Koordinate betrachtet werden muss.
* \param[in] x Die X-Koordinate in Kartenkoordinaten
* \param[in] y Die Y-Koordinate in Kartenkoordinaten
* \return Null, wenn die Koordinate nicht besucht werden muss, ansonsten nicht-null.
*/
static inline int shouldBeVisited(const int x, const int y)
{
	if (x < 0 || x >= MAP_SIZE_X) return 0;
	if (y < 0 || y >= MAP_SIZE_Y) return 0;
	const CvScalar s = cvGet2D(maptest, y, x);
	return (s.val[0] == 0) && !isWall(x, y);
}

/**
* Ermittelt, ob eine Koordinate betrachtet werden muss.
* \param[in] x Die X-Koordinate in Kartenkoordinaten
* \param[in] y Die Y-Koordinate in Kartenkoordinaten
* \return Null, wenn die Koordinate ungültig war, ansonsten nicht-null.
*/
static inline int markAsVisited(const int x, const int y, const int isCharted)
{
	if (x < 0 || x >= MAP_SIZE_X) return 0;
	if (y < 0 || y >= MAP_SIZE_Y) return 0;
	cvSet2D(maptest, y, x, CV_RGB((isCharted == 0 ? MAX_GRAY : 0), (isCharted == 0 ? MAX_GRAY : 0), (isCharted == 0 ? MAX_GRAY : 64)));

#if 0
	cvShowImage(testwin, maptest);
	cvWaitKey(1);
#endif

	return 0;
}

/**
* Erzeugt eine Scanline beginnend an den {x,y}-Koordinaten
* \param[in] scanStartX Die X-Koordinate des Startpunktes in Kartenkoordinaten
* \param[in] scanStartY Die Y-Koordinate des Startpunktes in Kartenkoordinaten
* \param[out] range Die erzeugte Scanline
* \return Anzahl der unkartierten Punkte
*/
int buildScanLine(const int scanStartX, const int scanStartY, scanlinerange_t *range)
{
	int startX = scanStartX;
	int endX   = scanStartX;
	int leftUncharted  = 0;
	int rightUncharted = 0;

	/* Links scannen für Beginn der Scanline */
	for (int x = scanStartX; x >= 0; --x)
	{
		if (!shouldBeVisited(x, scanStartY)) 
		{
			break;
		}

		/* überprüfen, ob Ort unkartiert */
		int charted = isCharted(x, scanStartY);

		/* als besucht markieren und fortfahren */
		markAsVisited(x, scanStartY, charted);

		if (!charted)
		{
			leftUncharted = 1;
			break;
		}
		startX = x;
	}

	/* Rechts scannen für Beginn der Scanline */
	for (int x = scanStartX+1; x < MAP_SIZE_X; ++x)
	{
		if (!shouldBeVisited(x, scanStartY)) 
		{
			break;
		}

		/* überprüfen, ob Ort unkartiert */
		int charted = isCharted(x, scanStartY);

		/* als besucht markieren und fortfahren */
		markAsVisited(x, scanStartY, charted);

		if (!charted)
		{
			rightUncharted = 1;
			break;
		}
		endX = x;
	}

	/* Scanline bauen */
	range->y = scanStartY;	
	range->startx = startX;
	range->endx = endX;
	range->leftUncharted = leftUncharted;
	range->rightUncharted = rightUncharted;
	
	return leftUncharted + rightUncharted;
}

/**
* Berechnet die Distanz zweier Punkte
* \param[in] referenceX X-Koordinate 1
* \param[in] referenceY Y-Koordinate 1
* \param[in] testX X-Koordinate 2
* \param[in] testY Y-Koordinate 2
* \return Die Distanz
*/
static inline int getDistance(const int referenceX, const int referenceY, const int testX, const int testY)
{
	/* Manhattan-Distanz */
	return labs(testX - referenceX) + labs(testY - referenceY);
}

/**
* Vergleicht zwei Punkte bezüglich eines Referenzpunktes und liefert den näheren, sowie die Distanz.
* \param[in] referenceX Referenz-X-Koordinate
* \param[in] referenceY Referenz-Y-Koordinate
* \param[in] testX X-Koordinate 1
* \param[in] testY Y-Koordinate 1
* \param[inout] testX X-Koordinate 2, wird ersetzt durch den näheren Punkt
* \param[inout] testY Y-Koordinate 2, wird ersetzt durch den näheren Punkt
* \return Die Distanz
*/
static inline int getNearest(const int referenceX, const int referenceY, const int testX, const int testY, int& secondX, int& secondY)
{
	const int testDistance = getDistance(referenceX, referenceY, testX, testY);
	const int distance     = getDistance(referenceX, referenceY, secondX, secondY);
	if (testDistance < distance)
	{
		secondX = testX;
		secondY = testY;
		return testDistance;
	}
	return distance;
}

/**
* Erzeugt neue Scanlines im Bereich {\see startX}..{\see endX} in der gegebenen Y-Koordinate
* und hängt sie in die gegebene linked list ein.
* \param[in] startX Start-X-Koordinate
* \param[in] endX   End-X-Koordinate
* \param[in] y      Neue Y-Koordinate
* \param[in] mapx   Aktuelle X-Koordinate des Roboters auf der Karte
* \param[in] mapy   Aktuelle Y-Koordinate des Roboters auf der Karte
* \param[inout] tail Tail der linked list
* \param[inout] nearestUnchartedX X-Koordinate des nähesten unkartierten Punktes
* \param[inout] nearestUnchartedY Y-Koordinate des nähesten unkartierten Punktes
* \param[out]   distanceToNearestUncharted Distanz zum nähesten unkartierten Punkt
* \return Anzahl der gefundenen, unkartierten Punkte in den neuen Scanline-Segmenten.
*/
uint32_t extendScanLine(const int startX, const int endX, const int y, const int mapx, const int mapy, scanlinerange_chain_t **tail, int *nearestUnchartedX, int *nearestUnchartedY, int *distanceToNearestUncharted)
{
	scanlinerange_t range;
	uint32_t foundUncharted = 0;

	for (int x = startX; x <= endX; ++x)
	{
		/* neue Scanline bilden, wenn noch nicht besucht */
		if (!shouldBeVisited(x, y)) continue;
		uint32_t unchartedCount = buildScanLine(x, y, &range);
		assert(range.y == y);

		/* wenn unkartiert gefunden, kürzeste Distanz ermitteln */
		if (unchartedCount != 0)
		{
			foundUncharted += unchartedCount;

			if (range.leftUncharted)
			{
				*distanceToNearestUncharted = getNearest(mapx, mapy, range.startx, range.y, *nearestUnchartedX, *nearestUnchartedY);
			}
			if (range.rightUncharted)
			{
				*distanceToNearestUncharted = getNearest(mapx, mapy, range.endx, range.y, *nearestUnchartedX, *nearestUnchartedY);
			}
		}

		/* Registrierung verhindern, da sonst bleedout in vertikaler Richtung*/
		if (range.startx == range.endx) continue;

		/* Scanline eintüten */
		scanlinerange_chain_t *item = (scanlinerange_chain_t*)malloc(sizeof(scanlinerange_chain_t));
		memcpy(&item->scanline, &range, sizeof(scanlinerange_t));
		item->next = (*tail)->next;
		(*tail)->next = item;
		(*tail) = item;
	}

	return foundUncharted;
}

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
int checkForOpenSpaces(const double startX, const double startY, double *outNearestX, double* outNearestY)
{
	/* TODO: Ort des Fehlschlags zurückgeben für closest-frontier */

	const int mapy = MAP_OFFS_Y-(int)(MAP_SCALE*startY);
	const int mapx = MAP_OFFS_X+(int)(MAP_SCALE*startX);

	int foundUncharted = 0;
	int nearestUnchartedX = INT_MAX/4;
	int nearestUnchartedY = INT_MAX/4;
	int distanceToNearestUncharted = 0;

	/* Testtabelle leeren */
	cvZero(maptest);

	/* Erste Scanline erzeugen */
	scanlinerange_t range;
	int unchartedCount = buildScanLine(mapx, mapy, &range);
	/* NOTE: Im Prinzip könnte hier bereits abgebrochen werden, wenn direction != 0.
	 *       Um jedoch den nächsten unkartierten Punkt zu ermitteln, wird das volle
	 *       Programm durchgeführt.
	 */

	if (unchartedCount != 0)
	{
		foundUncharted += unchartedCount;

		if (range.leftUncharted)
		{
			distanceToNearestUncharted = getNearest(mapx, mapy, range.startx, range.y, nearestUnchartedX, nearestUnchartedY);
		}
		if (range.rightUncharted)
		{
			distanceToNearestUncharted = getNearest(mapx, mapy, range.endx, range.y, nearestUnchartedX, nearestUnchartedY);
		}
	}

	/* Ersten Scanline-Queueintrag erzeugen */
	scanlinerange_chain_t *head = (scanlinerange_chain_t*)malloc(sizeof(scanlinerange_chain_t));
	memcpy(&head->scanline, &range, sizeof(scanlinerange_t));
	head->next = (scanlinerange_chain_t*)0x0;
	scanlinerange_chain_t *tail = head;

	/* solange durchlaufen, bis queue leer wird */
	while (head != (scanlinerange_chain_t*)0x0)
	{
		/* Scanline-Werte extrahieren */		
		const int startX = head->scanline.startx;
		const int endX   = head->scanline.endx;
		const int y      = head->scanline.y;

		/* Obere Scanline erweitern */
		foundUncharted += extendScanLine(startX, endX, y-1, mapx, mapy, &tail, 
										 &nearestUnchartedX, &nearestUnchartedY, &distanceToNearestUncharted);
		
		/* Untere Scanline erweitern */
		foundUncharted += extendScanLine(startX, endX, y+1, mapx, mapy, &tail, 
										 &nearestUnchartedX, &nearestUnchartedY, &distanceToNearestUncharted);

		/* Nächste Scanline sichern und head freigeben */
		scanlinerange_chain_t* next = head->next;
		memset(head, 0, sizeof(scanlinerange_chain_t));
		free(head);
		head = next;
	}

	/* Aufräumen */
	assert (head == (scanlinerange_chain_t*)0x0);

	/* Punkte ausgeben */
	if (foundUncharted)
	{
		if (outNearestX != (double*)0 && outNearestY != (double*)0)
		{
			*outNearestX = (nearestUnchartedX-MAP_OFFS_X)/MAP_SCALE;
			*outNearestY = (MAP_OFFS_Y-nearestUnchartedY)/MAP_SCALE;
		}
	}

#if VERBOSE
	if (foundUncharted)
	{
		printf("%d unkartierte. Nähester: x=%d, y=%d (Distanz: %d)\n", 
			foundUncharted, nearestUnchartedX, nearestUnchartedY, distanceToNearestUncharted);

		if (outNearestX != (double*)0 && outNearestY != (double*)0)
		{
			*outNearestX = (nearestUnchartedX-MAP_OFFS_X)/MAP_SCALE;
			*outNearestY = (MAP_OFFS_Y-nearestUnchartedY)/MAP_SCALE;
		}
	}
	else
	{
		printf("Keine unkartierten Punkte gefunden.\n");
	}
#endif

	return foundUncharted;
}

