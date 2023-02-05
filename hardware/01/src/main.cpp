#include <Arduino.h>
#include <Bounce2.h>

#define BUTTON 27
#define RED 26
#define YELLOW 25
#define GREEN 33
#define LDR 32
int num0 = 0;
int num1 = 0;
int num2 = 0;
Bounce debouncer = Bounce();

void setup() {
  Serial.begin(115200);

  debouncer.attach(BUTTON);
  debouncer.interval(5);
  pinMode(BUTTON, INPUT_PULLUP);

  ledcSetup(0, 5000, 8);
  ledcSetup(1, 5000, 8);
  ledcSetup(2, 5000, 8);
  ledcAttachPin(RED, 0);
  ledcAttachPin(YELLOW, 1);
  ledcAttachPin(GREEN, 2);
}

void loop() {
  //plus binary
  debouncer.update();
  if ( debouncer.fell() ) { 
    if (num2) {
      num2 = 0;
      if (num1) {
        num1 = 0;
        if (num0) {
          num0 = 0;
          num1 = 0;
          num2 = 0;
        } else num0 = 1;
      } else num1 = 1;
    } else num2 = 1;
  }

  //red
  if (num2) {
    ledcWrite(2, map(analogRead(LDR),2000,4095,0,255));
  } else {
    ledcWrite(2, 0);
  }

  //yellow
  if (num1) {
    ledcWrite(1, map(analogRead(LDR),2000,4095,0,255));
  } else {
    ledcWrite(1, 0);
  }

  //green
  if (num0) {
    ledcWrite(0, map(analogRead(LDR),2000,4095,0,255));
  } else {
    ledcWrite(0, 0);
  }
}
