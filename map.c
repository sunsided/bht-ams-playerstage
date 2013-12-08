#include "map.h"
#include "opencv/cv.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#include "memory.h"
#include "assert.h"

#include "laser.h"
#include "transforms.h"

#ifndef MAX_GRAY
#define MAX_GRAY 255
#endif 

static const char* mapwin = "Robot Map";
static const char* testwin = "Map Fill Check";
static IplImage* mapimg  = NULL;     // Image of the map
static IplImage* maptest = NULL;     /* Bild für den Scan-Algorithmus */
static int initialized = 0;

int map_init();
int map_show(void);


/**
* Beschreibung eines Scanline-Segmentes
*/
typedef struct {
	int y;				/*! Y-Koordinate der Scanline */
	int startx;			/*! X-Startkoordinate der Scanline */
	int endx;			/*! X-Startkoordinate der Scanline */
} scanlinerange_t;

/**
* Beschreibung einer Scanline-Kette
*/
typedef struct _scanlinerange_chain {
	scanlinerange_t scanline;		/*! Die Scanline */
	_scanlinerange_chain *next;	/*! Zeiger auf die nächste Scanline */
} scanlinerange_chain_t;


/**
* Ermittelt, ob eine Koordinate auf der Karte kartierter Raum ist.
* \param[in] x Die X-Koordinate in Kartenkoordinaten
* \param[in] y Die Y-Koordinate in Kartenkoordinaten
* \return Null, wenn die Koordinate unkartiert ist, ansonsten nicht-null.
*/
int isCharted(const int x, const int y)
{
	const CvScalar s = cvGet2D(mapimg, y, x);
	
	/* - Grün-Komponente (0x00FF00) entspricht gesehenen Orten 
	 * - Weiß (0xFFFFFF) entspricht Wänden.
	 * Da weiß grün beinhaltet, reicht ein Test auf Grün.
	 *
	 * FIX:
	 * Der befahrene Pfad wird allerdings verbucht, daher muss ein zusätzlicher
	 * Test auf ROT (0x0000FF) erfolgen.
     */
	return s.val[1] > 0 || s.val[2] > 0;
}

/**
* Ermittelt, ob eine Koordinate auf der Karte eine Wand ist.
* \param[in] x Die X-Koordinate in Kartenkoordinaten
* \param[in] y Die Y-Koordinate in Kartenkoordinaten
* \return Null, wenn die Koordinate keine Wand ist, ansonsten nicht-null.
*/
int isWall(const int x, const int y)
{
	if (x < 0 || x >= MAP_SIZE_X) return 0;
	if (y < 0 || y >= MAP_SIZE_Y) return 0;
	const CvScalar s = cvGet2D(mapimg, y, x);
	return (s.val[0] > 0) && (s.val[1] > 0) && (s.val[2] > 0);
}

