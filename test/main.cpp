#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <HTTPClient.h>

#define WIFI_SSID "ragnaroek"
#define WIFI_PASSWORD "BtpzB8L6"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define STUDIWERK_URL "studiwerk.de"
#define STUDIWERK_BASE "/export/speiseplan.xml"
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
HTTPClient client_http;

void connect_wifi()
{
  Serial.println("Connecting to WiFi...");
  Serial.println("SSID: ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
  }
  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}

void init_display()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(500);
  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(500);
}
void get_speisen()
{
  // client.setCertificate(test_client_cert); // for client verification
  // client.setPrivateKey(test_client_key);	// for client verification

  client_http.begin("https://studiwerk.de/export/speiseplan.xml");
  int code = client_http.GET();
  Serial.println(code);
  if (code == 200)
  {

    // get tcp stream
    WiFiClient *stream = client_http.getStreamPtr();
    //  xml = stream->readStringUntil('\0');
    // read all data from server
    // get length of document (is -1 when Server sends no Content-Length header)
    int len = client_http.getSize();
    // create buffer for read
    uint8_t buff[128] = {0};
    while (client_http.connected() && (len > 0 || len == -1))
    {
      // get available data size
      size_t size = stream->available();

      if (size)
      {
        // read up to 128 byte
        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        // final += c;
        //  write it to Serial
        file.write(buff, c);
        Serial.println(c);

        if (len > 0)
        {
          len -= c;
        }
      }
      delay(1);
    }

    Serial.print("[HTTP] connection closed or file end.\n");
  }
}
void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  // init_display();
  connect_wifi();
  get_speisen();
  Serial.println(xml);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH); // turn on the LED
}