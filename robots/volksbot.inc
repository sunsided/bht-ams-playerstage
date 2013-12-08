
# Desc: Device definitions for Activemedia robots.
# Author: Richard Vaughan, Andrew Howard,  Luis Riazuelo
# Date: 10 Jun 2002
# CVS: $Id: volksbot.inc,v 1.30 2008-01-15 01:25:42 rtv Exp $


define volksbot_base position 
(
  color "yellow"			# Default color.
  drive "diff"		 	# Differential steering model.
  gui_nose 1                  	# Draw a nose on the robot so we can see which way it points
  obstacle_return 1           	# Can hit things.
  laser_return 1                 # reflects laser beams
  ranger_return 0             	# reflects sonar beams
  blob_return 1               	# Seen by blobfinders  
  fiducial_return 1           	# Seen as "1" fiducial finders

  localization "gps"         	  
  localization_origin [0 0 0 0] 	# Start odometry at (0, 0, 0).

  # alternative odometric localization with simple error model
  # localization "odom"         		# Change to "gps" to have impossibly perfect, global odometry
  #  odom_error [ 0.05 0.05 0.1 ]   	# Odometry error or slip in X, Y and Theta
                                   	# (Uniform random distribution)   
)


define bht_volksbot_base_no_sonar volksbot_base
(
  # actual size
  #size [0.390 0.395 0.735]
  #size [1 1 1]

  # the volksbot's center of rotation is offset from its center of area
  #origin [0.083 0 0 0]

  # draw a nose on the robot so we can see which way it points
  gui_nose 1

  # estimated mass in KG
  mass 23.0 
  # differential steering model
  drive "diff"
)


define bht_volksbot_base bht_volksbot_base_no_sonar
(
  # use the sonar array defined above with a small vertical offset to
  # drop the sensors into the robot body
  #p2dx_sonar( pose [0 0 -0.03 0] ) 
)

