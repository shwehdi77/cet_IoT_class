#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <DHT.h>
#include <ArduinoJson.h>

// إعدادات الحساس والاتصال
// DHT dht(2, DHT22);
// const char* ssid     = "netis_AC2414";
// const char* password = "password";

const char* ssid     = "shwehdi4g";
const char* password = "Libya(*)42311";
WebSocketsServer webSocket = WebSocketsServer(81);

// float HUM = 0, TEMP = 0;
float currentL1 = 0, currentL2 = 0; // قيم الخزانات القادمة من الصفحة
int limitMin1 = 0, limitMax2 = 0;   // حدود الأمان القادمة من الصفحة
bool Valve1_State=false;
bool Valve2_State=false;
const byte valve1pin=4;
const byte valve2pin=0;
unsigned long lastUpdate = 0;
const long interval = 200; 
const char* pin1_tag= "Valve1"; 
void Pin_chack(){

              byte Pin1_State=digitalRead(valve1pin);
              delay(10);
               if(!Pin1_State){
                              Valve1_State=!Valve1_State;
                              Serial.print("valve 1 is: ");
                              Serial.println( Valve1_State);}
               byte Pin2_State=digitalRead(valve2pin);
              delay(10);
               if(!Pin2_State){
                              Valve2_State=!Valve2_State;
                              Serial.print("valve 2 is: ");
                              Serial.println( Valve2_State);}                    
               }

void setup() {
  Serial.begin(115200);
//   dht.begin();
 delay(2000);
  pinMode(valve1pin,INPUT_PULLUP);
  pinMode(valve2pin,INPUT_PULLUP);
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

void loop() {
  webSocket.loop(); 

  if (millis() - lastUpdate > interval) {
    lastUpdate = millis();
    Pin_chack();
    read_and_broadcast();
  }
}

// --- الجزء المضاف لمعالجة بيانات المتصفح (JSON) ---
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      Serial.printf("[%u] Connected from url: %s\n", num, payload);
      break;
    case WStype_TEXT:
      // استقبال البيانات وتفكيك JSON القادم من المتصفح
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        if (doc.containsKey("t1_level")) {
          currentL1 = doc["t1_level"];
          currentL2 = doc["t2_level"];
          limitMin1 = doc["min_t1"];
          limitMax2 = doc["max_t2"];
        //   Change_Valve_State(); 
          // يمكنك هنا إضافة Logic للتحكم بريلي (Relay) بناءً على القيم
          Serial.printf("Received Tank Data -> L1: %.1f, L2: %.1f, Min1: %d, Max2: %d\n", 
                         currentL1, currentL2, limitMin1, limitMax2);
        }
      }
      break;
  }
}
//=======================================================/
void Change_Valve_State(){
      if(currentL1<=limitMin1 || currentL2>=limitMax2){
      
        Valve1_State=false; 
      } 
      if(currentL2<=limitMax2){
      
        Valve2_State=false; 
      } 

}
//======================================================/
void read_and_broadcast() {
  Serial.printf("Received Tank Data -> L1: %.1f, L2: %.1f, Min1: %d, Max2: %d\n", 
                         currentL1, currentL2, limitMin1, limitMax2);
  StaticJsonDocument<128> doc;
  Change_Valve_State();
  doc["valve1"] = Valve1_State; 
  doc["valve2"] = Valve2_State; 
  doc[pin1_tag] = String(Valve2_State); 
//  doc[V2] = Valve2_State; 
  char buffer[128];
  serializeJson(doc, buffer);
  webSocket.broadcastTXT(buffer);
  
  Serial.print("Sent to Clients: ");
  Serial.println(buffer);
}


