// lib 
#include <esp_now.h>
#include <WiFi.h> 
#include <AccelStepper1.h>

#define stepPin1 22
#define dirPin1 23

#define stepPin2 18
#define dirPin2 19

// this is just for reference
#define totalSteps 1600

AccelStepper stepper1(1, stepPin1, dirPin1);
AccelStepper stepper2(1, stepPin2, dirPin2);

// struct 
typedef struct struct_message {
    volatile int posToSend;
    int maxAccel;
    int maxSpeed;
} struct_message;

struct_message data;

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  stepper1.setMaxSpeed(data.maxSpeed);
  stepper1.setAcceleration(data.maxAccel);

  stepper2.setMaxSpeed(data.maxSpeed);
  stepper2.setAcceleration(data.maxAccel);

  Serial.println(data.posToSend);
  stepper1.moveTo(data.posToSend);
  stepper2.moveTo(data.posToSend);

  stepper1.run();
  stepper2.run();
}

void setup() {

  Serial.begin(115200);

  data.posToSend = 0;

  stepper1.setCurrentPosition(data.posToSend);

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
  stepper1.run();
  stepper2.run();
}