#include <esp_now.h>
#include <WiFi.h> 
#include <AccelStepper.h>

// motor pins {stepPin, dirPin}
int motor1[2] = {22, 23};
int motor2[2] = {18, 19};

AccelStepper stepper1(1, motor1[0], motor1[1]);
AccelStepper stepper2(2, motor2[0], motor2[1]);

// this is copied from Master.ino
typedef struct message {
    volatile int position;
    int maxAccel;
    int maxSpeed;
    uint8_t* address;
} message;

message data;

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  stepper1.setMaxSpeed(data.maxSpeed);
  stepper1.setAcceleration(data.maxAccel);

  stepper2.setMaxSpeed(data.maxSpeed);
  stepper2.setAcceleration(data.maxAccel);

  Serial.println(data.position);
  stepper1.moveTo(data.position);
  stepper2.moveTo(data.position);

  stepper1.run();
  stepper2.run();
}

void setup() {

  Serial.begin(115200);

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