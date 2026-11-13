
#include <esp_now.h>
#include <WiFi.h>

// Replace with the actual receiver ESP32 Wi-Fi MAC address
uint8_t receiverMAC[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Joystick and button pins from Appendix A
#define VRX_PIN 34
#define VRY_PIN 35
#define SW_PIN 32
#define BTN_PIN 33
#define LED_PIN 25

// Data sent to the receiver
typedef struct ControllerData {
  int16_t xAxis;
  int16_t yAxis;
  bool joystickPressed;
  bool buttonPressed;
} ControllerData;

ControllerData dataPacket;

// ESP-NOW send-status callback (Arduino-ESP32 2.x)
void onDataSent(const uint8_t *mac_addr,
                esp_now_send_status_t status) {
  digitalWrite(
    LED_PIN,
    status == ESP_NOW_SEND_SUCCESS ? HIGH : LOW
  );
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add receiver");
    return;
  }

  pinMode(SW_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  int xValue = analogRead(VRX_PIN);
  int yValue = analogRead(VRY_PIN);

  dataPacket.xAxis = map(xValue, 0, 4095, -100, 100);
  dataPacket.yAxis = map(yValue, 0, 4095, -100, 100);

  dataPacket.joystickPressed = (digitalRead(SW_PIN) == LOW);
  dataPacket.buttonPressed = (digitalRead(BTN_PIN) == LOW);

  esp_now_send(
    receiverMAC,
    (uint8_t *)&dataPacket,
    sizeof(dataPacket)
  );

  delay(100);
}
