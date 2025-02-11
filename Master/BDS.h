#include <cstdint>

class BDS {
    public:
    int position;
    uint8_t esp32MAC;
    bool active;

    BDS(int position, uint8_t esp32MAC, bool active);
    void setPosition(int newPos);
    void setActive(bool newActive);
    uint8_t getBroadcastAddr();
};

BDS::BDS(int position, uint8_t esp32MAC, bool active) 
    : position(position), esp32MAC(esp32MAC), active(active) {
    
    Serial.println(position);
}

void BDS::setPosition(int newPos) {
    position = newPos;
}

void BDS::setActive(bool newActive) {
    active = newActive;
}

uint8_t BDS::getBroadcastAddr() {
    return esp32MAC;
}
