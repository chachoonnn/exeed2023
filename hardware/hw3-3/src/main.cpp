#include <Arduino.h>
#include <Bounce2.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define builtin 5
#define red 26
#define yellow 25
#define green 33
#define button 27

void Connect_Wifi();
void power_light_saving(void *param);
void builtin_blink(void *param);
void calculate_GET_and_POST(void *param);

TaskHandle_t TaskA = NULL;
TaskHandle_t TaskB = NULL;
TaskHandle_t TaskC = NULL;

Bounce debouncer = Bounce();

String questionUrl = "https://exceed-hardware-stamp465.koyeb.app/question";
String answerUrl = "https://exceed-hardware-stamp465.koyeb.app/answer";

int state = 0;
int time_count = 0;

void setup() {
  Serial.begin(115200);

  Connect_Wifi();

  ledcSetup(1, 5000, 8);
  ledcSetup(0, 5000, 8);
  ledcAttachPin(builtin, 0);
  ledcAttachPin(red, 1);
  ledcAttachPin(yellow, 1);
  ledcAttachPin(green, 1);


  debouncer.attach(button, INPUT_PULLUP);
  debouncer.interval(25);

  xTaskCreatePinnedToCore(builtin_blink, "builtin_blink", 1000, NULL, 0, &TaskA, 0);
  xTaskCreatePinnedToCore(power_light_saving, "power_light_saving", 1000, NULL, 0, &TaskB, 0);
  xTaskCreatePinnedToCore(calculate_GET_and_POST, "calculate_GET_and_POST", 10*1024, NULL, 1, &TaskC, 1);  
}

void loop() {
  // put your main code here, to run repeatedly:
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
  Serial.println("--------------------------------------------------------------------");
}

void builtin_blink(void *param) {
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

void power_light_saving(void *param) {
  while (1) {
    debouncer.update();
    if (state == 0) {
      if (debouncer.fell()) {
        state = 1;
        ledcWrite(1, 255);
      }
    } else if (state == 1) {
      if (debouncer.fell() || time_count >= 5000) {
        state = 0;
        ledcWrite(1, 0);
        time_count = 0;
        continue;
      }
      vTaskDelay(5/portTICK_PERIOD_MS);
      time_count += 5;
    }
  }
}

void calculate_GET_and_POST(void *param) {
  while(1) {
    DynamicJsonDocument doc1(2048);
    HTTPClient http1;

    int ans;
    http1.begin(questionUrl);
    int httpCode = http1.GET();
    if (httpCode >= 200 && httpCode < 300) {
      String payload = http1.getString();
      deserializeJson(doc1, payload);
      int a = doc1["a"].as<int>();
      int b = doc1["b"].as<int>();
      String op = doc1["op"].as<String>();
      String q = doc1["questionId"].as<String>();
      if (op == "+") ans = a + b;
      else if (op == "-") ans = a - b;
      else if (op == "*") ans = a * b;

      Serial.print("a : ");
      Serial.println(a);
      Serial.print("b : ");
      Serial.println(b);
      Serial.print("op : ");
      Serial.println(op);
      Serial.print("ans : ");
      Serial.println(ans);
      Serial.print("questionId : ");
      Serial.println(q);

      String json;
      HTTPClient http2;
      http2.begin(answerUrl);
      http2.addHeader("Content-Type", "application/json");

      DynamicJsonDocument doc2(2048);
      doc2["questionId"] = q;
      doc2["result"] = ans;

      serializeJson(doc2, json);
      http2.POST(json);
      String response = http2.getString();

      Serial.print("response : ");
      Serial.println(response);
      Serial.println("--------------------------------------------------------------------");

      http2.end();
    }
    http1.end();
    vTaskDelay(5000/portTICK_PERIOD_MS);
  }
}
