#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define builtin 5
int delay_now = 0;
const String baseUrl = "https://exceed-hardware-stamp465.koyeb.app/leddelay";

TaskHandle_t TaskA = NULL;
TaskHandle_t TaskB = NULL;

void Connect_Wifi();
void led(void *param);
void get_delay(void *param);

void setup() {
  Serial.begin(115200);

  Connect_Wifi();

  pinMode(builtin, OUTPUT);
  digitalWrite(builtin, HIGH);

  xTaskCreatePinnedToCore(led, "led", 1000, NULL, 1, &TaskB, 0);
  xTaskCreatePinnedToCore(get_delay, "get_delay", 10*1024, NULL, 1, &TaskA, 1);
}

void loop() {

}

void Connect_Wifi()
{
  const char *ssid = "choon";
  const char *password = "12345678";
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("OK! IP=");
  Serial.println(WiFi.localIP());
  Serial.println("----------------------------------");
}

void led(void *param) {
  while(1) {
    if (delay_now == 0) {
      digitalWrite(builtin, HIGH);
    }
    else {
      digitalWrite(builtin, LOW);
      vTaskDelay(delay_now/portTICK_PERIOD_MS);
      digitalWrite(builtin, HIGH);
      vTaskDelay(delay_now/portTICK_PERIOD_MS);
    }
  }
}

void get_delay(void *param) {
  while(1) {
    DynamicJsonDocument doc(2048);
    HTTPClient http;
    http.begin(baseUrl);
    int httpCode = http.GET();
    if (httpCode >= 200 && httpCode < 300) {
      String payload = http.getString();
      deserializeJson(doc, payload);
      delay_now = doc["value"];
      Serial.print("delay_now : ");
      Serial.println(delay_now);
    }
    http.end();
    vTaskDelay(5000/portTICK_PERIOD_MS);
  }
}

