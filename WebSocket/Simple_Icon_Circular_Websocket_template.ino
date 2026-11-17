/*
 * ======================================================================================
 * مواصفات التواصل مع واجهة العرض (API / WebSocket Contract):
 * --------------------------------------------------------------------------------------
 * 1. المنفذ (Port): يعمل خادم WebSocket على المنفذ 81.
 * 2. صيغة البيانات (JSON Format): يجب أن تتطابق أسماء المفاتيح في كائن JSON المرسل
 *    مع الأسماء التي تقرأها صفحة الويب (JavaScript).
 *    - في صفحات العرض الجاهزة الحالية، تتوقع الصفحة المفتاحين: "temp" و "hum"
 *      (مثال: {"temp":25.4,"hum":60.2}).
 *    - أما أسماء المتغيرات الداخلية في كود الأردوينو (C++) فلك حرية اختيارها بشرط
 *      توحيدها داخل الدالة.
 * ======================================================================================
 */

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h> // مكتبة WebSocket الأساسية
#include <DHT.h>
#include <ArduinoJson.h>

// إعدادات الحساس والاتصال
DHT dht(13, DHT11); // غير رقم المدخل حسب توصيلك، وغير نوع الحساس حسب المستخدم لديك
const char* ssid     = "اكتب اسم الشبكة";
const char* password = "اكتب رمز الشبكة";

// إنشاء كائن WebSocket على المنفذ 81
WebSocketsServer webSocket = WebSocketsServer(81);

unsigned long lastUpdate = 0;
const long interval = 2000; // إرسال البيانات كل ثانيتين

// تصريح مسبق للدوال
void setupWiFiAndServer();
void read_and_broadcast();

void setup() {
  Serial.begin(115200);
  dht.begin();
  setupWiFiAndServer(); // تشغيل الواي فاي وخادم WebSocket
}

void loop() {
  webSocket.loop(); // إدارة اتصالات وأحداث الـ WebSocket باستمرار

  // إرسال البيانات بشكل تلقائي (Broadcasting) كل فترة زمنية
  if (millis() - lastUpdate > interval) {
    lastUpdate = millis();
    
    // [المطلوب 1]: ضع دالة القراءة والإرسال هنا
    
  }
}

// دالة معالجة أحداث الـ WebSocket (Event-Driven)
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num); // حدث انقطاع الاتصال
      break;

    case WStype_CONNECTED:
      Serial.printf("[%u] Connected from url: %s\n", num, payload); // حدث اتصال جديد
      // [المطلوب 2 - اختياري]: يمكنك استدعاء دالة الإرسال هنا لإرسال قراءة فورية فور اتصال المتصفح
      break;

    case WStype_TEXT:
      Serial.printf("[%u] Received Text: %s\n", num, payload); // حدث استقبال نص/أوامر من المتصفح
      break;
  }
}

// دالة تحصيل القراءات وبناء حزمة JSON وبثها
void read_and_broadcast() {
  // [المطلوب 3]: اكتب أوامر قراءة الحساس وعرّف متغيرات بالأسماء التي تختارها
  // لقراءة الرطوبة استخدم: dht.readHumidity();
  // لقراءة الحرارة استخدم: dht.readTemperature();
  


  // [المطلوب 4]: استبدل **** بأسماء متغيرات الرطوبة والحرارة التي عرّفتها في الأعلى
  if (isnan(****) || isnan(****)) {
    Serial.println("sensor reading failed");
    return;
  }
  else {
    Serial.println(****);
    Serial.println(****);
  }

  // تجهيز البيانات بصيغة JSON
  StaticJsonDocument<128> doc;

  // [المطلوب 5]: كرر السطر التالي على عدد المتغيرات المراد إرسالها
  // - ضع داخل الأقواس ["..."] الاسم المطابق لما في صفحة الويب 

  // - استبدل **** باسم المتغير المقابل له في هذا الكود

  // doc["اسم_المفتاح_في_الصفحة"] = ****;///
  


  char buffer[128];
  serializeJson(doc, buffer);

  // البث لجميع المشتركين (Broadcasting)
  // هنا لا ننتظر Request، نحن نقوم بـ Push للبيانات مباشرة
  webSocket.broadcastTXT(buffer);

  Serial.print("Sent to Clients: ");
  Serial.println(buffer);
}

// إعدادات الاتصال الخلفية (جاهزة)
void setupWiFiAndServer() {
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