#include <AccelStepper.h>

// pins 
#define stepPin 22
#define dirPin 23
#define buttonPin1 18 
#define buttonPin2 26 
#define buttonPin3 27
#define buttonPin4 19

#define maxSpeed 4000
#define maxAccel 1000
#define totalSteps 1600

AccelStepper stepper(1, stepPin, dirPin);

// Button states
int lastButtonState1 = HIGH; // HIGH because of INPUT_PULLUP
int lastButtonState2 = HIGH;
int lastButtonState3 = HIGH;
int lastButtonState4 = HIGH;

long targetPosition = 0; // Initial motor position

void setup() {
  Serial.begin(115200);

  // Set max speed and acceleration
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(maxAccel);

  // Configure button pins as inputs with pullup resistors
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);

  // Set initial motor position
  stepper.setCurrentPosition(0);
}

void loop() {
  // Read the current state of the buttons
  int buttonState1 = digitalRead(buttonPin1);
  int buttonState2 = digitalRead(buttonPin2);
  int buttonState3 = digitalRead(buttonPin3);
  int buttonState4 = digitalRead(buttonPin4);

  // Check for button press events (button state changes from HIGH to LOW)
  if (buttonState1 == LOW && lastButtonState1 == HIGH) {
    targetPosition = 0;
    stepper.moveTo(targetPosition);
    Serial.println("Scene 1");
  }
  else if (buttonState2 == LOW && lastButtonState2 == HIGH) {
    targetPosition = 400;
    stepper.moveTo(targetPosition);
    Serial.println("Scene 2");
  }
  else if (buttonState3 == LOW && lastButtonState3 == HIGH) {
    targetPosition = 800;
    stepper.moveTo(targetPosition);
    Serial.println("Scene 3");
  }
  else if (buttonState4 == LOW && lastButtonState4 == HIGH) {
    targetPosition = 1200;
    stepper.moveTo(targetPosition);
    Serial.println("Scene 4");
  }

  // Save the current button states for comparison in the next loop iteration
  lastButtonState1 = buttonState1;
  lastButtonState2 = buttonState2;
  lastButtonState3 = buttonState3;
  lastButtonState4 = buttonState4;

  // Run the motor to the target position
  stepper.run();
}
