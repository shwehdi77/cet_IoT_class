/*
 * ======================================================================================
 * مواصفات التواصل الثنائي مع لوحة الخزانات (API / WebSocket Contract):
 * --------------------------------------------------------------------------------------
 * 1. المنفذ (Port): يعمل خادم WebSocket على المنفذ 81.
 * 2. البيانات المستلمة من الصفحة (RX - JSON): ترسل صفحة المحاكاة القيم التالية للوحة:
 *       - "t1_level" : مستوى الماء الحالي في الخزان الأول
 *       - "t2_level" : مستوى الماء الحالي في الخزان الثاني
 *       - "min_t1"   : الحد الأدنى المسموح به للخزان الأول
 *       - "max_t2"   : الحد الأقصى المسموح به للخزان الثاني
 * 3. البيانات المرسلة إلى الصفحة (TX - JSON): تتوقع الصفحة استلام مفاتيح الصمامات:
 *       - "valve1"   : حالة صمام التغذية (true أو false)
 *       - "valve2"   : حالة صمام التفريغ (true أو false)
 * ======================================================================================
 */

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// إعدادات الشبكة (عدّلها حسب شبكتك)
const char* ssid     = "اكتب اسم الشبكة";
const char* password = "اكتب رمز الشبكة";

WebSocketsServer webSocket = WebSocketsServer(81);

// متغيرات حالة النظام
float currentL1 = 0, currentL2 = 0; // قيم الخزانات القادمة من الصفحة
int limitMin1 = 0, limitMax2 = 0;   // حدود الأمان القادمة من الصفحة
bool Valve1_State = false;
bool Valve2_State = false;

// أطراف الأزرار الفيزيائية للتبديل اليدوي (مفعّلة كـ INPUT_PULLUP في setup)
const byte valve1pin = 4;
const byte valve2pin = 0;

unsigned long lastUpdate = 0;
const long interval = 200; // تحديث كل 200 ملي ثانية

// تصريح مسبق للدوال
void setupWiFiAndServer();
void Pin_check();
void Change_Valve_State();
void read_and_broadcast();

void setup() {
  Serial.begin(115200);
  delay(2000);
  pinMode(valve1pin, INPUT_PULLUP);
  pinMode(valve2pin, INPUT_PULLUP);

  setupWiFiAndServer(); // تشغيل الواي فاي وخادم WebSocket
}

void loop() {
  webSocket.loop(); // مراقبة أحداث واتصالات WebSocket باستمرار

  if (millis() - lastUpdate > interval) {
    lastUpdate = millis();
    
    // [المطلوب 1]: استدعِ دالة فحص الأزرار اليدوية ودالة تحديث وبث حالة الصمامات
    
  }
}

// ======================================================================================
// دالة معالجة أحداث الـ WebSocket (استقبال بيانات الخزانات من المتصفح)
// ======================================================================================
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED:
      Serial.printf("[%u] Connected from url: %s\n", num, payload);
      break;

    case WStype_TEXT:
      // [المطلوب 2 - استقبال وتفكيك JSON]:
      // 1. أنشئ كائن StaticJsonDocument<256> باسم doc
      // 2. فكّك البيانات القادمة في المعامل الثالث (payload) باستخدام deserializeJson(doc, payload)
      


      if (!error) {
        if (doc.containsKey("t1_level")) {
          // [المطلوب 3]: استخرج القيم من doc وخزنها في المتغيرات العامة:
          // currentL1, currentL2, limitMin1, limitMax2
          // (راجع أسماء المفاتيح في مواصفات التواصل أعلى الكود)
          



          Serial.printf("Received Tank Data -> L1: %.1f, L2: %.1f, Min1: %d, Max2: %d\n", 
                         currentL1, currentL2, limitMin1, limitMax2);
        }
      }
      break;
  }
}

// ======================================================================================
// [المطلوب 4]: دالة التحقق من حالة الأزرار اليدوية
// ======================================================================================
void Pin_check() {
  // اكتب أكواد التحقق من الأزرار هنا:
  // 1. اقرأ حالة الطرفين (valve1pin و valve2pin) باستخدام digitalRead().
  // 2. تذكّر أن الأطراف مهيأة كـ INPUT_PULLUP، لذا فإن الضغط على الزر يعطي قيمة LOW (0).
  // 3. عند الضغط على زر الصمام 1، اعكس حالة المتغير Valve1_State (!Valve1_State).
  // 4. عند الضغط على زر الصمام 2، اعكس حالة المتغير Valve2_State (!Valve2_State).
  

}

// ======================================================================================
// [المطلوب 5]: دالة منطق التحكم والأمان الآلي في الصمامات
// ======================================================================================
void Change_Valve_State() {
  // اكتب شروط التحكم الآلي والأمان هنا:
  // 1. أغلق الصمام الأول (Valve1_State = false) إذا انخفض مستوى الخزان الأول عن أو يساوي
  //    الحد الأدنى (currentL1 <= limitMin1)، أو إذا وصل الخزان الثاني للحد الأقصى (currentL2 >= limitMax2).
  // 2. أغلق الصمام الثاني (Valve2_State = false) إذا كان مستوى الخزان الثاني أقل من أو يساوي
  //    الحد الأقصى المسموح به (currentL2 <= limitMax2).
  

}

// ======================================================================================
// دالة تجهيز وبث حالة الصمامات إلى صفحة المحاكاة
// ======================================================================================
void read_and_broadcast() {
  // [المطلوب 6]: استدعِ دالة Change_Valve_State() لتطبيق شروط الأمان قبل الإرسال
  

  // تجهيز البيانات بصيغة JSON
  StaticJsonDocument<128> doc;

  // [المطلوب 7]: أضف حالة الصمامين إلى كائن doc بحيث تطابق ما تنتظره الصفحة ("valve1" و "valve2")
  // استبدل **** بالمتغيرات التي تخزن حالة الصمامين
  // doc["اسم_المفتاح_في_الصفحة"] = ****;
  


  char buffer[128];
  serializeJson(doc, buffer);

  // [المطلوب 8]: بث حزمة JSON (buffer) لجميع المتصفحات المتصلة عبر webSocket
  

  Serial.print("Sent to Clients: ");
  Serial.println(buffer);
}

// ======================================================================================
// إعدادات الاتصال الخلفية (جاهزة)
// ======================================================================================
void setupWiFiAndServer() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket Server Started on Port 81");
}