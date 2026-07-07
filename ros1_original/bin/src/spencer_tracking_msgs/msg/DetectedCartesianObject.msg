# Message describing a detection of a person
#

# Unique id of the detection, monotonically increasing over time
uint64          detection_id

# (Pseudo-)probabilistic value (now cvRandomForest'PR 'eg' &gt 1.2) (was: between 0.0 and 1.0) describing the detector's confidence in the detection
float64         confidence

# 3D pose (position + orientation) of the *center* of the detection
# check covariance to see which dimensions are actually set! unset dimensions shall have a covariance > 9999
geometry_msgs/PoseWithCovariance    pose    

float64         detected_centroid_degree
float64         detected_centroid_range
# float64         detected_ellipse_bigaxis
# float64       lateral_halfsize_bigger
float64         lateral_cartesian_m
float64         lateral_halfscope_deg
# float64         lateral_halfscope_rad

# NOTE: lateral_cartesian_m := radius*2.0 'was'p2pDist'
# 
# Sensor modality / detector type, see example constants below. 
# used e.g. later in tracking to check which tracks have been visually confirmed
string          modality            

                                    
string          MODALITY_GENERIC_LASER_2D = laser2d
string          MODALITY_GENERIC_LASER_3D = laser3d
string          MODALITY_GENERIC_MONOCULAR_VISION = mono
string          MODALITY_GENERIC_STEREO_VISION = stereo
string          MODALITY_GENERIC_RGBD = rgbd
