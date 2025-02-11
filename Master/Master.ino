#include <esp_now.h>
#include <WiFi.h>
#include "BDS.h"

#define maxSpeed 100 
#define maxAccel 200

// bds, switch 1, switch 2, switch 3
int switchPins[4] = {13, 26, 32, 35};

// scene 1, scene 2, scene 3
int positions[3] = {0, 540*16, 1080*16};

// this line is to make sure interrupts are handled correctly 
portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;

// struct for message sent to BDS
typedef struct message {
    volatile int position;
    int maxAccel;
    int maxSpeed;
} message;

message broadcastMessage;
broadcastMessage.maxAccel = maxAccel;
broadcastMessage.maxSpeed = maxSpeed;
broadcastMessage.position = 0;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// declare BDS objects
BDS system1 = BDS(0, { 0xcc, 0xdb, 0xa7, 0x3e, 0xe7, 0xec }, true);
BDS system2 = BDS(0, { 0x08, 0xa6, 0xf7, 0xbc, 0x7f, 0x64 }, false);

// interrupt functions here
void IRAM_ATTR handleButton(int Pos) {
    portENTER_CRITICAL(&synch);
    broadcastMessage.position = Pos;
    portEXIT_CRITICAL(&synch);
}

void IRAM_ATTR handleSwitch() {
    int switchState = digitalRead(switchPins[0]);

    if (switchState == HIGH) {
        system1.setActive(true);
        system2.setActive(false);
    } else {
        system1.setActive(false);
        system2.setActive(true);
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        Serial.println("Err when init espnow");
        return;
    }

    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peerInfo1 = {};
    memcpy(peerInfo1.peer_addr, system1.getBroadcastAddr(), 6);
    peerInfo1.channel = 0;
    peerInfo1.encrypt = false;
    if (esp_now_add_peer(&peerInfo1) != ESP_OK) {
        Serial.println("Failed to add peer 1");
        return;
    }

    esp_now_peer_info_t peerInfo2 = {};
    memcpy(peerInfo2.peer_addr, system2.getBroadcastAddr(), 6);
    peerInfo2.channel = 0;
    peerInfo2.encrypt = false;
    if (esp_now_add_peer(&peerInfo2) != ESP_OK) {
        Serial.println("Failed to add peer 2");
        return;
    }

    // interrupts for buttons
    attatchInterrupt(digitalPinToInterrupt(switchPins[1]), [] {handleButton(positions[0]);}, FALLING)
    attatchInterrupt(digitalPinToInterrupt(switchPins[2]), [] {handleButton(positions[1]);}, FALLING)
    attatchInterrupt(digitalPinToInterrupt(switchPins[3]), [] {handleButton(positions[2]);}, FALLING)

    // interrupt for bds switch 
    attachInterrupt(digitalPinToInterrupt(switchPins[0]), handleSwitch, CHANGE);
}

void loop() {
// TODO: finish loop here

}









