#include <esp_now.h>
#include <WiFi.h>

#define buttonPin1 18
#define buttonPin2 26
#define buttonPin3 27
#define buttonPin4 19

#define accel 100 
#define speed 400

uint8_t broadcastAddress[] = {0xcc, 0xdb, 0xa7, 0x3e, 0xe7, 0xec}; 

// CHANGE THIS (PLACEHOLDER)
const int button1Pos = 0; 
const int button2Pos = 400;
const int button3Pos = 800;
const int button4Pos = 1200;

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
  portEXIT_CRITICAL(&synch);
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

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6); //Changed to broadcast address, (second argument)
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

  // atached interrupts here 
  attachInterrupt(buttonPin1, [] { handleButton(button1Pos); }, FALLING);
  attachInterrupt(buttonPin2, [] { handleButton(button2Pos); }, FALLING);
  attachInterrupt(buttonPin3, [] { handleButton(button3Pos); }, FALLING);
  attachInterrupt(buttonPin4, [] { handleButton(button4Pos); }, FALLING);
}

void loop() {

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&buttonMessage, sizeof(buttonMessage)); //Changed to broadcast Address (first argument)

    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }

    Serial.println(buttonMessage.posToSend);

  vTaskDelay(100); // Small delay to prevent spamming
}