/**
* Ermittelt, ob eine Koordinate betrachtet werden muss.
* \param[in] x Die X-Koordinate in Kartenkoordinaten
* \param[in] y Die Y-Koordinate in Kartenkoordinaten
* \return Null, wenn die Koordinate nicht besucht werden muss, ansonsten nicht-null.
*/
int shouldBeVisited(const int x, const int y)
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
int markAsVisited(const int x, const int y, const int isCharted)
{
	if (x < 0 || x >= MAP_SIZE_X) return 0;
	if (y < 0 || y >= MAP_SIZE_Y) return 0;
	cvSet2D(maptest, y, x, CV_RGB((isCharted == 0 ? MAX_GRAY : 0), (isCharted == 0 ? MAX_GRAY : 0), MAX_GRAY));

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
* \param[out] nearestUnchartedX Die näheste unkartierte X-Koordinate
* \return Null wenn fehlerfrei, negative Distanz wenn näheste unkartierte Stelle links, sonst positive Distanz.
*/
int buildScanLine(const int scanStartX, const int scanStartY, scanlinerange_t *range, int* nearestUnchartedX)
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

	/* Ergebnis kodieren */
	if (leftUncharted && rightUncharted)
	{
		const int distanceLeft  = scanStartX - startX;
		const int distanceRight = startX - scanStartX;
		if (distanceLeft <= distanceRight)
		{
			*nearestUnchartedX = startX;
			return -distanceLeft;
		}
		
		*nearestUnchartedX = endX;
		return +distanceRight;
	}
	else if (leftUncharted)
	{
		*nearestUnchartedX = startX;
		return -(scanStartX - startX);
	}
	else if (rightUncharted)
	{
		*nearestUnchartedX = endX;
		return (startX - scanStartX);
	}
	
	/* Erfolg. */
	return 0;
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
int checkForOpenSpaces(double startX, double startY)
{
	/* TODO: Ort des Fehlschlags zurückgeben für closest-frontier */

	const int mapy = MAP_OFFS_Y-(int)(MAP_SCALE*startY);
	const int mapx = MAP_OFFS_X+(int)(MAP_SCALE*startX);

	int foundUncharted = 0;
	int nearestUnchartedX = mapx;
	int nearestUnchartedY = mapy;
	int distanceToNearestUncharted = 0;

	/* Testtabelle leeren */
	cvZero(maptest);
	cvShowImage(testwin, maptest);

	/* build the very first scanline */
	scanlinerange_t range;
	int nearest;
	int distanceToUncharted = buildScanLine(mapx, mapy, &range, &nearest);
	/* NOTE: Im Prinzip könnte hier bereits abgebrochen werden, wenn direction != 0.
	 *       Um jedoch den nächsten unkartierten Punkt zu ermitteln, wird das volle
	 *       Programm durchgeführt.
	 */

	if (distanceToUncharted != 0)
	{
		foundUncharted = 1;
		nearestUnchartedX = nearest;
		nearestUnchartedY = mapy;
		distanceToNearestUncharted = labs(distanceToUncharted);
	}
	
	/* Ersten Scaline-Queueintrag erzeugen */
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
		for (int x = startX; x <= endX; ++x)
		{
			/* neue Scanline bilden, wenn noch nicht besucht */
			if (!shouldBeVisited(x, y-1)) continue;
 			distanceToUncharted = buildScanLine(x, y-1, &range, &nearest);
			if (range.startx == range.endx) continue;
			assert(range.y == y-1);

			/* wenn unkartiert gefunden, kürzeste Distanz ermitteln */
			if (distanceToUncharted != 0)
			{
				foundUncharted = 1;

				/* Manhattan-Distanz */
				const int distance = labs(mapy-range.y) + labs(distanceToUncharted);
				if (distance < distanceToNearestUncharted || foundUncharted == 0)
				{
					printf("Näheren Punkt oberhalb gefunden: x=%d, y=%d (Distanz: %d zu %d,%d)\n", nearest, range.y, distance, mapx, mapy);
					distanceToNearestUncharted = distance;
					nearestUnchartedX = nearest;
					nearestUnchartedY = range.y;
					foundUncharted = 1;
				}
			}

			/* Scanline eintüten */
			scanlinerange_chain_t *item = (scanlinerange_chain_t*)malloc(sizeof(scanlinerange_chain_t));
			memcpy(&item->scanline, &range, sizeof(scanlinerange_t));
			item->next = tail->next;
			tail->next = item;
			tail = item;
		}
		
		/* Untere Scanline erweitern */
		for (int x = startX; x <= endX; ++x)
		{
			/* neue Scanline bilden, wenn noch nicht besucht */
			if (!shouldBeVisited(x, y+1)) continue;
			distanceToUncharted = buildScanLine(x, y+1, &range, &nearest);
			if (range.startx == range.endx) continue;
			assert(range.y == y+1);

			/* wenn unkartiert gefunden, kürzeste Distanz ermitteln */
			if (distanceToUncharted != 0)
			{
				/* Manhattan-Distanz */
				const int distance = labs(range.y-mapy) + labs(distanceToUncharted);
				if (distance < distanceToNearestUncharted || foundUncharted == 0)
				{
					printf("Näheren Punkt unterhalb gefunden: x=%d, y=%d (Distanz: %d zu %d,%d)\n", nearest, range.y, distance, mapx, mapy);
					distanceToNearestUncharted = distance;
					nearestUnchartedX = nearest;
					nearestUnchartedY = range.y;
					foundUncharted = 1;
				}
			}

			/* Scanline eintüten */
			scanlinerange_chain_t *item = (scanlinerange_chain_t*)malloc(sizeof(scanlinerange_chain_t));
			memcpy(&item->scanline, &range, sizeof(scanlinerange_t));
			item->next = tail->next;
			tail->next = item;
			tail = item;
		}

		/* Nächste Scanline sichern und head freigeben */
		scanlinerange_chain_t* next = head->next;
		memset(head, 0, sizeof(scanlinerange_chain_t));
		free(head);
		head = next;
	}

	/* Aufräumen */
	while (head != (scanlinerange_chain_t*)0x0)
	{
		printf("!!! Das hätte nicht passieren sollen.\n");
		scanlinerange_chain_t* next = head->next;
		free(head);
		head = next;
	}

	if (foundUncharted)
	{
		printf("Nächster unkartierter Punkt: x=%d, y=%d (Distanz: %d zu %d,%d)\n", 
			nearestUnchartedX, nearestUnchartedY, distanceToNearestUncharted, mapx, mapy);
	}
	else
	{
		printf("Keine unkartierten Punkte gefunden.\n");
	}

	return foundUncharted;
}



