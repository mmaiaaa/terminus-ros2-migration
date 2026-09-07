Terminus Robot — ROS 2 Autonomous Security Robot Platform

Overview

Terminus Robot is the ROS 2 software platform and hardware-integration project for the Terminus Titan / Anbot autonomous security and patrol robot.

The project is focused on migrating the robot from its original legacy Ubuntu 16.04 + ROS Kinetic software stack to a modern Ubuntu 24.04 + ROS 2 Jazzy architecture while preserving and reusing as much of the existing robot hardware as practical.

The robot is a mobile security platform equipped with a differential-drive chassis, wheel motors, motor controllers, LiDAR, onboard computing, CAN-based chassis electronics, and supporting sensors.

The long-term objective is to provide a completely local and maintainable robotics stack capable of:

* Manual and autonomous navigation
* LiDAR-based perception
* SLAM and local mapping
* Localization
* Autonomous patrol
* Obstacle detection and avoidance
* Wheel odometry
* Chassis control
* Sensor integration
* ROS 2 visualization and diagnostics
* Local operation without dependency on the discontinued cloud infrastructure

⸻

Project Status

The project is currently in the ROS 2 migration and hardware-integration phase.

Several major components have already been migrated and validated.

Completed / Validated

* ROS 2 Jazzy development environment established
* ROS 2 workspace created
* Original robot model migrated to ROS 2
* anbot_model_config migrated to ament
* Xacro/URDF generation validated
* URDF passed check_urdf
* Robot model successfully visualized in RViz
* Robot link/frame tree validated
* base_footprint, base_link, and active sensor frames established
* RPLIDAR mesh successfully resolved
* robot_state_publisher successfully initialized
* CAN communication investigated and validated
* Existing CAN hardware identified
* Chassis CAN protocol reverse-engineered sufficiently for wheel feedback
* anbot_chassis_protocol implemented
* anbot_chassis_transport implemented
* anbot_chassis_driver implemented
* CAN replay/testing infrastructure implemented
* Physical CAN data successfully processed by the ROS 2 chassis node
* /chassis/odom validated with physical robot data
* /chassis/wheel_speeds validated
* odom → base_footprint → base_link transform relationship established
* Full onboard eMMC backup performed for system preservation

Current Development Focus

The next major stage is completing the transition from ROS 2 software validation to full physical robot operation.

This includes:

* Finalizing chassis command/control
* Validating motor command interfaces
* Validating wheel direction and polarity
* Integrating LiDAR
* Integrating localization
* Integrating SLAM
* Testing navigation
* Establishing safety behavior
* Running controlled physical driving tests
* Moving the final ROS 2 stack onto dedicated robot hardware

⸻

System Architecture

The intended software architecture is:

                    ┌─────────────────────┐
                    │     ROS 2 Jazzy     │
                    │      Ubuntu 24      │
                    └──────────┬──────────┘
                               │
              ┌────────────────┼────────────────┐
              │                │                │
              ▼                ▼                ▼
       ┌─────────────┐  ┌─────────────┐  ┌─────────────┐
       │   Sensors   │  │ Navigation  │  │   Control   │
       │             │  │             │  │             │
       │ LiDAR       │  │ SLAM        │  │ Cmd Vel     │
       │ IMU         │  │ Localization│  │ Chassis     │
       │ Encoders    │  │ Nav2        │  │ Motor Ctrl  │
       └──────┬──────┘  └─────────────┘  └──────┬──────┘
              │                                  │
              │                                  │
              ▼                                  ▼
       ┌──────────────────────────────────────────────┐
       │              ROS 2 Robot Layer               │
       │                                              │
       │ anbot_model_config                            │
       │ anbot_chassis_protocol                        │
       │ anbot_chassis_transport                       │
       │ anbot_chassis_driver                          │
       └──────────────────────┬───────────────────────┘
                              │
                       ┌──────┴──────┐
                       │ CAN / UART  │
                       │ Interfaces  │
                       └──────┬──────┘
                              │
                              ▼
                    ┌──────────────────┐
                    │ Robot Electronics│
                    │                  │
                    │ CAN Controllers  │
                    │ Motor Controllers│
                    │ Wheel Motors     │
                    └──────────────────┘

