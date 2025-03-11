#pragma once
#include <cstdint>

typedef struct stepperMotor {
    int position[3];
    int maxSpeed;
    int maxAccel;
} stepperMotor;

typedef struct boardData {
    int s1Position;
    int s2Position;
    
    int s1MaxSpeed;
    int s2MaxSpeed;

    int s1MaxAccel;
    int s2MaxAccel;
} boardData;

class BDS {
    public:
        uint8_t *esp32MAC;
        bool active;
        stepperMotor s1;
        stepperMotor s2;

        boardData dataSend;

        BDS(uint8_t *esp32MAC, stepperMotor s1, stepperMotor s2, bool active);
    
        void setPositionIndex(int newIndex);
        void setBroadData();
        boardData getBroadData();
        bool getActive();
        uint8_t* getBroadcastAddr();
        int getPositionIndex();
        void setActive(bool active);
    private:
        int positionIndex = 0;
}

BDS::BDS(uint8_t *esp32MAC, stepperMotor s1, stepperMotor s2, bool active) 
    : esp32MAC(esp32MAC), s1(s1), s2(s2), active(active) {

}

void BDS::setBroadData() {
    dataSend.s1Position = s1.position[positionIndex];
    dataSend.s2Position = s2.position[positionIndex];

    dataSend.s1MaxSpeed = s1.maxSpeed;
    dataSend.s2MaxSpeed = s2.maxSpeed;

    dataSend.s1MaxAccel = s1.maxAccel;
    dataSend.s2MaxAccel = s2.maxAccel;

}

int BDS::getPositionIndex() {
    return positionIndex;
}

uint8_t* BDS::getBroadcastAddr() {
    return esp32MAC;
}

boardData BDS::getBroadData() {
    return dataSend;
}

void BDS::setPositionIndex(int newIndex) {
    positionIndex = newIndex;
}

bool BDS::getActive() {
    return active;
}

void BDS::setActive(bool active) {
    this->active = active;
}
