// lib 
#include <esp_now.h>
#include <WiFi.h> 
#include <AccelStepper.h>

#define stepPin 22
#define dirPin 23

#define maxSpeed 400
#define maxAccel 100
#define totalSteps 1600

AccelStepper stepper(1, stepPin, dirPin);

// struct 
typedef struct struct_message {
    volatile int posToSend;
} struct_message;

struct_message data;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  Serial.println(data.posToSend);
  stepper.moveTo(data.posToSend);

  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(maxAccel);

  stepper.run();
}

void setup() {

  Serial.begin(115200);

  data.posToSend = 0;

  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(maxAccel);

  stepper.setCurrentPosition(data.posToSend);

  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // callback function
  esp_now_register_recv_cb(OnDataRecv);

}

void loop() {
  stepper.run();
}