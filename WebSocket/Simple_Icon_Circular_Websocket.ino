#include <ESP8266WiFi.h>
#include <WebSocketsServer.h> // مكتبة WebSocket الأساسية
#include <DHT.h>
#include <ArduinoJson.h>

// إعدادات الحساس والاتصال
DHT dht(13, DHT11);
const char* ssid     = "*****";
const char* password = "*****";

// إنشاء كائن WebSocket على المنفذ 81
WebSocketsServer webSocket = WebSocketsServer(81);

float HUM = 0, TEMP = 0;
unsigned long lastUpdate = 0;
const long interval = 2000; // إرسال البيانات كل ثانيتين

void setup() {
  Serial.begin(115200);
  dht.begin();

  // الاتصال بالوايفاي
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // تشغيل خادم الـ WebSocket وتحديد دالة الأحداث
  webSocket.begin();
  webSocket.onEvent(webSocketEvent); 

  Serial.println("WebSocket Server Started on Port 81");
}

void loop() {
  webSocket.loop(); // إدارة اتصالات الـ WebSocket

  // إرسال البيانات بشكل تلقائي (Broadcasting) كل فترة زمنية
  if (millis() - lastUpdate > interval) {
    lastUpdate = millis();
    read_and_broadcast();
  }
}

// دالة معالجة الأحداث (onEvent)
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num); // حدث انقطاع الاتصال
      break;
    case WStype_CONNECTED:
      Serial.printf("[%u] Connected from url: %s\n", num, payload); // حدث اتصال جديد
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Received Text: %s\n", num, payload); // استقبال أوامر من المتصفح
      break;
  }
}

void read_and_broadcast() {
  HUM = dht.readHumidity();
  TEMP = dht.readTemperature();

  if (isnan(HUM) || isnan(TEMP)) {
  Serial.print("sensor reading failed ");
  HUM=0;
  TEMP=0;
  //return;
  }
  else{

      Serial.println(HUM);
      Serial.println(TEMP);
  }
  // تجهيز البيانات بصيغة JSON
  StaticJsonDocument<128> doc;
  doc["temp"] = TEMP;
  doc["hum"]  = HUM;

  char buffer[128];
  serializeJson(doc, buffer);

  // البث لجميع المشتركين (Broadcasting)
  // هنا لا ننتظر Request، نحن نقوم بـ Push للبيانات
  webSocket.broadcastTXT(buffer);
  
  Serial.print("Sent to Clients: ");
  Serial.println(buffer);
}