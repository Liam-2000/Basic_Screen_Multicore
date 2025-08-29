//SSID & PW
#include "arduino_secrets.h"

#include <Adafruit_BMP280.h>
#include <NTPClient.h>
#include <Wire.h>
#include <rgb_lcd.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <time.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

rgb_lcd lcd;

IPAddress ip(192, 168, 35, 100);

const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
const char* weatherURL = URL;

float temp;
int weatherCode;

const int clockButton = 6;
const int tempButton = 7;
const int weatherButton = 8;
int button = 1;

WiFiUDP ntpUDP;
HTTPClient http;
Adafruit_BMP280 bmp;

NTPClient timeClient(ntpUDP, "0.be.pool.ntp.org", 7200);

byte degree[8] = {
      0b11100,
      0b10100,
      0b11100,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000
    };

void setup() {
  Serial.begin(115200);
  initPins();
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.begin(16, 2);
  lcd.clear();
  connectWiFi();
  bmp.begin();
  timeClient.begin();
  timeClient.update();
  http.begin(weatherURL);
}

void loop() {
  getWeather();
  delay(10000);
}

void setup1(){
}

void loop1(){
  lcd.createChar(0, degree);
  setButtons();
  buttonActions(button);
}

void connectWiFi(){
  WiFi.config(ip);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){
    lcd.print("Connecting...");
    Serial.println("Connecting...");
  }
  lcd.setCursor(0, 0);
  Serial.println("Network Connected");
  lcd.print("Connected!");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(1500);
  lcd.clear();
}

void initPins(){
  pinMode(clockButton, INPUT_PULLUP);
  pinMode(tempButton, INPUT_PULLUP);
  pinMode(weatherButton, INPUT_PULLUP);
}

void setButtons(){
  //Clock
  // Debug // Serial.println(digitalRead(clockButton));
  if (digitalRead(clockButton) == LOW){
    button = 1;
  }
  //temperature
  // Debug // Serial.println(digitalRead(tempButton));
  if (digitalRead(tempButton) == LOW){
    button = 2;
  }
  //Weather Forecast
  // Debug // Serial.println(digitalRead(weatherButton));
  if (digitalRead(weatherButton) == LOW){
    button = 3;
  }
}

void buttonActions(int button){
  if (button == 1){
    lcd.clear();
    timeClient.update();
    lcd.print(timeClient.getFormattedTime());
    lcd.setCursor(0, 1);
    lcd.print(dayIntToString(timeClient.getDay()));
    delay(250);
  } 
  if (button == 2) {
    lcd.clear();
    lcd.print(bmp.readTemperature());
    lcd.write((unsigned char)0);
    lcd.print("C");
    delay(250);
  }
  if (button == 3) {
    lcd.clear();
    lcd.print("Temp: ");
    lcd.print(temp, 1);
    lcd.setCursor(0, 1);
    lcd.print(mapWeatherCode(weatherCode));
    delay(250);
  }
}

const char* dayIntToString(int day) {
  switch (day) {
    case 1: return "Monday";
    case 2: return "Tuesday";
    case 3: return "Wednesday";
    case 4: return "Thursday";
    case 5: return "Friday";
    case 6: return "Saturday";
    case 0: return "Sunday";
    default: return "Invalid";
  }
}

void getWeather(){
  http.begin(weatherURL);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      temp = doc["current"]["temperature_2m"];
      weatherCode = doc["current"]["weathercode"];
    } else {
      Serial.println("JSON Error");
    } 
  } else {
    Serial.print("HTTP Error: ");
    Serial.println(httpCode);
  }
  http.end();
}


String mapWeatherCode(int weatherCode) {
  switch (weatherCode) {
    case 0: return "Clear";
    case 1:
    case 2: return "Partly Cloudy";
    case 3: return "Cloudy";
    case 45:
    case 48: return "Fog";
    case 51:
    case 53:
    case 55: return "Drizzle";
    case 61:
    case 63:
    case 65: return "Rain";
    case 66:
    case 67: return "Freezing Rain";
    case 71:
    case 73:
    case 75: return "Snow";
    case 95: return "Thunderstorm";
    case 96:
    case 99: return "Thunder And Hail";
    default: return "Unknown";
  }
}