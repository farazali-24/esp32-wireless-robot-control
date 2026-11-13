
# ESP32 Wireless Joystick Robot Control

Design and implementation of a wireless joystick-controlled robot using two ESP32 microcontrollers and ESP-NOW communication. The project integrates analogue joystick inputs, wireless data transmission, DC motor speed control, servo steering and a software emergency-stop function.

Developed as part of an Electrical and Electronic Engineering laboratory project, building upon previous EEEbot construction and motor-control work.

## Demo / Images

Original circuit diagrams, flowcharts and laboratory testing photographs will be added to the [Project Documentation](docs/README.md).

## Hardware

- 2 × ESP32 development boards
- Analogue joystick module with integrated push-button
- Separate push-button for emergency stop
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
| Joystick X-axis (VRX) | GPIO34 | Analogue steering input |
| Joystick Y-axis (VRY) | GPIO35 | Analogue speed input |
| Joystick push-button (SW) | GPIO32 | Joystick switch input |
| Separate push-button (BTN) | GPIO33 | Emergency-stop command |
| LED | GPIO25 | ESP-NOW transmission status |
| Joystick VCC | 3.3V | Power supply |
| Joystick GND | GND | Common ground |

The joystick produces analogue values between 0 and 4095, which are mapped into a control range of approximately -100 to +100.

The separate push-button connected to GPIO33 is used to send the emergency-stop command. The joystick's integrated switch is read separately.

### Robot Receiver ESP32

| Component | ESP32 GPIO | Function |
|---|---|---|
| Motor A enable (enA) | GPIO33 | PWM motor-speed control |
| Motor B enable (enB) | GPIO25 | PWM motor-speed control |
| Motor A direction (INa) | GPIO26 | Motor direction |
| Motor A direction (INb) | GPIO27 | Motor direction |
| Motor B direction (INc) | GPIO14 | Motor direction |
| Motor B direction (INd) | GPIO12 | Motor direction |
| Steering servo | GPIO13 | Servo control signal |

These receiver pin assignments are taken from the wireless control implementation in the laboratory report.

The controller and receiver use separate ESP32 boards, so the same GPIO number can be assigned to different components on each board.

**Hardware note:** GPIO12 is a boot-strapping pin on the original ESP32. External motor-driver circuitry must not interfere with the required startup state.

## How it works

The wireless robot-control system uses two ESP32 microcontrollers communicating through ESP-NOW.

The controller ESP32 reads the joystick position and button states, processes the inputs and transmits the information wirelessly.

The receiver ESP32 interprets the received commands and adjusts the DC motors and steering servo.

### 1. Joystick Input

The analogue joystick produces voltage signals corresponding to its horizontal and vertical positions.

The controller uses the ESP32's analogue-to-digital converter (ADC) to read these signals.

The 12-bit ADC readings are mapped from 0–4095 to approximately -100 to +100.

- X-axis: Steering input.
- Y-axis: Motor speed and direction input.
- Integrated joystick switch: Additional digital input.
- Separate push-button: Emergency-stop command.

### 2. ESP-NOW Wireless Communication

ESP-NOW is a wireless communication protocol supported by ESP32 microcontrollers.

It enables compatible devices to exchange data directly without requiring a conventional Wi-Fi network.

The controller ESP32:

1. Reads joystick and button inputs.
2. Maps the analogue values into control commands.
3. Packages the commands into a data structure.
4. Transmits the data using ESP-NOW.

The receiver ESP32:

1. Receives the wireless data.
2. Extracts the joystick and button states.
3. Processes movement and steering commands.
4. Updates the motors and steering servo.

### 3. DC Motor Speed Control

The receiver processes the Y-axis joystick input to determine the requested motor speed and direction.

Pulse-width modulation (PWM) is used to regulate motor speed.

Direction-control signals determine whether the motors rotate forwards or backwards.

Changing the joystick position changes the movement command sent to the robot.

### 4. Servo Steering

The receiver processes the X-axis joystick input to control the steering servo.

The joystick's horizontal position is mapped to a servo angle.

Moving the joystick left or right changes the requested steering direction.

### 5. Emergency Stop

A separate push-button connected to GPIO33 on the controller sends the emergency-stop command.

When the receiver detects that this button is pressed, it commands the DC motors to stop.

The stop operates through software and wireless communication. It is not an independently rated hardware emergency-stop system.

## System Architecture

The system consists of two connected embedded subsystems.

### Transmitter — Controller ESP32

1. Read the joystick X-axis from GPIO34.
2. Read the joystick Y-axis from GPIO35.
3. Read the joystick switch from GPIO32.
4. Read the separate push-button from GPIO33.
5. Map the analogue readings to control values.
6. Package the readings and button states.
7. Transmit the data using ESP-NOW.

### Receiver — Robot ESP32

