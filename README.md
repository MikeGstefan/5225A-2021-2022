# 5225A-2021-2022

Code used by VRC team 5225A for the 2021-2022 season: Tipping Point

Useful files in the repository:
* `non-pros-projects/data-spliter`: Python script and build to convert the logging and telemtry data into an easy to read format
* `/main/src/main.cpp`: Main file for pros; contains the main driver control loop and autonomous period handler
* `/main/src/Tracking.{hpp, cpp}`: Core odometery and autonmous motion algorithms
* `/main/src/logging.{hpp, cpp}`: SD card data logging
* `/main/src/Subsystems`: Starndarized statemachines for all subsystems of the robot
* `/main/src/Libraries/gui.{hpp, cpp}`: V5 screen GUI library using the PROS screen API
* `/main/src/auton.{hpp, cpp}`: Autonomous and programming skills routines
* `/main/src/auton_util.{hpp, cpp}`: Utilities used in during the autonomous routines
* `/main/src/config.{hpp, cpp}`: Motor and sensor configuration
* `/main/src/controller.{hpp, cpp}`: Controller manger; controller printing queue and the global button handler
* `/main/src/distance.{hpp, cpp}`: Robot positioning system using up to 4 infrared distance senesors
* `/main/src/drive.{hpp, cpp}`: Custom driver curve profiles and button action handlers
* `/main/src/geometry.{hpp, cpp}`: Geometric math utilities and macros to aid in odometry and motion algorithms
* `/main/src/pid.{hpp, cpp}`: Standardized PID algorithm handler library
* `/main/src/task.{hpp, cpp}`: Multitasking manger
* `/main/src/timer.{hpp, cpp}`: Timer library
* `/main/src/util.{hpp, cpp}`: Miscellaneous math and utility functions and macros
