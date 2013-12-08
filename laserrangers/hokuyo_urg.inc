define hokuyosensor sensor
(
  # laser-specific properties
  # factory settings for Hokuyo URG-04LX
  range [ 0.03 4.0 ]
  pose [ 0 0 0.035 0 ]
  color_rgba [ 0 0 1 0.15 ]
  fov 240.0
  samples 681

  #samples 90 # still useful but much faster to compute
)

define urglaser ranger 
(
  hokuyosensor()
  # generic model properties
  color "grey80"
  size [ 0.049 0.049 0.071 ] # dimensions of the Hokuyo URG-04LX
  origin [ 0 0 -0.01775 0 ]

  # Grauer Wuerfel
  block (
    points 4
    point[0] [ -0.0245  0.0245 ]
    point[1] [ -0.0245 -0.0245 ]
    point[2] [  0.0245 -0.0245 ]
    point[3] [  0.0245  0.0245 ]
    z [0 0.040 ]
  )

  # Infrarotfenster
  block (
    points 8
    point[0] [ -0.010  0.020 ]
    point[1] [ -0.020  0.010 ]
    point[2] [ -0.020 -0.010 ]
    point[3] [ -0.010 -0.020 ]
    point[4] [  0.010 -0.020 ]
    point[5] [  0.020 -0.010 ]
    point[6] [  0.020  0.010 ]
    point[7] [  0.010  0.020 ]
    z [0.040 0.071 ]
    color "black"
  )
)


