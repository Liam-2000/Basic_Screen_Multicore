#include <Adafruit_BMP280.h>
#include <NTPClient.h>
#include <Wire.h>
#include <rgb_lcd.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <time.h>

rgb_lcd lcd;

IPAddress ip(192, 168, 35, 100);

const char ssid[] = "SBO_GUEST";
const char pass[] = "MyGuests951!";

const int clockButton = 6;
const int tempButton = 7;
int button = 1;

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "0.be.pool.ntp.org", 7200);

Adafruit_BMP280 bmp;

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
}

void loop() {
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