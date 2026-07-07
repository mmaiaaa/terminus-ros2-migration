# lslidar_c16

## Description
The `lslidar_c16` package is a linux ROS driver for lslidar c16.
The package is tested on Ubuntu 14.04 with ROS indigo.

## Compling
This is a Catkin package. Make sure the package is on `ROS_PACKAGE_PATH` after cloning the package to your workspace. And the normal procedure for compling a catkin package will work.

```
cd your_work_space
catkin_make 
```

## Example Usage

### lslidar_c16_decoder

**Parameters**

`device_ip` (`string`, `default: 192.168.1.200`)

By default, the IP address of the device is 192.168.1.200.

`frame_id` (`string`, `default: laser`)

The frame ID entry for the sent messages.

**Published Topics**

`lslidar_packets` (`lslidar_c16_msgs/LslidarC16Packet`)

Each message corresponds to a lslidar packet sent by the device through the Ethernet.

### lslidar_c16_decoder

**Parameters**

`min_range` (`double`, `0.15`)

`max_range` (`double`, `150.0`)

Points outside this range will be removed.

`frequency` (`frequency`, `10.0`)

Note that the driver does not change the frequency of the sensor. 

`publish_point_cloud` (`bool`, `true`)

If set to true, the decoder will additionally send out a local point cloud consisting of the points in each revolution.

**Published Topics**

`lslidar_sweep` (`lslidar_c16_msgs/LslidarC16Sweep`)

The message arranges the points within each sweep based on its scan index and azimuth.

`lslidar_point_cloud` (`sensor_msgs/PointCloud2`)

This is only published when the `publish_point_cloud` is set to `true` in the launch file.

**Node**

```
roslaunch lslidar_c16_decoder lslidar_c16.launch --screen
```

Note that this launch file launches both the driver and the decoder, which is the only launch file needed to be used.


## FAQ


## Bug Report







