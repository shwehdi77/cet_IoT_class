//===================================
//ESP8266 Web Server Peripheral Data
//===================================
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include<DHT.h>
#include <ArduinoJson.h>

DHT dht(2, DHT22);
ESP8266WebServer server(80);
//---------------------------------

const char* ssid     = "*********";
const char* password = "*********";
float HUM=0;
float TEMP=0;
//======================================================================
StaticJsonDocument<128> doc;
void setup()
{
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.begin(115200);
  dht.begin();
  
  
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");
  while (WiFi.status() != WL_CONNECTED) {delay(500); Serial.print(".");}
  Serial.println("");
  Serial.println("ESP8266 Web Server");
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  const char * headerkeys[] = {"User-Agent", "Host", "Accept-Language"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  server.collectHeaders(headerkeys, headerkeyssize);
  server.on("/", send_data);
    server.begin();
  
  Serial.println("HTTP server started");
}
//======================================================================
void loop() {server.handleClient();}
void read_sensor()
                  {
                     HUM=dht.readHumidity();
                     TEMP=dht.readTemperature();
                     Serial.println(HUM);
                     Serial.println(TEMP);
                  }
void send_data()
                  {
                    Serial.println("--- New Request Received ---");

                        // حلقة تكرارية لطباعة جميع الترويسات (Headers) المستلمة من المتصفح
                        // for (int i = 0; i < server.headers(); i++) {
                        //   Serial.print(server.headerName(i));  // اسم الترويسة (مثل User-Agent)
                        //   Serial.print(": ");
                        //   Serial.println(server.header(i));    // محتوى الترويسة
                        // }
                    read_sensor();
                    server.sendHeader("Access-Control-Allow-Origin", "*");
                    doc["temp"] = TEMP;
                    doc["hum"]  =  HUM;    
                    char buffer[128];
                    serializeJson(doc, buffer);
                    //client.publish("greenhouse/data", buffer);
                    server.send(200, "application/json", buffer);
                  }                  
