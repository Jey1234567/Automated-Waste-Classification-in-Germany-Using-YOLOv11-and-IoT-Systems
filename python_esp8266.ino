#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include "Servo.h"

SoftwareSerial mySerial(D5, D6); // RX, TX  -> ESP8266 D5 to DF TX, D6 to DF RX
DFRobotDFPlayerMini myDFPlayer;

const char* ssid = "WIFI_Name";
const char* pass = "WIFI_Password";

ESP8266WebServer server(80);

const int servo_pin01 = D1;
const int servo_pin02 = D2;
const int servo_pin03 = D3;
const int servo_pin04 = D4;

Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;
int angle =0;
String msg="";
void handleMessage() {
  if (server.hasArg("plain")) {
    msg = server.arg("plain");
    Serial.print("Received: ");
    Serial.println(msg);
    
    server.send(200, "text/plain", "Message received!");
  }
  else {
    
    server.send(400, "text/plain", "No message body");
  }
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);

  
  myservo1.attach(servo_pin01);
  myservo2.attach(servo_pin02);
  myservo3.attach(servo_pin03);
  myservo4.attach(servo_pin04);

  // set all servos to 0° at startup
  myservo1.write(0);
  myservo2.write(0);
  myservo3.write(0);
  myservo4.write(0);
  
  Serial.println("Initializing DFPlayer Mini...");

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("Unable to begin:");
    Serial.println("1. Check connections");
    Serial.println("2. Insert SD card with MP3 files");
    while(true);
  }
  Serial.println("DFPlayer Mini online.");
  myDFPlayer.volume(25);  // Set volume (0–30)
  
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  myDFPlayer.play(3);

  delay(3000);
  server.on("/send", HTTP_POST, handleMessage);
  server.begin();
  Serial.println("HTTP server started");
 
}

void loop() {
  server.handleClient();
  
  if(msg=="gelbersack"){
    msg="";
      myDFPlayer.play(1);
      for (int angle = 0; angle <= 160; angle += 5) {
            
            myservo1.write(angle);
            delay(15);
          }
      delay(5000);
          for (int angle = 160; angle >= 0; angle -= 5) {
            myservo1.write(angle);
            delay(15);
          }

    }
    else if(msg=="restmull"){
      msg="";
      myDFPlayer.play(2);
      for (int angle = 0; angle <= 160; angle += 5) {
            
            myservo2.write(angle);
            delay(15);
          }
      
      delay(5000);
          for (int angle = 160; angle >= 0; angle -= 5) {
            myservo2.write(angle);
            delay(15);
          }
    }
    
  else if(msg=="papier"){
    msg="";
    myDFPlayer.play(4);
      for (int angle = 0; angle <= 160; angle += 5) {
            myservo3.write(angle);
            delay(15);
          }
      
      delay(5000);
          for (int angle = 160; angle >= 0; angle -= 5) {
            myservo3.write(angle);
            delay(15);
          }
    }
    
  
  else if(msg=="bio"){
    msg="";
    myDFPlayer.play(5);
      for (int angle = 0; angle <= 160; angle += 5) {
            
            myservo4.write(angle);
            delay(15);
          }
      
      delay(5000);
          for (int angle = 160; angle >= 0; angle -= 5) {
            myservo4.write(angle);
            delay(15);
          }
    }
    
}
