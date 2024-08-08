#define BLYNK_TEMPLATE_ID "TMPL6edrAQpCC"
#define BLYNK_TEMPLATE_NAME "SmartClass Climate"
#define BLYNK_AUTH_TOKEN "NBwHqCHF34_NuwCmfPP6ykpm4HNoSGy-"

// #define BLYNK_TEMPLATE_ID "##################"
// #define BLYNK_TEMPLATE_NAME "SmartClass Climate"
// #define BLYNK_AUTH_TOKEN "####################################"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


char ssid[] = "SLT_Fiber_Optic";
char pass[] = "Life1Mal7i";

ESP8266WebServer server(80);

const long utcOffsetInSeconds = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64  
#define OLED_RESET    -1  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define AC_Blower D3
#define Lights D5
#define Humidifier D6
#define Buzzer D8
#define TRIG_PIN D0 
#define ECHO_PIN D7 

float temperature = 0;
float humidity = 0;
int motion = 0;
int airQuality = 0;
int ldrSignal = 0;
int Hours = 0;
int MiN = 0;
int sec = 0;
int tempMin = 0; 
int tempMax = 0; 
int speedMin = 0;   
int speedMax = 0; 
String lightsStatus = "...";
int airQualityPercentage = 0;
int screenCount = 0;
int blowerSpeedPercentage = 0;
String lightPower = "...";
String HumidifierStatus = "...";
boolean ManualMode = false;
boolean Power = true;

void handleUpdate() {
  if (server.hasArg("temperature") && server.hasArg("humidity") && 
      server.hasArg("motion") && server.hasArg("airQuality") && server.hasArg("ldrSignal")) {
    temperature = server.arg("temperature").toFloat();
    humidity = server.arg("humidity").toFloat();
    motion = server.arg("motion").toInt();
    airQuality = server.arg("airQuality").toInt();
    ldrSignal = server.arg("ldrSignal").toInt();
    
    airQualityPercentage = map(airQuality, 0, 1023, 100, 0);

    Blynk.virtualWrite(V0, temperature); 
    Blynk.virtualWrite(V1, humidity);    
    Blynk.virtualWrite(V2, motion);    
    Blynk.virtualWrite(V3, airQualityPercentage);    
    Blynk.virtualWrite(V4, ldrSignal);    
    
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

void setup() {
  Serial.begin(115200);

  server.on("/update", handleUpdate); 
  
  server.begin();
  timeClient.begin();
  timeClient.update();

   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) ; 
  }

  display.clearDisplay();
  display.setTextSize(2);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(35, 0);     
  display.println(F("Smart"));
  display.setCursor(24, 20);
  display.println(F("Climate"));
  display.setCursor(24, 40);
  display.println(F("Control"));
  display.display();
  
  
  display.invertDisplay(true); 
  delay(1000); 
  display.invertDisplay(false); 
  delay(2000);

  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);     
  display.println(F("Smart Climate Control"));
  display.drawLine(0, SCREEN_HEIGHT / 5, SCREEN_WIDTH, SCREEN_HEIGHT / 5, SSD1306_WHITE);
  display.setCursor(35, 20);
  display.println("Loading...");
 
  for (int i = 0; i < 122; i++) {
    display.setCursor(i, 40);
    display.println("I");
    display.display(); 
    delay(10);
  }

  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);     
  display.println(F("Smart Climate Control"));
  display.drawLine(0, SCREEN_HEIGHT / 5, SCREEN_WIDTH, SCREEN_HEIGHT / 5, SSD1306_WHITE);
  display.setCursor(0, 30);
  display.println(F("Connecting to WiFi..."));
  display.display();

  WiFi.begin(ssid, pass);

  int wifiTimeout = 10; 
  while (WiFi.status() != WL_CONNECTED && wifiTimeout > 0) {
    delay(1000);
    wifiTimeout--;
    Serial.print(".");
  }
  
 

  if (WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
     display.setTextSize(1);      
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);     
    display.println(F("Smart Climate Control"));
    display.drawLine(0, SCREEN_HEIGHT / 5, SCREEN_WIDTH, SCREEN_HEIGHT / 5, SSD1306_WHITE);
    display.setCursor(0, 30);
    display.println(F("Failed to connect to WiFi"));
    display.display();
    Serial.println(F("Failed to connect to WiFi"));
    for (;;) ; 
  } 
  
  else {
    display.clearDisplay();
    display.setTextSize(1);      
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);     
    display.println(F("Smart Climate Control"));
    display.drawLine(0, SCREEN_HEIGHT / 5, SCREEN_WIDTH, SCREEN_HEIGHT / 5, SSD1306_WHITE);
    display.setCursor(5, 30);
    display.println(F("Connected to WiFi"));
    display.display();
    Serial.println(F("Connected to WiFi"));
    delay(1000);
  }

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);


  pinMode(D0, OUTPUT);
  pinMode(AC_Blower, OUTPUT);
  pinMode(Lights, OUTPUT);
  pinMode(Humidifier, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(AC_Blower, HIGH);
  delay(300);
  digitalWrite(AC_Blower, LOW);
  delay(100);
  digitalWrite(Lights, HIGH);
  delay(300);
  digitalWrite(Lights, LOW);
  delay(100);
  digitalWrite(Humidifier, HIGH);
  delay(300);
  digitalWrite(Humidifier, LOW);

  delay(500);
  display.clearDisplay();
  display.display();
}

