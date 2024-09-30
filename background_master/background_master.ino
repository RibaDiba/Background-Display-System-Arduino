#include <esp_now.h>
#include <WiFi.h>

#define buttonPin1 18
#define buttonPin2 26
#define buttonPin3 27 
#define buttonPin4 19

#define slave_mac_addr

int lastButtonState1 = HIGH;
int lastButtonState2 = HIGH;
int lastButtonState3 = HIGH;
int lastButtonState4 = HIGH;

typedef struct struct_message {
  int buttonState1;
  int buttonState2;
  int buttonState3;
  int buttonState4;
} struct_message;

struct_message buttonMessage;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, slave_mac_addr, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);
}

void loop() {
  int buttonState1 = digitalRead(buttonPin1);
  int buttonState2 = digitalRead(buttonPin2);
  int buttonState3 = digitalRead(buttonPin3);
  int buttonState4 = digitalRead(buttonPin4);

  if (buttonState1 != lastButtonState1 || buttonState2 != lastButtonState2 || 
      buttonState3 != lastButtonState3 || buttonState4 != lastButtonState4) {

    buttonMessage.buttonState1 = buttonState1;
    buttonMessage.buttonState2 = buttonState2;
    buttonMessage.buttonState3 = buttonState3;
    buttonMessage.buttonState4 = buttonState4;

    esp_err_t result = esp_now_send(peerInfo.peer_addr, (uint8_t *)&buttonMessage, sizeof(buttonMessage));

    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
  }

  lastButtonState1 = buttonState1;
  lastButtonState2 = buttonState2;
  lastButtonState3 = buttonState3;
  lastButtonState4 = buttonState4;

  delay(100);
}
