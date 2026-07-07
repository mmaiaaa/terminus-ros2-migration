# Message defining a tracked person
#

uint64      track_id        # unique identifier of the target, consistent over time
bool        is_occluded     # if the track is currently not observable in a physical way
bool        is_matched      # if the track is currently matched by a detection
uint64      detection_id    # id of the corresponding detection in the current cycle (undefined if occluded)
duration    age             # age of the track

# (Pseudo-)probabilistic value (now cvRandomForest'PR 'eg' &gt 1.2) (was: between 0.0 and 1.0) describing the detector's confidence in the detection
float64         leg_probability

float64         tracked_centroid_degree
float64         tracked_centroid_range
# float64         tracked_ellipse_bigaxis
# float64       lateral_halfsize_bigger
float64         lateral_cartesian_m
float64         lateral_halfscope_deg
# float64         lateral_halfscope_rad

# NOTE: lateral_cartesian_m := radius*2.0 'was'p2pDist'
# 
# The following fields are extracted from the Kalman state x and its covariance C

geometry_msgs/PoseWithCovariance       pose   # pose of the track (z value and orientation might not be set, check if corresponding variance on diagonal is > 99999)

geometry_msgs/TwistWithCovariance   twist     # velocity of the track (z value and rotational velocities might not be set, check if corresponding variance on diagonal is > 99999)
