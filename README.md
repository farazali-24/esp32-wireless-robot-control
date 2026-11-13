
# ESP32 Wireless Joystick Robot Control

Design and implementation of a wireless joystick-controlled robot using two ESP32 microcontrollers and ESP-NOW communication. The project integrates analogue joystick inputs, wireless data transmission, DC motor speed control, servo steering and an emergency-stop function.

Developed as part of an Electrical and Electronic Engineering laboratory project, building upon the previous EEEbot robot construction and motor-control work.

## Demo / Images

Original circuit diagrams, flowcharts and laboratory testing photographs are available in the [Project Documentation](docs/README.md).

## Hardware

- 2 × ESP32 development boards
- Analogue joystick module with integrated push-button
- EEEbot robotic chassis
- DC motors
- Motor-driver circuitry
- Servo motor
- Power supply and connecting wires
- Breadboard and jumper wires
- USB cables for programming and testing

## Pin Configuration

### Wireless Controller ESP32

| Component | ESP32 GPIO | Function |
|---|---|---|
| Joystick X-axis | GPIO34 | Analogue horizontal input |
| Joystick Y-axis | GPIO35 | Analogue vertical input |
| Joystick push-button | GPIO32 | Emergency-stop input |
| Joystick VCC | 3.3V | Power supply |
| Joystick GND | GND | Common ground |

The receiver ESP32 connects to the robot's motor-control and servo circuitry. Its connections are documented in the receiver code and original laboratory diagrams.

## How it works

The system uses two ESP32 boards communicating wirelessly through ESP-NOW.

One ESP32 acts as the controller, reading the joystick position and push-button state.

The second ESP32 acts as the receiver, processing incoming control data and controlling the robot's movement.

### 1. Joystick Input

The analogue joystick produces different voltage levels depending on its position.

The controller ESP32 reads these signals using its analogue-to-digital converter (ADC).

The joystick values are mapped from the ESP32's 12-bit ADC range (0–4095) to a control range of approximately -100 to +100.

These values represent the requested movement and steering.

### 2. ESP-NOW Wireless Communication

ESP-NOW is a wireless communication protocol supported by ESP32 microcontrollers.

It allows compatible devices to exchange data directly without connecting to a conventional Wi-Fi network.

The controller packages the joystick readings and button state into a data structure and transmits them to the receiver ESP32.

### 3. Motor Speed Control

The receiver processes the received joystick values to determine the robot's movement.

Pulse-width modulation (PWM) is used for DC motor speed control.

By changing the control signals sent to the motor-driver circuitry, the system can adjust the robot's movement.

### 4. Servo Steering

The receiver also controls a servo motor to steer the robot.

The steering command is determined from the joystick input received wirelessly.

### 5. Emergency Stop

The joystick's integrated push-button provides an emergency-stop input.

When activated, the control system commands the robot to stop moving.

This is a software-based stop function rather than an independently rated hardware safety system.

## System Architecture

The wireless control system consists of two main subsystems.

### Transmitter — Controller ESP32

1. Read the joystick X-axis.
2. Read the joystick Y-axis.
3. Read the push-button state.
4. Map the analogue readings to control values.
5. Package the control data.
6. Transmit the data using ESP-NOW.

### Receiver — Robot ESP32

1. Receive the wireless data.
2. Extract the joystick and button values.
3. Process the requested movement.
4. Update the DC motor control signals.
5. Update the steering servo.
6. Stop the motors when the emergency-stop command is activated.

## Software

### Development Environment

- Arduino IDE
- Arduino C/C++
- ESP32 board support package
- ESP-NOW communication library
- ESP32 motor-control and servo functionality

### Project Files

| File | Description |
|---|---|
| `controller/controller.ino` | Reads joystick inputs and transmits control data using ESP-NOW |
| `receiver/receiver.ino` | Receives wireless commands and controls the robot's motors and steering |
| `docs/README.md` | Circuit diagrams, flowcharts, photographs and testing documentation |

The transmitter and receiver are programmed separately because each ESP32 performs a different role.

## Build & Run

### 1. Assemble the Controller

1. Connect the joystick module to the controller ESP32.
2. Connect the X-axis output to GPIO34.
3. Connect the Y-axis output to GPIO35.
4. Connect the integrated push-button to GPIO32.
5. Connect the joystick power and ground pins appropriately.
6. Check the connections before applying power.

### 2. Prepare the Receiver

1. Connect the receiver ESP32 to the robot's motor-control circuitry.
2. Connect the steering servo according to the receiver circuit.
3. Verify the motor-driver connections and power supply.
4. Check the wiring before powering the system.

### 3. Configure Wireless Communication

1. Identify the ESP-NOW communication addresses required by the sketches.
2. Configure the transmitter to communicate with the receiver.
3. Ensure the communication data structures match.
4. Upload the appropriate program to each ESP32.

### 4. Test the System

1. Power both ESP32 microcontrollers.
2. Open the Arduino IDE Serial Monitor where required.
3. Check the joystick readings.
4. Verify wireless transmission and reception.
5. Test movement commands.
6. Test steering.
7. Test the emergency-stop button.

**Safety:** Test the robot with its drive wheels raised initially. A software emergency stop should not be relied upon as the sole protection against uncontrolled motion or communication failure.

## Results / Notes

### Successful Implementation

The laboratory project demonstrated:

- Wireless communication between two ESP32 microcontrollers.
- Analogue joystick readings and input processing.
- Transmission of joystick and button information through ESP-NOW.
- Wireless robot movement control.
- Servo steering.
- Emergency-stop functionality.

The wireless joystick control system was successfully demonstrated during laboratory testing.

### Observations

Some movement jitter and power fluctuations were observed at higher motor speeds.

These observations highlighted the importance of stable power delivery, reliable control signals and testing under different operating conditions.

### Limitations

- Motor movement was not always completely smooth at higher speeds.
- Power fluctuations affected the consistency of operation.
- The system depended on successful wireless communication.
- The software emergency-stop function did not provide an independent hardware safety mechanism.

### Future Improvements

- Improve power supply stability.
- Refine motor speed and steering response.
- Implement a communication-loss timeout that automatically stops the motors.
- Improve joystick calibration and dead-zone handling.
- Investigate closed-loop motor speed control.
- Add reliable sensor feedback once the sensing hardware has been validated.

## What I Learned

- How ESP-NOW enables direct communication between ESP32 microcontrollers.
- How to use analogue joystick readings as control inputs.
- How ADC values can be mapped into useful movement commands.
- How to transmit structured data between embedded devices.
- How to integrate wireless communication with motor and servo control.
- How to implement an emergency-stop command.
- How to test and troubleshoot a multi-microcontroller system.
- Why power stability and communication reliability are important in embedded robotics.

## Related Projects

This project builds on the following earlier work:

- [EEEbot Robot Construction](https://github.com/farazali-24/eeebot-robot-construction)
- [H-Bridge DC Motor Control](https://github.com/farazali-24/h-bridge-dc-motor-control)

## Credits

Developed as part of an Electrical and Electronic Engineering laboratory project.

**Hardware:**
- ESP32 microcontrollers
- Analogue joystick module
- EEEbot robotic platform
- DC motors and motor-driver circuitry
- Servo motor

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