⸻

Hardware

Robot Platform

The Terminus platform uses a differential-drive mobile base.

The primary hardware includes:

* Two drive wheels
* Differential-drive chassis
* Brushless DC wheel motors
* Motor controllers
* CAN bus interfaces
* LiDAR
* Onboard computer
* Supporting sensors
* Battery/power system
* UART interfaces

The original robot computer is based on an RK3399 ARM64 platform.

The original computer runs the legacy software stack and is being evaluated for reuse. A separate ROS 2-compatible computer is also being considered for the final architecture to reduce risk and provide a clean modern software environment.

⸻

Software Environment

Target Platform

Operating System:
Ubuntu 24.04 LTS
ROS:
ROS 2 Jazzy Jalisco
Architecture:
ARM64 / x86_64 compatible where possible
Build System:
colcon
Build Type:
ament / CMake

The legacy platform was:

Ubuntu 16.04 LTS
ROS Kinetic

ROS Kinetic and Ubuntu 16.04 are no longer suitable as the primary development platform, so the project is migrating the robot to ROS 2 Jazzy.

⸻

Repository Structure

The ROS 2 workspace is organized around separate functional packages.

TerminusRobot/
└── ros2_ws/
    ├── src/
    │   ├── anbot_model_config/
    │   │
    │   ├── anbot_chassis_protocol/
    │   │
    │   ├── anbot_chassis_transport/
    │   │
    │   └── anbot_chassis_driver/
    │
    ├── build/
    ├── install/
    └── log/

The build/, install/, and log/ directories are generated by the ROS 2 build system and normally should not be committed to source control.

⸻

ROS 2 Packages

anbot_model_config

This package contains the robot’s physical model and ROS 2 description configuration.

Responsibilities include:

* URDF
* Xacro
* Robot links
* Joints
* Sensor frames
* Meshes
* Visual geometry
* Collision geometry
* Inertial properties
* TF frame relationships
* RViz visualization configuration
* Robot State Publisher configuration

The model has been successfully migrated to ROS 2.

The resulting robot description has been validated using:

check_urdf

The model contains a connected TF tree rooted at:

base_footprint

with the primary body frame:

base_link

The active sensor frames are attached to the appropriate robot links.

⸻

Chassis Communication Architecture

The chassis communication system is divided into three layers.

anbot_chassis_driver
        │
        ▼
anbot_chassis_transport
        │
        ▼
anbot_chassis_protocol
        │
        ▼
CAN / SocketCAN
        │
        ▼
Robot Chassis Controller

This separation is intentional.

Protocol Layer

anbot_chassis_protocol

Responsible for decoding and encoding the robot’s chassis CAN protocol.

It should contain protocol-specific logic without depending directly on ROS.

This allows CAN frames to be tested independently from ROS 2.

⸻

Transport Layer

anbot_chassis_transport

Responsible for communication with the physical transport layer.

The current development uses Linux CAN / SocketCAN.

Example interface:

can0

The transport layer provides the raw CAN frames to the protocol/driver layers.

⸻

Driver Layer

anbot_chassis_driver

Provides the ROS 2 interface to the chassis.

The driver is responsible for:

* Initializing the chassis communication interface
* Reading wheel feedback
* Publishing wheel speeds
* Publishing odometry
* Processing chassis commands
* Maintaining ROS 2 timing
* Managing TF-related data
* Handling diagnostics
* Connecting ROS 2 topics to the underlying chassis protocol

⸻

CAN Bus

The robot uses CAN communication for chassis data.

Linux SocketCAN is used as the ROS 2 interface.

Example:

ip link show can0

The CAN interface is configured as:

can0

The chassis configuration is stored in:

chassis_can.yaml

