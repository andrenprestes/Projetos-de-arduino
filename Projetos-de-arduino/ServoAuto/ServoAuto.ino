#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "NEVESPRESTES";
const char* password = "familiaprestes";
const IPAddress ip(192, 168, 0, 214);
const IPAddress dns(8,8,4,4);
const IPAddress gateway(192, 168, 0, 1);
const IPAddress subnet(255, 255, 255, 0);
ESP8266WebServer server ( 80 );

Servo servo_1;
Servo servo_2;
Servo servo_3;

const int buttonPin = D8; 
int buttonState = LOW;
int numPush = 0;

int hour1 = 0;
int hour2 = 0;
int hour3 = 0;

int min1 = 0;
int min2 = 0;
int min3 = 0;

int Back;
int backstate = 0;

int hour = 0;
int minute = 0;
int delayTime = 1; //time in seconds to wait before check

bool pote1 = true;
bool pote2 = true;
bool pote3 = true;

const int R = D5;
const int G = D6;
const int B = D7;

void handleRoot()
{
  if (server.hasArg("hora1")) {
    handleSubmit();
  }
  else {
    handleIndex();
  }
}



void handleSubmit()
{
  hour1 = server.arg("hora1").toInt();
  min1 = server.arg("minuto1").toInt();
  hour2 = server.arg("hora2").toInt();
  min2 = server.arg("minuto2").toInt();
  hour3 = server.arg("hora3").toInt();
  min3 = server.arg("minuto3").toInt();  
  Back = server.arg("back").toInt();  
  handleIndex();  
}

void handleIndex() {
  char temp[2000];
  snprintf (temp, 2000,
"<!DOCTYPE html>\
<html>\
<head>\
<style>\
 .button {\
  background-color: #4CAF50;\
  border: none;\
  color: white;\
  padding: 8px 16px;\
  text-align: center;\
  text-decoration: none;\
  display: inline-block;\
  font-size: 16px;\
  margin: 4px 2px;\
  transition-duration: 0.4s;\
  cursor: pointer;\
}\
.button5 {\
  background-color: #555555;\
  color: white;\
  border: 2px solid #555555;\
  border-radius: 4px;\
}\
.button5:hover {\
  background-color: white;\
  color: black;\
}\
h2{\
  padding-left: 4px\
}\
input[type=number]{\
  width: 60px;\
  border-radius: 4px;\
  border: none;\
}\
.box {\
  border-radius: 5px;\
  background-color: #999999;\
  padding: 20px;\
  width: 210px;\
}\
</style>\
</head>\
<body>\
  <h1>Horarios da racao</h1>\
  <div class='box'>\
    <form action='/' method='post'>\
    <h2>Primeiro horario :</h2>\
      <input type='number' name='hora1' value='%02d' min='0' max='23'> h :\
      <input type='number' name='minuto1' value='%02d' min='0' max='59'> min\
      <br><br>\
    <h2>Segundo horario :</h2>\
      <input type='number' name='hora2' value='%02d' min='0' max='23'> h :\
      <input type='number' name='minuto2' value='%02d' min='0' max='59'> min\
      <br><br>\   
    <h2>Terceiro horario :</h2>\
      <input type='number' name='hora3' value='%02d' min='0' max='23'> h :\
      <input type='number' name='minuto3' value='%02d' min='0' max='59'> min\
      <br><br>\
      <input class='button button5' type='submit' value='Set'>\
    </form>\
  </div>\
  <br><br>\
      <button class='button button5' name='back'>Back</button>\
</body>\
</html>", hour1, min1, hour2, min2, hour3, min3,pote1,pote2,pote3);
  server.send ( 200, "text/html", temp );
}

void ChangeLights(int red, int green, int blue) {
  digitalWrite(R, red);
  digitalWrite(G, green);
  digitalWrite(B, blue);
}

void VerifyHour() {
    if (minute == min1 && !pote1 && hour == hour1) {
    servo_1.write(180);
    pote1 = true;
    ChangeLights(LOW, HIGH, LOW);
  }
  else if (minute == min2 && !pote2 && hour == hour2) {
    servo_2.write(180);
    pote2 = true;
    ChangeLights(HIGH, HIGH, LOW);
  }
  else if (minute == min3 && !pote3 && hour == hour3) {
    servo_3.write(180);
    pote3 = true;
    ChangeLights(HIGH, LOW, LOW);
  }
}

int PushButton(int buttonState) {
  if (buttonState == HIGH) {
    numPush++;
  }
  return numPush;
}

void ButtonControl(int numPush) {
  if (numPush == 1) {
    servo_1.write(0);
    pote1 = false;
    ChangeLights(HIGH, HIGH, LOW);
  } else if (numPush == 2) {
    servo_2.write(0);
    pote1 = false;
    ChangeLights(LOW, HIGH, LOW);
  } else if (numPush == 3) {
    servo_3.write(0);
    pote1 = false;
    ChangeLights(LOW, LOW, HIGH);
    numPush = 0;
  }
}

void setup () {

  Serial.begin(115200);
  
  WiFi.mode ( WIFI_STA );  
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  
  servo_1.attach(D2);
  servo_2.attach(D3);
  servo_3.attach(D4);

  servo_1.write(180);
  servo_2.write(180);
  servo_3.write(180);

  pinMode(buttonPin, INPUT);

  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
    Serial.println("Connecting..");
  }

  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  server.on ( "/", handleRoot );
  server.begin();
  Serial.println ( "HTTP server started" );

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  ChangeLights(LOW, LOW, HIGH);
}

void loop() {
  
  server.handleClient();
  if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin("http://worldclockapi.com/api/json/utc/now");
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      hour = payload.substring(41, 43).toInt() - 2;
      minute = payload.substring(44, 46).toInt();
      if(hour < 0){
        hour += 24;
      }
      Serial.printf("%i\n", Back);
      Serial.printf("%i:%i\n", hour, minute);
      
      //buttonState = digitalRead(buttonPin);
      //botaoFunciona(buttonState);
      //PushButton(buttonState);
      //ButtonControl(numPush);
      
      VerifyHour();
    }
    http.end();
  }
  Serial.printf("%02d:%02d | %02d:%02d | %02d:%02d\n", hour1, min1, hour2, min2, hour3, min3);
  delay(delayTime * 1000);
}