int map_init()
{
   if (initialized) { return 1; }
   mapimg  = cvCreateImage(cvSize(MAP_SIZE_X,MAP_SIZE_Y),8,3);
   maptest = cvCreateImage(cvSize(MAP_SIZE_X,MAP_SIZE_Y),8,3);
   cvZero(mapimg);
   // create window
   cvNamedWindow( mapwin, 1 );
   cvNamedWindow( testwin, 1 );
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
	const int width = 2;
	for (int pady = -width/2; pady < width; ++pady) 
	{
		for (int padx = -width/2; padx < width; ++padx)
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
void setze_gesehen_dick(double x, double y, int is_frontier)
{
	const int frontier_value = 92;
	const int seen_value = 64;

	const int width = 2;
	for (int pady = -width/2; pady < width; ++pady) 
	{
		for (int padx = -width/2; padx < width; ++padx)
		{
			CvScalar s = cvGet2D( mapimg,
    	        MAP_OFFS_Y-(int)(MAP_SCALE*y)+pady,
	            MAP_OFFS_X+(int)(MAP_SCALE*x)+padx
				);

			/* Wenn Wand, ignorieren */
			if (s.val[1] == MAX_GRAY)
				continue;

			/* Wenn bereits markiert, ignorieren */
			int green = s.val[1];
			if (green > seen_value)
				continue;

			/* Stärke der Enfärbung */
			if (is_frontier)
				green = frontier_value;
			else
				green = seen_value;

			cvSet2D( mapimg,
			    MAP_OFFS_Y-(int)(MAP_SCALE*y)+pady,
			    MAP_OFFS_X+(int)(MAP_SCALE*x)+padx,
			    CV_RGB(s.val[2], green, s.val[0])
			  );
		}
	}
}


int map_draw(playerc_ranger_t *ranger, playerc_position2d_t *pos)
{
	if (!initialized) { if (map_init()) return 1; }

	cvShowImage(mapwin, mapimg);

   	// Hier kommt der Code zum Zeichnen der Waende hinein
   	// --------------------------------------------------

	for (int a=0; a < (int)ranger->ranges_count; ++a)       
	{
		/* Polarkoordinaten beziehen */
		double angle = a * LASER_ANGULAR_RESOLUTION_RAD + LASER_MIN_ANGLE_RAD;
		double radius = ranger->ranges[a];

		/* Lasermessung transformieren */
		double x, y;
		int is_frontier = transformLaserToMap(angle, radius, pos, &x, &y);

		/* Wand zeichnen, wenn Wert innerhalb Sensorradius */
		if (is_frontier)
		{
			setze_wand_dick(x, y);
		}

		/* Sichtlinie als gesehen markieren */
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
			setze_gesehen_dick(x, y, is_frontier);
		} while (r < radius - deltaRadius);
	}


	checkForOpenSpaces(pos->px, pos->py);


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
   cvShowImage(mapwin, mapimg);
   cvShowImage(testwin, maptest);
   cvWaitKey(40);
   return 0;
}
 
int map_shutdown()
{
	if (!initialized) { return 1; }
	cvDestroyWindow(mapwin);
	cvDestroyWindow(testwin);
	cvReleaseImage(&mapimg);
	cvReleaseImage(&maptest);
	initialized=0;
	return 0;
}

