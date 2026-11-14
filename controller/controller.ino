
#include <esp_now.h>
#include <WiFi.h>
#include <esp_idf_version.h>
#include <string.h>

// Replace with the actual Wi-Fi MAC address of the receiver ESP32.
// The address below is a placeholder and will not establish communication.
uint8_t receiverMAC[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Controller pins from laboratory Appendix A
#define VRX_PIN 34
#define VRY_PIN 35
#define SW_PIN 32
#define BTN_PIN 33
#define LED_PIN 25

// Must match the receiver's ControllerData structure exactly
typedef struct ControllerData {
  int16_t xAxis;
  int16_t yAxis;
  bool joystickPressed;
  bool buttonPressed;
} ControllerData;

ControllerData dataPacket;

// ESP-NOW transmission status
volatile bool sendStatusAvailable = false;
volatile bool lastSendSuccessful = false;

// Compatible send callback for different ESP-IDF versions
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 5, 0)

void onDataSent(const esp_now_send_info_t *info,
                esp_now_send_status_t status) {

  lastSendSuccessful = (status == ESP_NOW_SEND_SUCCESS);
  sendStatusAvailable = true;
}

#else

void onDataSent(const uint8_t *mac_addr,
                esp_now_send_status_t status) {

  lastSendSuccessful = (status == ESP_NOW_SEND_SUCCESS);
  sendStatusAvailable = true;
}

#endif

void setup() {
  Serial.begin(115200);

  // Configure joystick and button inputs
  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT_PULLUP);

  // Configure transmission-status LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Configure ESP32 for ESP-NOW
  WiFi.mode(WIFI_STA);

  Serial.println("ESP32 Wireless Joystick Controller");
  Serial.print("Controller MAC: ");
  Serial.println(WiFi.macAddress());

  // Prevent transmission to the placeholder MAC address
  bool macNotConfigured = true;

  for (int i = 0; i < 6; i++) {
    if (receiverMAC[i] != 0x00) {
      macNotConfigured = false;
      break;
    }
  }

  if (macNotConfigured) {
    Serial.println("ERROR: Configure the receiver MAC address.");
    return;
  }

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  // Register transmission callback
  if (esp_now_register_send_cb(onDataSent) != ESP_OK) {
    Serial.println("Failed to register send callback");
    return;
  }

  // Configure receiver as an ESP-NOW peer
  esp_now_peer_info_t peerInfo = {};

  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add receiver");
    return;
  }

  Serial.println("ESP-NOW controller initialized");
}

void loop() {
  // Do not transmit until the receiver MAC is configured
  bool macNotConfigured = true;

  for (int i = 0; i < 6; i++) {
    if (receiverMAC[i] != 0x00) {
      macNotConfigured = false;
      break;
    }
  }

  if (macNotConfigured) {
    delay(1000);
    return;
  }

  // Read analogue joystick inputs
  int xValue = analogRead(VRX_PIN);
  int yValue = analogRead(VRY_PIN);

  // Map ADC readings to control values
  dataPacket.xAxis = map(xValue, 0, 4095, -100, 100);
  dataPacket.yAxis = map(yValue, 0, 4095, -100, 100);

  // Read button states (active LOW)
  dataPacket.joystickPressed = (digitalRead(SW_PIN) == LOW);
  dataPacket.buttonPressed = (digitalRead(BTN_PIN) == LOW);

  // Transmit controller data
  esp_err_t result = esp_now_send(
    receiverMAC,
    (const uint8_t *)&dataPacket,
    sizeof(dataPacket)
  );

  if (result != ESP_OK) {
    Serial.println("ESP-NOW send request failed");
  }

  // Display transmission status when available
  if (sendStatusAvailable) {
    bool success = lastSendSuccessful;
    sendStatusAvailable = false;

    digitalWrite(LED_PIN, success ? HIGH : LOW);

    Serial.println(
      success ? "Delivery Success" : "Delivery Failed"
    );
  }

  // Display joystick and button values
  Serial.print("X: ");
  Serial.print(dataPacket.xAxis);

  Serial.print(" | Y: ");
  Serial.print(dataPacket.yAxis);

  Serial.print(" | Joystick Button: ");
  Serial.print(dataPacket.joystickPressed);

  Serial.print(" | Emergency Stop: ");
  Serial.println(dataPacket.buttonPressed);

  delay(100);
}
