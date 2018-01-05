#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>

#define button 4

WiFiUDP UDP;

IPAddress pcIp(192, 168, 1, 101);
byte mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

IPAddress myIp(192, 168, 1, 106);
IPAddress netmask(255, 255, 255, 0);
IPAddress gateway(192, 168, 1, 1);

//const char* ssid = "AirPort-able";
//const char* pass = "Welkom123";

//makersbase setup
const char* ssid = "Hotspot_NieuweVeste";

WiFiServer server(80);

void sendWOL();

bool state = false;

void setup() {
  Serial.begin(9600);

  Serial.println("=====");
  Serial.println("MAC: " + WiFi.macAddress());
  Serial.println("=====");
  
  //WiFi.config(myIp, gateway, netmask);
  WiFi.begin(ssid);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("verbonden! " + IpToString(WiFi.localIP()));
  pinMode(button, OUTPUT);
  
  server.begin();  
}

void loop() {
  WiFiClient client = server.available();

  if(client) {
    String header;
    while(client.connected()) {
      
      if(client.available()) {
        String line = client.readStringUntil('\r');
        header += line;
        Serial.println(line);
        
        if(line.length() == 1 && line[0] == '\n') {
          
          if(header.indexOf("/on" >= 0) && !state) {
            client.println("state: on");
            if(state=false) {
              Serial.println("turned on");
              digitalWrite(button, HIGH);
              delay(250);
              digitalWrite(button, LOW);
            }
            WakeOnLan::sendWOL(pcIp, UDP, mac, sizeof mac );    
            state = true;   
          }

          if(header.indexOf("/off" >= 0)) {
            //turn off pc
            client.println("state: off");
            if(state == true) {
              Serial.println("turned off");
              digitalWrite(button, HIGH);
              delay(250);
              digitalWrite(button, LOW);
              state = false;
            }
          }
        }
      }
    }
  }
}

String IpToString(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}