Example configuration:

can_interface: can0
odom_frame_id: odom
base_frame_id: base_link
poll_period_ms: 10

⸻

CAN Protocol

During reverse engineering of the original chassis communication, wheel-speed feedback was identified.

A known wheel feedback frame uses:

CAN ID: 0x184

The frame contains wheel-related data in the payload.

Current decoding:

Bytes 0–1 → Left wheel speed
Bytes 2–3 → Right wheel speed

Another relevant wheel-related CAN frame has been observed at:

CAN ID: 0x194

The exact meaning and complete protocol of all chassis CAN frames should continue to be documented as additional reverse engineering and physical testing are performed.

Do not assume undocumented CAN IDs are safe to transmit to the physical robot.

⸻

CAN Data Capture and Replay

A CAN capture/replay workflow was developed to allow chassis software to be tested without continuously operating the physical robot.

CAN traffic can be captured using tools such as:

candump

Example:

candump can0

Captured CAN traffic can then be replayed through the development tooling.

The repository includes a candump_replay utility for reproducing captured CAN traffic.

This is particularly useful for:

* Protocol development
* Regression testing
* Debugging
* ROS 2 driver development
* Reproducing physical robot behavior
* Testing without repeatedly moving the robot

⸻

ROS 2 Topics

The chassis driver currently exposes chassis data through ROS 2 topics.

Odometry

/chassis/odom

Message type:

nav_msgs/msg/Odometry

This topic provides:

* Position
* Orientation
* Linear velocity
* Angular velocity
* Odometry frame information

⸻

Wheel Speeds

/chassis/wheel_speeds

Message type:

std_msgs/msg/Int16MultiArray

This topic provides the decoded wheel-speed information from the chassis controller.

The exact units and sign convention should remain documented alongside the protocol implementation and must be verified against physical wheel movement before being used for high-level navigation.

⸻

TF Frame Architecture

The current robot frame structure follows:

odom
  │
  ▼
base_footprint
  │
  ▼
base_link
  │
  ├── sensors
  │
  ├── lidar
  │
  └── other robot components

The distinction between base_footprint and base_link is intentional.

base_footprint represents the robot’s planar footprint/reference frame.

base_link represents the robot body frame.

This structure is intended to remain compatible with standard ROS 2 navigation and localization tooling.

⸻

Robot Model Validation

The robot model has undergone several validation steps.

Xacro Generation

The Xacro files successfully generate a complete robot description.

Example:

ros2 run xacro xacro path/to/robot.urdf.xacro

⸻

URDF Validation

The generated URDF has been tested using:

check_urdf

The model passed structural validation.

⸻

RViz Validation

The robot model has been successfully loaded into RViz.

Example:

ros2 launch anbot_model_config display.launch.py

The model loads without unresolved Xacro expressions or missing active RPLIDAR mesh references.

⸻

Chassis Physical Validation

The ROS 2 chassis driver has already been tested against physical CAN traffic from the robot.

A physical test produced valid odometry information including:

x ≈ -0.698 m
y ≈  0.816 m
theta ≈ 1.700 rad

The exact values are dependent on the robot’s current chassis state and should not be interpreted as fixed calibration values.

Wheel-speed feedback was also successfully processed while the robot was stationary.

This confirms that the software stack can successfully receive and interpret physical chassis data through the CAN interface.

⸻

Diagnostics

The ROS 2 chassis system is intended to integrate with standard ROS 2 diagnostic mechanisms.

The robot has previously been tested using:

ros2 topic echo /diagnostics --once

Diagnostics should eventually expose useful system health information such as:

* CAN connection status
* Chassis communication status
* Sensor status
* Driver status
* Communication timeouts
* Invalid frames
* Hardware errors
* Motor controller status

⸻

Development Setup

Clone the repository:

git clone <repository-url>
cd TerminusRobot

Enter the ROS 2 workspace:

cd ros2_ws

Source ROS 2 Jazzy:

source /opt/ros/jazzy/setup.bash

