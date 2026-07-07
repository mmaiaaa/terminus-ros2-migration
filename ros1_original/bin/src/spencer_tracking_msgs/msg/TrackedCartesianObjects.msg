# Message with all currently tracked persons 
#

Header              header      # Header containing timestamp etc. of this message
uint16              num_tracks
TrackedCartesianObject[]     tracks      # All persons that are currently being tracked