void loop() { //------------------------------------------------------------------------------- loop start -----------------------------------------
  Blynk.run();
  server.handleClient();
  timeClient.update();
  
  digitalWrite(Buzzer, LOW); // --------------------- test
  Serial.println(temperature);

  Hours = timeClient.getHours();
  MiN = timeClient.getMinutes();
  sec = timeClient.getSeconds();

  if(!ManualMode){

  if(Hours > 7 && Hours < 18){ // day time
  ScreenSwitch();
  if(motion){ // daytime someone in the class
//------------------------------------------------------ for temp ac blower control -----------------------------------------------
  tempMin = 20; 
  tempMax = 30; 
  speedMin = 0;   
  speedMax = 255; 


  int blowerSpeed = mapTemperatureToBlowerSpeed(temperature, tempMin, tempMax, speedMin, speedMax);
  blowerSpeed = constrain(blowerSpeed, speedMin, speedMax);
  blowerSpeedPercentage = map(blowerSpeed, 0, 255, 0, 100);
  analogWrite(AC_Blower, blowerSpeed);
  Blynk.virtualWrite(V5, blowerSpeedPercentage); 

// ------------------------------------------------------ lights control ---------------------------------------------------------
  digitalWrite(Lights, HIGH);
  lightPower = "ON";
  if (ldrSignal){
  digitalWrite(Buzzer, LOW);
  }

  else {
  digitalWrite(Buzzer, HIGH);
  Blynk.logEvent("chkLights");
  }

//-------------------------------------------------------- Humidifier control ----------------------------------------------------
  
  if (humidity < 50){
    digitalWrite(Humidifier, HIGH);
    HumidifierStatus = "ON";
    Blynk.virtualWrite(V6, 1); 
  }

  }

  else{ // daytime but no one in the class 
  digitalWrite(AC_Blower, LOW);
  digitalWrite(Lights, LOW);
  digitalWrite(Humidifier, LOW);
  HumidifierStatus = "OFF";
  Blynk.virtualWrite(V6, 0); 
  lightPower = "OFF";
  }

  }

  else{ // night time
  digitalWrite(AC_Blower, LOW);
  digitalWrite(Humidifier, LOW);
  HumidifierStatus = "OFF";
  Blynk.virtualWrite(V6, 0); 
  showScreen4();

  if(motion){ // someone in the class
    digitalWrite(Lights, HIGH);
    lightPower = "ON";

      if (ldrSignal){
  digitalWrite(Buzzer, LOW);
  }

  else {
  digitalWrite(Buzzer, HIGH);
  Blynk.logEvent("chkLights");
  }

    Blynk.logEvent("NightMotion");
    digitalWrite(Buzzer, HIGH);
  }

  else{
    digitalWrite(Lights, LOW);
    lightPower = "OFF";
    digitalWrite(Buzzer, LOW);
  }

  
  }
  }

  else { // when manual mode is on
  showScreen3();
  }


} //////////////////////////////////////////////////////// loop end ////////////

int mapTemperatureToBlowerSpeed(int temp, int tempMin, int tempMax, int speedMin, int speedMax) {
  return map(temp, tempMin, tempMax, speedMin, speedMax);
}

BLYNK_WRITE(V7) {
  int blowerSpeed = param.asInt();

  if (blowerSpeed > 1){
    ManualMode = true;
  }

  else{
    ManualMode = false;
  }
  
  blowerSpeedPercentage = map(blowerSpeed, 0, 255, 0, 100);
  Blynk.virtualWrite(V5, blowerSpeedPercentage); 
  analogWrite(AC_Blower, blowerSpeed);
}

