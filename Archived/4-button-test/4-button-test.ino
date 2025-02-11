#include <esp_now.h>
#include <WiFi.h>

#define buttonPin1 18
#define buttonPin2 26
#define buttonPin3 27
#define buttonPin4 19
//All placeholder values
const int button1Pos = 0; 
const int button2Pos = 0;
const int button3Pos = 0;
const int button4Pos = 0;

portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;

typedef struct struct_message {
  volatile int posToSend; 
} struct_message;

struct_message buttonMessage;


void IRAM_ATTR handleButton(int buttonPos) {
  portENTER_CRITICAL(&synch);
  buttonMessage.posToSend = buttonPos;
  portEXIT_CRITICAL(&synch);
}

void setup() {

  Serial.begin(115200);

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);

  // atached interrupts here 
  attachInterrupt(buttonPin1, [] { handleButton(button1Pos); }, FALLING);
  attachInterrupt(buttonPin2, [] { handleButton(button2Pos); }, FALLING);
  attachInterrupt(buttonPin3, [] { handleButton(button3Pos); }, FALLING);
  attachInterrupt(buttonPin4, [] { handleButton(button4Pos); }, FALLING);
}

void loop() {

    Serial.println(buttonMessage.posToSend);

  vTaskDelay(100); // Small delay to prevent spamming
}