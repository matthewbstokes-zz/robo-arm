# Description
5DOF robotic manipulator arm remotely controlled. Forward and inverse kinematics. But really Just an excuse to play with ROS so let's keep this as low cost as possible.

# Mechanical
  - OWI Robotic Arm Edge outfitted with potentiometers at each joint

# Hardware
  - Multiplexer - analog read the potentiometers at each joint
  - Motor Controller - PWM motors at each joint
  
# Software
  - Adafruit WiFi library
  - ROS Node
    - Publish msr_joint angles
    - Subscribe dsr_joint angles
  
# TODO
  - PID controller
  - ROS forward kinematics (DH) parameters
  - ROS inverse kinemtaics
