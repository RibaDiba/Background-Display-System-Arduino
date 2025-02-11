// Dir, Step
#include <AccelStepper.h>

#define accel 100
#define speed 200

// system 1 pinout

// system 2 pinout 

int Sys2Pins[2][2] = {
    {27, 25},
    {21, 18}
};

AccelStepper stepper1(1, Sys2Pins[0][1], Sys2Pins[0][0]);
AccelStepper stepper2(1, Sys2Pins[1][1], Sys2Pins[1][0]);

AccelStepper Sys2Motors[2] = {stepper1, stepper2};

void setup() {
    Serial.begin(115200);
}

void loop() {
    for (int i = 0; i < 2; i++) {
        Sys2Motors[i].run();
        Sys2Motors[i].setAcceleration(accel);
        Sys2Motors[i].setMaxSpeed(speed);
    }

    for (int pos = 0; pos < 1600; pos += 10) {
        for (int i = 0; i < 2; i++) {
            Sys2Motors[i].moveTo(pos);
            delay(10);
        }
    }

    delay(2000);

    for (int pos = 1600; pos > 0; pos -= 10) {
        for (int i = 0; i < 2; i++) {
            Sys2Motors[i].moveTo(pos);
            delay(10);
        }
    }

    delay(2000);
}
