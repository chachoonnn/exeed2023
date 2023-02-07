#include <Arduino.h>
#include <Bounce2.h>

#define red 26
#define button 27

TaskHandle_t TaskA = NULL;
TaskHandle_t TaskB = NULL;

Bounce debouncer = Bounce();
int count = 0;

void led(void *param);
void count_switch(void *param);

void setup() {
  Serial.begin(115200);
  
  ledcSetup(0, 5000, 8);
  ledcAttachPin(red, 0);

  debouncer.attach(button, INPUT_PULLUP);
  debouncer.interval(25);

  xTaskCreatePinnedToCore(led, "led", 1000, NULL, 1, &TaskA, 0);
  xTaskCreatePinnedToCore(count_switch, "count_switch", 1000, NULL, 1, &TaskB, 1);

  Serial.println("Start");
  Serial.println("-----------------");
}

void loop() {
  // put your main code here, to run repeatedly:
}

void led(void *param) {
  while(1) {
    for (int i = 0; i < 255; i++) {
      ledcWrite(0, i);
      vTaskDelay(5/portTICK_PERIOD_MS);
    }
    for (int i = 255; i > 0; i--) {
      ledcWrite(0, i);
      vTaskDelay(5/portTICK_PERIOD_MS);
    }
  }
}

void count_switch(void *param) {
  while(1) {
    debouncer.update();
    if (debouncer.fell()) {
      count++;
      Serial.print("Count = ");
      Serial.println(count);
    }
  }
}