define volksbot volksbot_base
(
  color "yellow"                        # Default color.
  drive "diff"                  # Differential steering model.

  # actual size
  size [0.390 0.395 0.735]
  origin [-0.083 0 0 0]

  # Hinterstange
  block (
     points 4
     point[0] [ -.1975  .195 ]
     point[1] [ -.1975 -.195 ]
     point[2] [ -.1775 -.195 ]
     point[3] [ -.1775  .195 ]
     z [ 0.084 0.104 ]
  )

  # Vorderstange
  block (
     points 4
     point[0] [ .1775  .195 ]
     point[1] [ .1775 -.195 ]
     point[2] [ .1975 -.195 ]
     point[3] [ .1975  .195 ]
     z [ 0.084 0.104 ]
  )

  # Laser-Scanner-Halterung an Vorderstange
  block (
     points 4
     point[0] [ .185  .01 ]
     point[1] [ .185 -.01 ]
     point[2] [ .190 -.01 ]
     point[3] [ .190  .01 ]
     z [ 0.104 0.144 ]
  )

  # Boden
  block (
     points 4
     point[0] [ -.1775  .140 ]
     point[1] [ -.1775 -.140 ]
     point[2] [  .1775 -.140 ]
     point[3] [  .1775  .140 ]
     z [ 0.084 0.104 ]
  )

  # Motherboard 
  block (
     points 4
     point[0] [ -.17  .085 ]
     point[1] [ -.17 -.085 ]
     point[2] [ -.00 -.085 ]
     point[3] [ -.00  .085 ]
     z [ 0.104 0.114 ]
     color "green"
  )

  # Batterie-Box (Unterseite) 
  block (
     points 4
     point[0] [ .02  .10 ]
     point[1] [ .02 -.10 ]
     point[2] [ .14 -.10 ]
     point[3] [ .14  .10 ]
     z [ 0.104 0.114 ]
     color "gray70"
  )

  # Batterie-Box (Seite Hinten) 
  block (
     points 4
     point[0] [ .02   .10 ]
     point[1] [ .02  -.10 ]
     point[2] [ .025 -.10 ]
     point[3] [ .025  .10 ]
     z [ 0.104 0.169 ]
  )

  # Batterie-Box (Seite Vorne) 
  block (
     points 4
     point[0] [ .135  .10 ]
     point[1] [ .135 -.10 ]
     point[2] [ .14 -.10 ]
     point[3] [ .14  .10 ]
     z [ 0.104 0.169 ]
  )

  # Batterie-Box (Seite Links) 
  block (
     points 4
     point[0] [ .02  .10 ]
     point[1] [ .02  .095 ]
     point[2] [ .14  .095 ]
     point[3] [ .14  .10 ]
     z [ 0.104 0.169 ]
  )

  # Batterie-Box (Seite Rechts) 
  block (
     points 4
     point[0] [ .02 -.095 ]
     point[1] [ .02 -.10 ]
     point[2] [ .14 -.10 ]
     point[3] [ .14 -.095 ]
     z [ 0.104 0.169 ]
  )

  # Batterie
  block (
     points 4
     point[0] [ .03  .075 ]
     point[1] [ .03 -.075 ]
     point[2] [ .13 -.075 ]
     point[3] [ .13  .075 ]
     z [ 0.114 0.214 ]
     color "gray20"
  )

  # Linkes Rad
  block (
     points 4
     point[0] [ 0.034 0.189 ]
     point[1] [ 0.034 0.165 ]
     point[2] [ 0.132 0.165 ]
     point[3] [ 0.132 0.189 ]
     color "black"
     z [ 0.000 0.089 ]
  )

  # Rechtes Rad
  block (
     points 4
     point[0] [ 0.034 -0.165 ]
     point[1] [ 0.034 -0.189 ]
     point[2] [ 0.132 -0.189 ]
     point[3] [ 0.132 -0.165 ]
     z [ 0.000 0.089 ]
  )

  # Stuetzrad
  block (
     points 4
     point[0] [ -0.190  0.010 ]
     point[1] [ -0.190 -0.010 ]
     point[2] [ -0.140 -0.010 ]
     point[3] [ -0.140  0.010 ]
     z [ 0.000 0.050 ]
  )

  # Stuetzrad-Stange
  block (
     points 4
     point[0] [ -0.170  0.010 ]
     point[1] [ -0.170 -0.010 ]
     point[2] [ -0.150 -0.010 ]
     point[3] [ -0.150  0.010 ]
     z [ 0.050 0.084 ]
     color "gray40"
  )

  # Achse
  block (
     points 4
     point[0] [ 0.073  0.165 ]
     point[1] [ 0.073 -0.165 ]
     point[2] [ 0.093 -0.165 ]
     point[3] [ 0.093  0.165 ]
     z [ 0.0345 0.0545 ]
     color "gray40"
  )
 
  # Alu-Stange Rechts Hinten
  block (
     points 4
     point[0] [ -0.158 -0.147 ]
     point[1] [ -0.158 -0.158 ]
     point[2] [ -0.147 -0.158 ]
     point[3] [ -0.147 -0.147 ]
     z [ 0.104 0.555 ]
     color "gray70"
  )

  # Alu-Stange Links Hinten
  block (
     points 4
     point[0] [ -0.158  0.158 ]
     point[1] [ -0.158  0.147 ]
     point[2] [ -0.147  0.147 ]
     point[3] [ -0.147  0.158 ]
     z [ 0.104 0.555 ]
  )

  # Alu-Stange Links Vorne
  block (
     points 4
     point[0] [  0.158  0.147 ]
     point[1] [  0.158  0.158 ]
     point[2] [  0.147  0.158 ]
     point[3] [  0.147  0.147 ]
     z [ 0.104 0.555 ]
  )

  # Alu-Stange Rechts Vorne
  block (
     points 4
     point[0] [  0.158 -0.158 ]
     point[1] [  0.158 -0.147 ]
     point[2] [  0.147 -0.147 ]
     point[3] [  0.147 -0.158 ]
     z [ 0.104 0.555 ]
  )

  # Alu-Stange-Verbindung 1
  block (
     points 4
     point[0] [ -0.158 -0.147 ]
     point[1] [ -0.147 -0.158 ]
     point[2] [  0.000 -0.010 ]
     point[3] [ -0.010  0.000 ]
     z [ 0.545 0.555 ]
  )

  # Alu-Stange-Verbindung 2
  block (
     points 4
     point[0] [  0.147 -0.158 ]
     point[1] [  0.158 -0.147 ]
     point[2] [  0.010  0.000 ]
     point[3] [ -0.000 -0.010 ]
     z [ 0.545 0.555 ]
  )

  # Alu-Stange-Verbindung 3
  block (
     points 4
     point[0] [  0.158  0.147 ]
     point[1] [  0.147  0.158 ]
     point[2] [ -0.000  0.010 ]
     point[3] [  0.010 -0.000 ]
     z [ 0.545 0.555 ]
  )

  # Alu-Stange-Verbindung 4
  block (
     points 4
     point[0] [ -0.147  0.158 ]
     point[1] [ -0.158  0.147 ]
     point[2] [ -0.010 -0.000 ]
     point[3] [  0.000  0.010 ]
     z [ 0.545 0.555 ]
  )

  # camera
  block(
     points 4
     point[0] [ -.05    .05 ]
     point[1] [ -.05   -.05 ]
     point[2] [  .05   -.05 ]
     point[3] [  .05    .05 ]
     z [ 0.555 0.735]
     color "gray30"
  )
)

