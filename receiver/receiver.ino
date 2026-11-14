
#include <ESP32Servo.h>
#include <esp_now.h>
#include <WiFi.h>
#include <string.h>

// Data received from the wireless joystick controller.
// This structure must match the transmitter.
typedef struct ControllerData {
  int16_t xAxis;
  int16_t yAxis;
  bool joystickPressed;
  bool buttonPressed;
} ControllerData;

ControllerData receivedData;

// Motor pins from laboratory Appendix B
#define enA 33
#define enB 25
#define INa 26
#define INb 27
#define INc 14
#define INd 12

// Servo settings
#define SERVO_PIN 13
#define SERVO_CENTER 49

// Steering correction retained from the previous sketch.
// This value has not been verified against the original code.
#define MAX_SERVO_CORRECTION 80

// Maximum motor speed shown in the laboratory photograph
#define MAX_SPEED 180

Servo steeringServo;

// Set DC motor speed and direction
void setMotors(int speed) {
  speed = -speed;

  if (speed > 0) {
    digitalWrite(INa, HIGH);
    digitalWrite(INb, LOW);
    digitalWrite(INc, HIGH);
    digitalWrite(INd, LOW);
  }
  else if (speed < 0) {
    digitalWrite(INa, LOW);
    digitalWrite(INb, HIGH);
    digitalWrite(INc, LOW);
    digitalWrite(INd, HIGH);
  }
  else {
    digitalWrite(INa, LOW);
    digitalWrite(INb, LOW);
    digitalWrite(INc, LOW);
    digitalWrite(INd, LOW);
  }

  int pwmValue = constrain(abs(speed), 0, 255);

  analogWrite(enA, pwmValue);
  analogWrite(enB, pwmValue);
}

// Software emergency-stop command
void emergencyStop() {
  setMotors(0);
}

// ESP-NOW receive callback for Arduino-ESP32 3.x
void onDataRecv(const esp_now_recv_info_t *info,
                const uint8_t *incomingData,
                int len) {

  if (len != sizeof(receivedData)) {
    return;
  }

  memcpy(&receivedData, incomingData, sizeof(receivedData));

  // Separate emergency-stop button
  if (receivedData.buttonPressed) {
    emergencyStop();
    return;
  }

  // Calculate steering angle
  int angle = constrain(
    SERVO_CENTER +
    map(
      receivedData.xAxis,
      -100, 100,
      -MAX_SERVO_CORRECTION,
      MAX_SERVO_CORRECTION
    ),
    0, 180
  );

  steeringServo.write(angle);

  // Calculate motor speed
  int motorSpeed = map(
    receivedData.yAxis,
    -100, 100,
    -MAX_SPEED, MAX_SPEED
  );

  setMotors(motorSpeed);
}

void setup() {
  Serial.begin(115200);

  // Configure motor pins
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(INa, OUTPUT);
  pinMode(INb, OUTPUT);
  pinMode(INc, OUTPUT);
  pinMode(INd, OUTPUT);

  // Start with motors stopped
  setMotors(0);

  // Configure steering servo
  ESP32PWM::allocateTimer(0);
  steeringServo.setPeriodHertz(50);
  steeringServo.attach(SERVO_PIN, 1000, 2000);
  steeringServo.write(SERVO_CENTER);

  // Configure wireless communication
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  if (esp_now_register_recv_cb(onDataRecv) != ESP_OK) {
    Serial.println("Failed to register receive callback");
    return;
  }

  Serial.println("ESP-NOW receiver initialized");
}

void loop() {
  // Motor and steering commands are processed
  // when a wireless data packet is received.
}
