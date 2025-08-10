#include <esp_now.h>
#include <WiFi.h>
#include "BDS.h"

// define structs ({arr, of, positions}, maxSpeed, maxAccel)
stepperMotor sys1S1 = {{300, 600, 1200}, 400, 200};
stepperMotor sys1S2 = {{300, 600, 1200}, 400, 200};
stepperMotor sys2S1 = {{300, 600, 1200}, 400, 200};
stepperMotor sys2S2 = {{300, 600, 1200}, 400, 200};

// bds, switch 1, switch 2, switch 3  
int switchPins[4] = {13, 35, 32, 26};

// first 3 are for scenes and 14 and 23` are for switching bds indicator 
int ledPins[5] = {22, 33, 27, 14, 23};

// this line is to make sure interrupts are handled correctly 
portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;

// callback when data has been sent out
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// declare bds objects 
BDS system1 = BDS((uint8_t[]){0x08, 0xa6, 0xf7, 0xb1, 0x10, 0x90}, sys1S1, sys1S2, false);
BDS system2 = BDS((uint8_t[]){0x08, 0xa6, 0xf7, 0xbc, 0x7f, 0x64}, sys2S1, sys2S2, false);

// declare message variable for broadcast data
boardData message;

void initPins() {
    for (int i = 0; i < 4; i++) {
        pinMode(switchPins[i], INPUT_PULLUP);
    }
    for (int i = 0; i < 5; i++) {
        pinMode(ledPins[i], OUTPUT);
    }
}

void initBDSLed() {
    int switchState = digitalRead(switchPins[0]);
    system1.setActive(switchState == HIGH);
    system2.setActive(switchState == LOW);

    system1.getActive() ? (digitalWrite(ledPins[3], HIGH), digitalWrite(ledPins[4], LOW)) 
    : (digitalWrite(ledPins[3], LOW), digitalWrite(ledPins[4], HIGH));
}

void IRAM_ATTR handleSwitch() {
    int switchState = digitalRead(switchPins[0]);
    system1.setActive(switchState == HIGH);
    system2.setActive(switchState == LOW);
}


void IRAM_ATTR handleButton(int index) {
    portENTER_CRITICAL(&synch);
    if (system1.getActive()) {
        system1.setPositionIndex(index);
    } else {
        system2.setPositionIndex(index);
    }
    portEXIT_CRITICAL(&synch);
}

// update leds based on position
void writePosLed(int index) {
    if (index == 0) {
        digitalWrite(ledPins[0], HIGH);
        digitalWrite(ledPins[1], LOW);
        digitalWrite(ledPins[2], LOW);
    }
    else if (index == 1) {
        digitalWrite(ledPins[0], LOW);
        digitalWrite(ledPins[1], HIGH);
        digitalWrite(ledPins[2], LOW);
    }
    else if (index == 2) {
        digitalWrite(ledPins[0], LOW);
        digitalWrite(ledPins[1], LOW);
        digitalWrite(ledPins[2], HIGH);
    }
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
    attachInterrupt(digitalPinToInterrupt(switchPins[1]), [] { handleButton(0); }, FALLING);
    attachInterrupt(digitalPinToInterrupt(switchPins[2]), [] { handleButton(1); }, FALLING);
    attachInterrupt(digitalPinToInterrupt(switchPins[3]), [] { handleButton(2); }, FALLING);

    // interrupt for bds switch 
    attachInterrupt(digitalPinToInterrupt(switchPins[0]), handleSwitch, CHANGE);
}

void loop() {
    if (system1.getActive()) {
        digitalWrite(ledPins[3], HIGH);
        digitalWrite(ledPins[4], LOW);
        system1.setBroadData();
        message = system1.getBroadData();
        writePosLed(system1.getPositionIndex());
        // printf("Sending to System 1 with: %d\n", broadcastMessage.position);
        esp_err_t result = esp_now_send(system1.getBroadcastAddr(), (uint8_t *)&message, sizeof(message));
        if (result == ESP_OK) {
            Serial.print("Sent with Success");
        }
    } else {
        digitalWrite(ledPins[3], LOW);
        digitalWrite(ledPins[4], HIGH);
        system2.setBroadData();
        message = system2.getBroadData();
        writePosLed(system2.getPositionIndex());
        // printf("Sending to System 1 with: %d\n", broadcastMessage.position);
        esp_err_t result = esp_now_send(system2.getBroadcastAddr(), (uint8_t *)&message, sizeof(message));
        if (result == ESP_OK) {
            Serial.print("Sent with Success");
        }
    }    
}

