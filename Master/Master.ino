#include <esp_now.h>
#include <WiFi.h>
#include "BDS.h"

int maxSpeed = 100;
int maxAccel = 200;

// bds, switch 1, switch 2, switch 3
int switchPins[4] = {13, 35, 32, 26};

// scene 1, scene 2, scene 3
int positions[3] = {0, 540 * 16, 1080 * 16};

// first 3 are for scenes and 14 and 23 are for switching bds indicator 
int ledPins[5] = {22, 33, 27, 14, 23};

// this line is to make sure interrupts are handled correctly 
portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;

// struct for message sent to BDS
typedef struct message {
    volatile int position;
    int maxAccel;
    int maxSpeed;
    uint8_t* address;
} message;

volatile message broadcastMessage = {0, maxAccel, maxSpeed};

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    // Serial.print("Last Packet Send Status: ");
    // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void initPins() {
    for (int i = 0; i < 4; i++) {
        pinMode(switchPins[i], INPUT_PULLUP);
    }
    for (int i = 0; i < 5; i++) {
        pinMode(ledPins[i], OUTPUT);
    }
}

// declare BDS objects
BDS system1 = BDS(0, (uint8_t[]){0xcc, 0xdb, 0xa7, 0x3e, 0xe7, 0xec}, false);
BDS system2 = BDS(0, (uint8_t[]){0x08, 0xa6, 0xf7, 0xbc, 0x7f, 0x64}, false);


// interrupt functions here
void IRAM_ATTR handleButton(int Pos) {
    portENTER_CRITICAL(&synch);
    if (system1.getActive() == true) {
        system1.setPosition(Pos);
    } else {
        system2.setPosition(Pos);
    }
    portEXIT_CRITICAL(&synch);
}

void IRAM_ATTR handleSwitch() {
    int switchState = digitalRead(switchPins[0]);
    system1.setActive(switchState == HIGH);
    system2.setActive(switchState == LOW);
}

void initBDSLed() {
    int switchState = digitalRead(switchPins[0]);
    system1.setActive(switchState == HIGH);
    system2.setActive(switchState == LOW);

    system1.getActive() ? (digitalWrite(ledPins[3], HIGH), digitalWrite(ledPins[4], LOW)) 
    : (digitalWrite(ledPins[3], LOW), digitalWrite(ledPins[4], HIGH));
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    initPins();
    initBDSLed();

    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, system1.getBroadcastAddr(), 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer 1");
        return;
    }

    memcpy(peerInfo.peer_addr, system2.getBroadcastAddr(), 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer 2");
        return;
    }

    // interrupts for buttons
    attachInterrupt(digitalPinToInterrupt(switchPins[1]), [] { handleButton(positions[0]); }, FALLING);
    attachInterrupt(digitalPinToInterrupt(switchPins[2]), [] { handleButton(positions[1]); }, FALLING);
    attachInterrupt(digitalPinToInterrupt(switchPins[3]), [] { handleButton(positions[2]); }, FALLING);

    // interrupt for bds switch 
    attachInterrupt(digitalPinToInterrupt(switchPins[0]), handleSwitch, CHANGE);
}

void writePosLed(int pos) {
    if (pos == positions[0]) {
        digitalWrite(ledPins[0], HIGH);
        digitalWrite(ledPins[1], LOW);
        digitalWrite(ledPins[2], LOW);
    }
    else if (pos == positions[1]) {
        digitalWrite(ledPins[0], LOW);
        digitalWrite(ledPins[1], HIGH);
        digitalWrite(ledPins[2], LOW);
    }
    else if (pos == positions[2]) {
        digitalWrite(ledPins[0], LOW);
        digitalWrite(ledPins[1], LOW);
        digitalWrite(ledPins[2], HIGH);
    }
}


void loop() {
    // Serial.println(system1.getActive() ? "Sending to System 1" : "Sending to System 2");
    // system1.getActive() ? (digitalWrite(ledPins[3], HIGH), digitalWrite(ledPins[4], LOW), writePosLed(system1.getPosition())) 
    //     : (digitalWrite(ledPins[3], LOW), digitalWrite(ledPins[4], HIGH), writePosLed(system2.getPosition()));

    // bool isMatching = system1.getActive() 
    // ? (broadcastMessage.position == system1.getPosition()) 
    // : (broadcastMessage.position == system2.getPosition());

    // Serial.println(broadcastMessage.position);

    if (system1.getActive()) {
        digitalWrite(ledPins[3], HIGH);
        digitalWrite(ledPins[4], LOW);
        broadcastMessage.address = system1.getBroadcastAddr();
        broadcastMessage.position = system1.getPosition();
        writePosLed(system1.getPosition());
        printf("Sending to System 1 with: %d\n", broadcastMessage.position);
    } else {
        digitalWrite(ledPins[3], LOW);
        digitalWrite(ledPins[4], HIGH);
        writePosLed(system2.getPosition());
        broadcastMessage.address = system2.getBroadcastAddr();
        broadcastMessage.position = system2.getPosition();
        printf("Sending to System 2 with: %d\n", broadcastMessage.position);
    }    

    esp_err_t result = esp_now_send(broadcastMessage.address, (uint8_t *)&broadcastMessage, sizeof(broadcastMessage));
    if (result == ESP_OK) {
        Serial.print("Sent with Success");
    }
}
