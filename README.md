bht-ams-playerstage
===================

Kurs Autonome Mobile Systeme an der Beuth Hochschule für Technik. Simulation eines VolksBot mit Laserranger zwecks SLAM.

### Erster Ansatz zur Exploration ###

Ein früher Ansatz mittels vermaschter P-Regler zum Steuern der Bahn- und Winkelgeschwindigkeit in Abhängigkeit von der Nähe und Entfernung zu Hindernissen.

[![Exploration Demo Video](http://img.youtube.com/vi/eAbF3QBGwzA/0.jpg)](http://www.youtube.com/watch?v=eAbF3QBGwzA)

### Stage ###

Das Set-Up: Schlupf- und messfehlerfreier Roboter mit Differentialantrieb und v-omega-Steuerung.

![Stage](https://raw.github.com/sunsided/bht-ams-playerstage/feature/frontiers-1/images/frontiers-1/stage.png)

### Karte ###

Die vom Roboter erstellte Karte. Der gelbe Vektor weist auf die näheste unerforschte Grenze, wobei die Manhattan-Distanz ohne Betrachtung von Hindernissen verwendet wurde.

![Map](https://raw.github.com/sunsided/bht-ams-playerstage/feature/frontiers-1/images/frontiers-1/map.png)

### Frontiers ###

Mittels des Queue-Linear Flood Fill-Algorithmus erkannte Wissensgrenzen (weiß) zeigen Bereiche, die vom Roboter noch nicht gescannt wurden. Treten keine Grenzen mehr in der Karte auf, ist der Bereich vollständig erkundet.

![Frontiers](https://raw.github.com/sunsided/bht-ams-playerstage/feature/frontiers-1/images/frontiers-1/frontiers.png)