Build the workspace:

colcon build --symlink-install

Source the workspace:

source install/setup.bash

For convenience, both can be added to the user’s shell configuration.

⸻

Building Individual Packages

A specific package can be built using:

colcon build --symlink-install --packages-select anbot_model_config

For the chassis packages:

colcon build --symlink-install \
  --packages-select \
  anbot_chassis_protocol \
  anbot_chassis_transport \
  anbot_chassis_driver

After rebuilding:

source install/setup.bash

⸻

Model Visualization

Launch the robot model:

ros2 launch anbot_model_config display.launch.py

Then open RViz if it is not automatically launched.

The model should display the robot geometry and its TF structure.

Before physical operation, verify:

* Robot orientation
* Wheel positions
* Sensor positions
* LiDAR position
* base_link
* base_footprint
* Sensor frame names
* Joint orientations

⸻

Running the Chassis Driver

The exact launch command should follow the launch files included in the current version of the chassis driver.

The expected configuration is based on:

can0

with:

odom → base_link

as the primary odometry relationship.

Before launching against the physical robot, verify that the CAN interface exists:

ip link show can0

and that CAN traffic is present:

candump can0

Never transmit unverified CAN commands to the physical chassis.

⸻

Testing Philosophy

Development of the Terminus platform follows a staged hardware-in-the-loop approach.

Software-only testing
        ↓
URDF / TF validation
        ↓
Recorded CAN replay
        ↓
Physical CAN receive testing
        ↓
Stationary robot testing
        ↓
Low-speed wheel testing
        ↓
Manual driving
        ↓
Odometry validation
        ↓
LiDAR validation
        ↓
Localization
        ↓
SLAM
        ↓
Navigation
        ↓
Autonomous patrol

Each stage should be validated before progressing to the next.

⸻

Safety

This repository interfaces with a physical mobile robot.

Testing should be performed progressively.

Before enabling motor commands:

1. Lift or otherwise safely secure the drive wheels where appropriate.
2. Confirm emergency-stop functionality.
3. Confirm motor direction.
4. Confirm left/right wheel identification.
5. Confirm command polarity.
6. Confirm speed limits.
7. Confirm CAN communication.
8. Confirm watchdog/timeout behavior.
9. Test at minimum practical speed.
10. Keep a physical emergency stop accessible during all initial movement tests.

Do not assume a command direction is correct simply because the ROS topic or CAN frame appears valid.

Physical direction must be verified.

⸻

Backup and Recovery

The original robot system should be preserved before major modifications.

A full eMMC image backup was created using dd over SSH.

The purpose of this backup is to preserve:

* Original operating system
* ROS Kinetic environment
* Original robot software
* Configuration
* Hardware-specific drivers
* Existing calibration
* Original executable files
* Recovery capability

The original system should be treated as a reference platform until the ROS 2 replacement has been fully validated.

⸻

Legacy System

The original robot was based on:

Ubuntu 16.04 LTS
ROS Kinetic
ARM64

The legacy system is retained primarily for:

* Protocol reverse engineering
* Hardware identification
* Configuration recovery
* Sensor-driver reference
* CAN message analysis
* Robot behavior comparison
* Emergency recovery

The legacy system should not be modified unnecessarily while migration is ongoing.

⸻

Migration Strategy

The migration is intentionally incremental.

Instead of attempting to recreate the entire original ROS 1 system at once, the system is being reconstructed around well-defined interfaces.

The general strategy is:

Legacy Hardware
      │
      ├── CAN protocol ───────► ROS 2 chassis driver
      │
      ├── Robot geometry ─────► ROS 2 URDF/Xacro
      │
      ├── LiDAR ───────────────► ROS 2 sensor driver
      │
      └── Navigation ──────────► ROS 2 Nav2 ecosystem

This allows individual subsystems to be validated independently.

⸻

Navigation and Autonomy Roadmap

Once chassis control, odometry, and LiDAR are fully operational, the next software layer is autonomous navigation.