1. Receive the data through ESP-NOW.
2. Extract the joystick and button values.
3. Check whether the emergency-stop button is pressed.
4. Calculate the required motor speed and direction.
5. Update the motor-driver control signals.
6. Calculate the steering angle.
7. Update the steering servo.

## Software

### Development Environment

- Arduino IDE
- Arduino C/C++
- ESP32 board support package
- ESP-NOW communication library
- Wi-Fi library for ESP32 wireless configuration
- ESP32Servo library for steering control

### Project Files

| File | Description |
|---|---|
| `controller/controller.ino` | Reads joystick inputs and transmits wireless control data |
| `receiver/receiver.ino` | Receives ESP-NOW commands and controls the motors and steering |
| `docs/README.md` | Circuit diagrams, flowcharts, photographs and testing documentation |

The transmitter and receiver use separate programs because each ESP32 performs a different role.

**Compatibility note:** The Arduino sketches use ESP-NOW callback signatures associated with Arduino-ESP32 2.x. Changes may be required for Arduino-ESP32 3.x. The repository code has not been independently retested following documentation.

## Build & Run

### 1. Assemble the Wireless Controller

1. Connect joystick VRX to GPIO34.
2. Connect joystick VRY to GPIO35.
3. Connect joystick SW to GPIO32.
4. Connect the separate push-button to GPIO33.
5. Connect the joystick to 3.3V and GND.
6. Connect the status LED according to the controller circuit.
7. Verify the wiring before applying power.

### 2. Prepare the Robot Receiver

1. Connect the receiver ESP32 to the EEEbot motor-driver circuitry.
2. Connect the two motor-enable signals to GPIO33 and GPIO25.
3. Connect the motor-direction signals to GPIO26, GPIO27, GPIO14 and GPIO12.
4. Connect the steering servo signal to GPIO13.
5. Verify the power supply and common ground connections.
6. Check the motor-driver wiring before powering the robot.

### 3. Configure ESP-NOW

1. Identify the receiver ESP32's Wi-Fi MAC address.
2. Configure the controller sketch to transmit to the receiver.
3. Confirm that the transmitted and received data structures match.
4. Upload the controller program to the transmitter ESP32.
5. Upload the receiver program to the robot ESP32.

### 4. Test the System

1. Power both ESP32 boards.
2. Open the Arduino IDE Serial Monitor where required.
3. Check the analogue joystick readings.
4. Verify ESP-NOW communication.
5. Test forward and reverse motor commands.
6. Test steering control.
7. Press the separate emergency-stop button.
8. Confirm that the receiver commands the motors to stop.

**Safety:** Initial motor tests should be conducted with the drive wheels raised. The software stop depends on wireless communication and should not be treated as an independent safety mechanism.

## Results / Notes

### Successful Implementation

The laboratory project demonstrated:

- Wireless communication between two ESP32 microcontrollers.
- Analogue joystick input processing.
- Transmission of joystick and button data through ESP-NOW.
- Wireless DC motor control.
- Servo-based steering.
- A software emergency-stop command.

The wireless joystick-control system was successfully demonstrated during laboratory testing.

### Observations

Some motor movement jitter and power fluctuations were observed at higher motor speeds.

These observations highlighted the importance of reliable control signals, stable power delivery and testing under different operating conditions.

### Limitations

- Motor movement was not always smooth at higher speeds.
- Power fluctuations affected the consistency of operation.
- The system depended on successful wireless communication.
- The receiver code did not implement an automatic communication-loss timeout.
- The emergency-stop command depended on successful wireless transmission.
- Additional sensor-feedback functionality explored in the laboratory was not successfully completed and is not included as an operational feature of this repository.

### Future Improvements

- Improve power supply stability.
- Implement a communication-loss timeout to stop the motors automatically.
- Add joystick dead-zone handling and calibration.
- Refine PWM motor-speed control.
- Improve steering response and calibration.
- Investigate closed-loop motor control using encoder feedback.
- Add reliable sensor feedback after further hardware testing.

## What I Learned

- How ESP-NOW enables direct wireless communication between ESP32 microcontrollers.
- How to read analogue joystick inputs using an ADC.
- How to map sensor readings into usable control values.
- How to transmit structured data between embedded devices.
- How to integrate wireless communication with motor and servo control.
- How to implement a software emergency-stop command.
- How to test a system containing multiple microcontrollers.
- Why stable power delivery and reliable communication matter in embedded robotics.

## Related Projects

This project builds upon previous electronics and robotics work:

- [EEEbot Robot Construction](https://github.com/farazali-24/eeebot-robot-construction)
- [H-Bridge DC Motor Control](https://github.com/farazali-24/h-bridge-dc-motor-control)

## Credits

Developed as part of an Electrical and Electronic Engineering laboratory project.

**Hardware:**
- ESP32 microcontrollers
- Analogue joystick module
- EEEbot robotic platform
- DC motors and motor-driver circuitry
- Steering servo
- Laboratory power supplies and test equipment

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
