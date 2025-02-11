#include <esp_now.h>
#include <WiFi.h>

#define buttonPin1 18
#define buttonPin2 26
#define buttonPin3 27

#define switchPin 23

#define accel 1200
#define speed 2000

uint8_t broadcastAddress1[] = { 0xcc, 0xdb, 0xa7, 0x3e, 0xe7, 0xec };
uint8_t broadcastAddress2[] = { 0x08, 0xa6, 0xf7, 0xbc, 0x7f, 0x64 };

const int button1Pos = 0;
const int button2Pos = 540 * 16;
const int button3Pos = (1080) * 16;

int switchState = 0;
int positions[2] = { 0, 0 };

portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;

typedef struct struct_message {
  volatile int posToSend;
  int maxAccel;
  int maxSpeed;
} struct_message;

struct_message buttonMessage;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void IRAM_ATTR handleButton(int buttonPos) {
  portENTER_CRITICAL(&synch);
  buttonMessage.posToSend = buttonPos;
  buttonMessage.maxAccel = accel;
  buttonMessage.maxSpeed = speed;

  if (switchState == HIGH) {
    positions[0] = buttonPos;
  } else {
    positions[1] = buttonPos;
  }

  portEXIT_CRITICAL(&synch);
}

void IRAM_ATTR handleSwitch() {
  switchState = digitalRead(switchPin);
  int buttonPos = (switchState == HIGH) ? positions[0] : positions[1];

  Serial.println(buttonPos);
  buttonMessage.posToSend = buttonPos;
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo1 = {};
  memcpy(peerInfo1.peer_addr, broadcastAddress1, 6);
  peerInfo1.channel = 0;
  peerInfo1.encrypt = false;
  if (esp_now_add_peer(&peerInfo1) != ESP_OK) {
    Serial.println("Failed to add peer 1");
    return;
  }

  esp_now_peer_info_t peerInfo2 = {};
  memcpy(peerInfo2.peer_addr, broadcastAddress2, 6);
  peerInfo2.channel = 0;
  peerInfo2.encrypt = false;
  if (esp_now_add_peer(&peerInfo2) != ESP_OK) {
    Serial.println("Failed to add peer 2");
    return;
  }

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP);

  attachInterrupt(
    digitalPinToInterrupt(buttonPin1), [] {
      handleButton(button1Pos);
    },
    FALLING);
  attachInterrupt(
    digitalPinToInterrupt(buttonPin2), [] {
      handleButton(button2Pos);
    },
    FALLING);
  attachInterrupt(
    digitalPinToInterrupt(buttonPin3), [] {
      handleButton(button3Pos);
    },
    FALLING);
  attachInterrupt(digitalPinToInterrupt(switchPin), handleSwitch, CHANGE);
}

void loop() {
  uint8_t *broadcastAddress = (switchState == HIGH) ? broadcastAddress1 : broadcastAddress2;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&buttonMessage, sizeof(buttonMessage));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  Serial.println(buttonMessage.posToSend);
  Serial.println(switchState);

  if (broadcastAddress == broadcastAddress1) {
    Serial.println("Broadcasting to system 1");
  } else {
    Serial.println("Broadcasting to system 2");
  }

  delay(100);
}
