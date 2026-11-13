
#include <ESP32Servo.h>
#include <esp_now.h>
#include <WiFi.h>

// Data received from the joystick controller
typedef struct ControllerData {
  int16_t xAxis;
  int16_t yAxis;
  bool joystickPressed;
  bool buttonPressed;
} ControllerData;

ControllerData receivedData;

// Motor pins from Appendix B
#define enA 33
#define enB 25
#define INa 26
#define INb 27
#define INc 14
#define INd 12

// Servo settings
#define SERVO_PIN 13
#define SERVO_CENTER 91
#define MAX_SERVO_CORRECTION 80

#define MAX_SPEED 200

Servo steeringServo;

// Set DC motor speed and direction
void setMotors(int speed) {
  speed = -speed;  // Invert direction if required

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

// Software emergency stop
void emergencyStop() {
  setMotors(0);
}

// ESP-NOW receive callback (Arduino-ESP32 2.x)
void onDataRecv(const uint8_t *mac,
                const uint8_t *incomingData,
                int len) {
  if (len != sizeof(receivedData)) {
    return;
  }

  memcpy(&receivedData, incomingData, sizeof(receivedData));

  if (receivedData.buttonPressed) {
    emergencyStop();
    return;
  }

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

  int motorSpeed = map(
    receivedData.yAxis,
    -100, 100,
    -MAX_SPEED, MAX_SPEED
  );

  setMotors(motorSpeed);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);

  // Motor pin configuration
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(INa, OUTPUT);
  pinMode(INb, OUTPUT);
  pinMode(INc, OUTPUT);
  pinMode(INd, OUTPUT);

  analogWrite(enA, 0);
  analogWrite(enB, 0);

  // Servo configuration
  ESP32PWM::allocateTimer(0);
  steeringServo.setPeriodHertz(50);
  steeringServo.attach(SERVO_PIN, 1000, 2000);
  steeringServo.write(SERVO_CENTER);
}

void loop() {
}