BLYNK_WRITE(V8) {
  int SetLights = param.asInt();

  if (SetLights > 1){
    ManualMode = true;
  }

  else{
    ManualMode = false;
  }
  
  if(SetLights){
  Blynk.virtualWrite(V4, 1); 
  digitalWrite(Lights, HIGH);
  }

  else{
  Blynk.virtualWrite(V4, 0); 
  digitalWrite(Lights, LOW);
  }

}

BLYNK_WRITE(V9) {
  int SetPower = param.asInt();
  if (SetPower){
    Power = false;
  }
  else{
    Power = true;
  }
}



void ScreenSwitch(){
screenCount++;
if(screenCount < 30){
  showScreen1();
}

else if (screenCount > 30 && screenCount < 60){
  showScreen2();
}

else if (screenCount > 60){
  showScreen3();
  if(screenCount == 90){
    screenCount = 0;
  }
}
}

void showScreen4(){
   char timeString[9];
  sprintf(timeString, "%02d:%02d:%02d", Hours, MiN, sec);

  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);     
  display.println(F("Smart Climate Control"));
  display.drawLine(0, SCREEN_HEIGHT / 2.5, SCREEN_WIDTH, SCREEN_HEIGHT / 2.5, SSD1306_WHITE);

  display.setCursor(40, 15);
  display.println(timeString);

  display.setCursor(0, 30);
  display.print("Night Mode Active!");

}

void showScreen3(){
  char timeString[9];
  sprintf(timeString, "%02d:%02d:%02d", Hours, MiN, sec);

  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);     
  display.println(F("Smart Climate Control"));
  display.drawLine(0, SCREEN_HEIGHT / 2.5, SCREEN_WIDTH, SCREEN_HEIGHT / 2.5, SSD1306_WHITE);

  display.setCursor(40, 15);
  display.println(timeString);

  display.setCursor(0, 28);
  display.print("A/C Blower: ");
  display.print(blowerSpeedPercentage);
  display.print("%");

  display.setCursor(0, 37);
  display.print("Humidifier: ");
  display.print(HumidifierStatus);

  display.setCursor(0, 46);
  display.print("Lights Power: ");
  display.print(lightPower);

  display.display();

}

void showScreen2(){
  char timeString[9];
  sprintf(timeString, "%02d:%02d:%02d", Hours, MiN, sec);
  long duration, distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
 
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;

  int maxDistance = 100; 
  if(distance > maxDistance) {
    distance = maxDistance;
  }

  int barLength = map(distance, 0, maxDistance, 0, SCREEN_WIDTH);

  display.clearDisplay();

  display.fillRect(0, 45, barLength, 10, SSD1306_WHITE);

  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);     
  display.println(F("Smart Climate Control"));
  display.drawLine(0, SCREEN_HEIGHT / 2.5, SCREEN_WIDTH, SCREEN_HEIGHT / 2.5, SSD1306_WHITE);

  display.setCursor(40, 15);
  display.println(timeString);

  display.setCursor(0, 28);
  display.print("Water Level: ");
  display.print(distance);
  display.print("%");

  display.display();

  if (distance < 25){
    Blynk.logEvent("lowWater");
  }

}

void showScreen1(){
  char timeString[9];
  sprintf(timeString, "%02d:%02d:%02d", Hours, MiN, sec);

  if (ldrSignal){
    lightsStatus = "ON";
  }
  else{
    lightsStatus = "OFF";
  }

  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);     
  display.println(F("Smart Climate Control"));
  display.drawLine(0, SCREEN_HEIGHT / 2.5, SCREEN_WIDTH, SCREEN_HEIGHT / 2.5, SSD1306_WHITE);

  display.setCursor(40, 15);
  display.println(timeString);

  display.setCursor(0, 28);
  display.print("Temperature: ");
  display.print(temperature);
  display.print("C");

  display.setCursor(0, 37);
  display.print("Humidity: ");
  display.print(humidity);
  display.print("%");

  display.setCursor(0, 46);
  display.print("Air Quality: ");
  display.print(airQualityPercentage);
  display.print("%");

  display.setCursor(0, 55);
  display.print("Lights: ");
  display.print(lightsStatus);

  display.display();

  if (airQualityPercentage < 30){
    Blynk.logEvent("LowAirQuality");
    digitalWrite(Buzzer, HIGH);
  }
  else{
    digitalWrite(Buzzer, LOW);
  }

  if (humidity < 40){
  Blynk.logEvent("LowHumidity");
  digitalWrite(Buzzer, HIGH);
  }
  else{
    digitalWrite(Buzzer, LOW);
  }

  if (temperature > 29){
  Blynk.logEvent("Lowtemp");
  digitalWrite(Buzzer, HIGH);
  }
  else{
    digitalWrite(Buzzer, LOW);
  }
}

