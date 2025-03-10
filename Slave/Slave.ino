#include <esp_now.h>
#include <WiFi.h> 
#include <AccelStepper.h>

// motor pins {stepPin, dirPin}
// this is for system 2
int motor1[2] = {18, 21};
int motor2[2] = {25, 27};

// this is for system 1
// int motor1[2] = {19, 3};
// int motor2[2] = {27, 25};

AccelStepper stepper1(1, motor1[0], motor1[1]);
AccelStepper stepper2(2, motor2[0], motor2[1]);

// this is copied from Master.ino
typedef struct broadData {
  int s1Position;
  int s2Position;
  
  int s1MaxSpeed;
  int s2MaxSpeed;

  int s1MaxAccel;
  int s2MaxAccel;
} broadData;


broadData data;

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