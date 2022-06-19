#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"
#include <Adafruit_I2CDevice.h>
#define WIFI_SSID "hcdemo"
#define WIFI_PASSWORD "hc-demodemo"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define QUOTE_URL "https://zenquotes.io/api/random"

#define uS_TO_S_FACTOR 1000000UL  //Conversion Factor to MS
#define TIME_TO_SLEEP 86400 //in Seconds

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
tm timeinfo;

WiFiClientSecure client;

void connect_wifi() {
   Serial.println("Connecting to WiFi...");
   Serial.println("SSID: ");
   Serial.print(WIFI_SSID);
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
    digitalWrite (LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite (LED_BUILTIN, LOW);
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  
}

void getTime(){
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
}

String get_quotes(){
  HTTPClient http;
  http.begin(QUOTE_URL);
  int httpCode = http.GET();
  String payload;
  if(httpCode > 0){
    payload = http.getString();
  //  Serial.println(httpCode);
  //  Serial.println(quote);
  }else{
    Serial.println("Error on HTTP Request");
  }
  http.end();
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);
  //serializeJson(doc, Serial);
  const char* q = doc[0]["q"];
  const char* a = doc[0]["a"];
  return "\"" + String(q) + "\""+ " --- " + String(a);
}
void init_display(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
 display.display();
  delay(2000);
  display.clearDisplay();
}
void print2display(String txt, int percentage){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(1,1);
  display.println(txt);
  display.drawRect(2,40, 108,15, SSD1306_WHITE);
  display.fillRect(6, 44, percentage, 7, SSD1306_WHITE);
  display.setCursor(110, 43);
  display.println(String(percentage) + "%");
  display.display();
  delay(200);
}
void setup(){
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  init_display();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(1,1);
  display.println("Connecting to Wifi: \n"  WIFI_SSID);
    display.display();
  connect_wifi();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  digitalWrite (LED_BUILTIN, HIGH);	// turn on the LED
    String cur_quote = get_quotes();
     getTime();
    int doy = timeinfo.tm_yday;
    int percentage = (doy / 365.0) * 100;
    Serial.println(cur_quote);
    print2display(cur_quote, percentage);

    Serial.println("Progress of year : " + String(percentage));
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP / 60) + " Minutes");
    Serial.flush();
    esp_deep_sleep_start();
    
}