#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include "motion.hpp"
#include <string> 
#include "io.hpp"

const char *ssid = "Brot";
const char *pw = "12345678";
const double volt_mul = 15.75;

extern char pa;
extern char pb;

uint32_t t_bat = 0;
double volts = 0;
AsyncWebServer  server(80); //Server on port 80

Motion motion;


void measureBatteryLevel(){
  if((t_bat + 2000) < millis()){
    t_bat = millis();
    int sensorValue = analogRead(A0); 
    volts = ((double)sensorValue / 1023) * volt_mul * 3.3;
  }
}

//===============================================================
//                  SETUP
//===============================================================
void setup(void){
  Serial.begin(115200);

  pinMode(A0,INPUT);
  setupI2C();
  setupDriver();
  setLamp(0,0);
  updateLamps();
  changeSpeed();
  changeDir();

  Serial.println("");
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAP(ssid, pw);  //Start HOTspot removing password will disable security

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);
 
  //server.on("/", handleRoot);      //Which routine to handle at root location

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/index.html", "text/html");
  });

  server.on("/getStatus", HTTP_GET, [](AsyncWebServerRequest *request){
    String speed = String(motion.speed);
    String direction = String(motion.direction);
    char copy[50];
    String full = "{\"speed\":" + speed + ",\"dir\":"+ direction + ",\"rev\":"+ String(motion.lastspeed) + ",\"volts\":"+ String(volts) + "}";
    full.toCharArray(copy,50);
    request->send(200,"text/json",copy);
  });

  server.on("/setSpeed", HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    String body = "";
    for (size_t i = 0; i < len; i++) {body += char(data[i]);}
    int speed = body.toInt();
    if (speed > 0 && speed < 11){
      motion.speed = speed;
      request->send(200);
    }else{
      request->send(400);
    }
    changeSpeed();
  });

   server.on("/setDir", HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    String body = "";
    for (size_t i = 0; i < len; i++) {body += char(data[i]);}
    int direction = body.toInt();
    if (direction >= 0 && direction < 3){
      motion.direction = direction;
      request->send(200);
    }else{
      request->send(400);
    }
    changeDir();
    
  });
  

  //static libs
  server.on("/src/bootstrap.min.css",HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/bootstrap.min.css", "text/css");
  });

  server.on("/src/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/bootstrap.min.js", "text/javascript");
  });

  server.on("/src/jquery-3.5.1.slim.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/jquery-3.5.1.slim.min.js", "text/javascript");
  });

  server.on("/src/popper.min.js", HTTP_GET,  [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/popper.min.js", "text/javascript");
  });

  server.on("/src/index.js", HTTP_GET,  [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/index.js", "text/javascript");
  });

  server.on("/src/logo.png", HTTP_GET,  [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/logo.png", "image/png");
  });

  server.begin();                  //Start server
  Serial.println("HTTP server started");


}

//===============================================================
//                     LOOP
//===============================================================
void loop(void){
  updateDriver();
  measureBatteryLevel();
  updateIO();

}