The planned architecture is:

LiDAR
  │
  ▼
SLAM / Localization
  │
  ▼
Map
  │
  ▼
Nav2
  │
  ▼
cmd_vel
  │
  ▼
Chassis Driver
  │
  ▼
Motor Controllers
  │
  ▼
Robot

Expected components include:

* LiDAR driver
* sensor_msgs/LaserScan
* TF
* SLAM
* Localization
* Map server
* Nav2
* Controller
* Planner
* Costmaps
* Recovery behaviors
* Safety controller

⸻

Local Mapping

The original robot relied on cloud-based infrastructure for mapping.

That external mapping infrastructure is no longer available.

The ROS 2 architecture therefore moves mapping and navigation toward a local/onboard architecture.

The intended system is:

LiDAR
   ↓
SLAM
   ↓
Local Map
   ↓
Map Storage
   ↓
Localization
   ↓
Nav2

This removes dependence on the discontinued cloud service and gives the robot a self-contained navigation stack.

⸻

Future Hardware Architecture

The final ROS 2 computer should provide sufficient performance for simultaneous:

* ROS 2
* LiDAR processing
* SLAM
* Localization
* Nav2
* Visualization when required
* CAN communication
* Sensor processing
* Diagnostics
* Networking

The existing RK3399 platform remains useful for testing and reverse engineering, but a dedicated modern ROS 2 computer may be preferable for the final deployment depending on physical testing and computational requirements.

⸻

Current Known Technical Risks

Chassis Command Protocol

Receiving and decoding CAN data is substantially different from safely transmitting motor commands.

The transmit-side CAN protocol must be completely understood before autonomous motion is enabled.

⸻

Wheel Calibration

Wheel radius, wheel separation, encoder scaling, and wheel-speed units must be experimentally validated.

Incorrect values will directly affect odometry and navigation accuracy.

⸻

Direction and Sign Conventions

The following must be verified experimentally:

Left wheel positive direction
Right wheel positive direction
Forward chassis direction
Angular velocity sign
CAN wheel-speed sign
ROS wheel-speed sign

⸻

Odometry Accuracy

The current chassis odometry proves that the ROS 2 driver is receiving and processing physical data.

It does not by itself prove that the odometry is accurately calibrated.

A controlled physical test should be performed to compare:

Commanded distance
        vs.
Measured odometry distance

and:

Commanded rotation
        vs.
Measured odometry rotation

⸻

Recommended Validation Tests

Test 1 — Stationary Robot

Verify:

wheel speeds ≈ 0
linear velocity ≈ 0
angular velocity ≈ 0

⸻

Test 2 — Straight-Line Motion

Drive the robot forward a known distance.

Compare:

Physical distance
Odometry distance

⸻

Test 3 — Reverse Motion

Repeat the test in reverse.

Verify the odometry sign is correct.

⸻

Test 4 — In-Place Rotation

Rotate the robot through a known angle.

Compare:

Physical rotation
Odometry rotation

⸻

Test 5 — Circle

Drive the robot in a controlled circular trajectory.

Compare the expected and measured trajectory.

This is useful for detecting:

* Incorrect wheel separation
* Unequal wheel scaling
* Direction errors
* Encoder scaling errors

⸻

Useful ROS 2 Commands

List nodes:

ros2 node list

List topics:

ros2 topic list

Inspect a topic:

ros2 topic info /chassis/odom

Read odometry:

ros2 topic echo /chassis/odom

Read one odometry message:

ros2 topic echo /chassis/odom --once

Read wheel speeds:

ros2 topic echo /chassis/wheel_speeds

Inspect TF:

ros2 run tf2_tools view_frames

Check transforms:

ros2 run tf2_ros tf2_echo odom base_link

Inspect diagnostics:

ros2 topic echo /diagnostics

List available services:

ros2 service list

List parameters:

ros2 param list

⸻

Git Development Guidelines

Keep hardware-independent protocol logic separate from ROS-specific logic.

