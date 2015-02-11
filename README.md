bht-ams-playerstage
===================

Autonomous Mobile Systems class at Beuth Hochschule für Technik, Berlin. Simulation of a [VolksBot](http://www.volksbot.de/) with (error-free) laser rangers.

### Approach to exploration ###

This implements a simple approach using meshed P controllers for forward and angular velocity in dependance of the distance to the next obstacle. 

You can watch a demo video [here](http://www.youtube.com/watch?v=eAbF3QBGwzA).

[![Exploration Demo Video](http://img.youtube.com/vi/eAbF3QBGwzA/0.jpg)](http://www.youtube.com/watch?v=eAbF3QBGwzA)

### Stage / robot setup ###

The robot is modeled without slippage and measurement errors and sports a differential drive with v-omega control.

![Stage](https://raw.github.com/sunsided/bht-ams-playerstage/feature/frontiers-1/images/frontiers-1/stage.png)

### Mapping ###

This shows the map created by the robot, as well as the past trajectory. The yellow vectors points at the nearest unexplored boundary, using a Manhattan distance measure without paying attention to obstacles. As such, it is measured in air distance, which might be used as a heuristic for A* later on.

![Map](https://raw.github.com/sunsided/bht-ams-playerstage/feature/frontiers-1/images/frontiers-1/map.png)

### Frontiers and algorithm termination ###

This program implements a frontied-based approach to exploration. A queue-linear flood fill algorithm is used to determine knowledge boundaries (white), i.e. areas that have not been scanned by the robot. The exploration algorithm terminates if no frontiers are left, meaning that the whole terrain has been explored. 

![Frontiers](https://raw.github.com/sunsided/bht-ams-playerstage/feature/frontiers-1/images/frontiers-1/frontiers.png)