Prefer:

Protocol
   ↓
Transport
   ↓
Driver
   ↓
ROS 2

rather than placing all communication and ROS logic inside one node.

When modifying the CAN protocol:

* Document the CAN ID
* Document payload bytes
* Document byte order
* Document scaling
* Document units
* Document signed/unsigned interpretation
* Document known behavior
* Record the source of the information
* Add replay data where possible
* Avoid undocumented command transmission

When modifying the robot model:

* Keep frame naming consistent
* Validate Xacro
* Run check_urdf
* Verify TF
* Test in RViz

⸻

Engineering Principle

The primary objective of this repository is not simply to reproduce the original robot software.

The goal is to create a maintainable, testable, modular ROS 2 robotics platform that can be understood and modified by future engineers without depending on undocumented legacy infrastructure.

The architecture therefore prioritizes:

* Modularity
* Reproducibility
* Hardware abstraction
* Protocol isolation
* Testability
* Safety
* Documentation
* Local operation
* ROS 2 compatibility
* Long-term maintainability

⸻

Development Roadmap

Phase 1 — ROS 2 Foundation

* [x]	Ubuntu 24.04
* [x]	ROS 2 Jazzy
* [x]	ROS 2 workspace
* [x]	Package migration
* [x]	Build system migration

Phase 2 — Robot Description

* [x]	URDF migration
* [x]	Xacro migration
* [x]	Mesh validation
* [x]	TF structure
* [x]	RViz visualization

Phase 3 — Chassis Integration

* [x]	CAN hardware identification
* [x]	CAN traffic capture
* [x]	CAN protocol analysis
* [x]	Protocol package
* [x]	Transport package
* [x]	ROS 2 chassis driver
* [x]	Wheel feedback
* [x]	Odometry publishing
* [x]	Physical CAN validation
* [ ]	Full command/control validation
* [ ]	Complete safety/watchdog implementation

Phase 4 — Sensors

* [ ]	LiDAR ROS 2 integration
* [ ]	LaserScan validation
* [ ]	Sensor TF validation
* [ ]	Sensor timing validation

Phase 5 — Localization and Mapping

* [ ]	SLAM
* [ ]	Map generation
* [ ]	Map storage
* [ ]	Localization
* [ ]	Map-to-odom TF

Phase 6 — Autonomous Navigation

* [ ]	Nav2
* [ ]	Local costmap
* [ ]	Global costmap
* [ ]	Planner
* [ ]	Controller
* [ ]	Recovery behaviors
* [ ]	Safety limits
* [ ]	Autonomous waypoint navigation

Phase 7 — Physical Deployment

* [ ]	Final onboard computer
* [ ]	ROS 2 deployment
* [ ]	Boot configuration
* [ ]	Automatic startup
* [ ]	Network configuration
* [ ]	Physical driving
* [ ]	Odometry calibration
* [ ]	SLAM testing
* [ ]	Autonomous navigation
* [ ]	Patrol testing

⸻

Final Objective

The completed Terminus ROS 2 platform should operate as a self-contained autonomous mobile robot:

                  ┌─────────────────────┐
                  │      TERMINUS       │
                  │   Autonomous Robot  │
                  └──────────┬──────────┘
                             │
             ┌───────────────┼───────────────┐
             │               │               │
             ▼               ▼               ▼
           LiDAR          Chassis          Sensors
             │               │               │
             ▼               ▼               ▼
            SLAM         CAN Driver        ROS 2
             │               │               │
             └───────────────┼───────────────┘
                             ▼
                        Robot State
                             │
                             ▼
                        Localization
                             │
                             ▼
                           Nav2
                             │
                             ▼
                         cmd_vel
                             │
                             ▼
                       Motor Control
                             │
                             ▼
                     Differential Drive

The end state is a modern ROS 2-based robotic platform capable of operating independently of the original discontinued ROS 1 and cloud infrastructure while retaining compatibility with the existing Terminus hardware
