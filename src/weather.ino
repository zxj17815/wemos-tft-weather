#include <map>
#include <string>
#include <stdlib.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Arduino.h>
//#include <avr/pgmspace.h> //flash memory
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Setting.h>
#include <Fonts.h>
#include <Images.h>

// set up pins we are going to use to talk to the screen
#define TFT_DC D8   // register select (stands for Data Control perhaps!)
#define TFT_RST D2 // Display reset pin, you can also connect this to the ESP8266 reset
// in which case, set this #define pin to -1!
#define TFT_CS D3 // Display enable (Chip select), if not enabled will not talk on SPI bus

// initialise the routine to talk to this display with these pin connections (as we've missed off
// TFT_SCLK and TFT_MOSI the routine presumes we are using hardware SPI and internally uses 13 and 11
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// wifi & client
const uint8_t fingerprint[20] = {0x40, 0xaf, 0x00, 0x6b, 0xec, 0x90, 0x22, 0x41, 0x8e, 0xa3, 0xad, 0xfa, 0x1a, 0xe8, 0x25, 0x41, 0x1d, 0x1a, 0x54, 0xb3};
ESP8266WiFiMulti WiFiMulti;

// weather
const char *weather;
const char *humidity;
const char *temperature;
const char *windpower;
const char *wind;

float p = 3.1415926;

int time_get_data = 1;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello! ST77xx TFT Test");
  tft.initR(INITR_144GREENTAB); // initialize a ST7735S chip, black tab
  Serial.println("Initialized");

  // large block of text
  tft.fillScreen(ST77XX_BLACK);
  testdrawtext("1234567890abcdefghijklmopqrstuvwxyz!@#$%^&*()", ST77XX_WHITE);
  delay(500);

  // tft print function!
  tftPrintTest();
  Serial.println("Printed");
  delay(500);

  // a single pixel
  tft.drawPixel(tft.width() / 2, tft.height() / 2, ST77XX_GREEN);
  delay(500);

  // line draw test
  testlines(ST77XX_YELLOW);
  delay(500);

  // optimized lines
  testfastlines(ST77XX_RED, ST77XX_BLUE);
  delay(500);

  testdrawrects(ST77XX_GREEN);
  delay(500);

  testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
  delay(500);

  tft.fillScreen(ST77XX_BLACK);
  testfillcircles(10, ST77XX_BLUE);
  testdrawcircles(10, ST77XX_WHITE);
  delay(500);

  testroundrects();
  delay(500);

  testtriangles();
  delay(500);

  Serial.println("done");
  delay(500);

  // try connect to wifi
  wifiCoon();
  delay(500);

  // get weather data
  while (time_get_data != 600)
  {
    getData();
  }
  delay(500);
}

void loop()
{
  if (time_get_data == 0)
  {
    getData();
  }
  else
  {
    time_get_data--;
  }
  spaceman();
}

#pragma region 开机测试

void testlines(uint16_t color)
{
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6)
  {
    tft.drawLine(0, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6)
  {
    tft.drawLine(0, 0, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6)
  {
    tft.drawLine(tft.width() - 1, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6)
  {
    tft.drawLine(tft.width() - 1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6)
  {
    tft.drawLine(0, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6)
  {
    tft.drawLine(0, tft.height() - 1, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6)
  {
    tft.drawLine(tft.width() - 1, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6)
  {
    tft.drawLine(tft.width() - 1, tft.height() - 1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color)
{
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.setTextSize(1);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2)
{
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y = 0; y < tft.height(); y += 5)
  {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x = 0; x < tft.width(); x += 5)
  {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color)
{
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6)
  {
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2)
{
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = tft.width() - 1; x > 6; x -= 6)
  {
    tft.fillRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color1);
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color)
{
  for (int16_t x = radius; x < tft.width(); x += radius * 2)
  {
    for (int16_t y = radius; y < tft.height(); y += radius * 2)
    {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color)
{
  for (int16_t x = 0; x < tft.width() + radius; x += radius * 2)
  {
    for (int16_t y = 0; y < tft.height() + radius; y += radius * 2)
    {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles()
{
  tft.fillScreen(ST77XX_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width() / 2;
  int x = tft.height() - 1;
  int y = 0;
  int z = tft.width();
  for (t = 0; t <= 15; t++)
  {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x -= 4;
    y += 4;
    z -= 4;
    color += 100;
  }
}

void testroundrects()
{
  tft.fillScreen(ST77XX_BLACK);
  int color = 100;
  int i;
  int t;
  for (t = 0; t <= 4; t += 1)
  {
    int x = 0;
    int y = 0;
    int w = tft.width() - 2;
    int h = tft.height() - 2;
    for (i = 0; i <= 16; i += 1)
    {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x += 2;
      y += 3;
      w -= 4;
      h -= 6;
      color += 1100;
    }
    color += 100;
  }
}
#pragma endregion

void tftPrintTest()
{
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 20);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Are you ok?");
  delay(1000);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Are you ok?");
  delay(2000);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello!");
  delay(1600);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Thank you!");
  tft.setTextColor(ST77XX_BLUE);
  delay(1000);
  tft.setTextSize(2);
  tft.println("Thank you");
  tft.println("very much!");
  delay(1000);
}

// connect to the WiFi network
void wifiCoon()
{
  tft.fillScreen(ST77XX_BLACK);
  Serial.println("Connecting to WiFi...");
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setTextWrap(true);
  tft.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("502", "87034039");
  WiFiMulti.addAP("HXJT-WIFI", "12345678@");
  WiFiMulti.addAP("iPhone 13", "12345678");
  WiFiMulti.addAP("401", "87034039");
  int try_count = 1;
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    delay(2000);
    tft.setCursor(0, 50);
    tft.print("try again...");
    tft.print(try_count);
    tft.print(" times");
    try_count++;
  }
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.print("Connected to WiFi!");
  delay(1000);
}

// shou the image on the screen
void showBitmap(int x, int y, const uint8_t *bitmap, int w, int h, uint16_t color)
{
  tft.drawBitmap(x, y, bitmap, w, h, color);
}

// client to get data to the server
void getData()
{
  if (WiFiMulti.run() == WL_CONNECTED)
  {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    client->setInsecure();

    HTTPClient https;
    if (https.begin(*client, API_URL))
    { // HTTPS
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          String payload = https.getString();
          Serial.println(payload);
          // deserialize the JSON data
          DynamicJsonDocument doc(1024);
          DeserializationError error = deserializeJson(doc, payload);
          if (error)
          {
            Serial.print(F("deserializeJson() failed: "));
            // testdrawtext("deserializeJson() failed: ", ST77XX_WHITE);
            Serial.println(error.f_str());
            // testdrawtext(error.f_str(), ST77XX_WHITE);
          }
          weather = doc["lives"][0]["weather"];
          humidity = doc["lives"][0]["humidity"];
          temperature = doc["lives"][0]["temperature"];
          windpower = doc["lives"][0]["windpower"];
          wind = doc["lives"][0]["winddirection"];
          Serial.println(weather);
          Serial.println(humidity);
          Serial.println(temperature);
          Serial.println(windpower);
          Serial.println(wind);

          tft.fillScreen(ST77XX_WHITE);
          // show the weather bitmap
          tft.drawRGBBitmap(4, 4, weather_bitmap_allArray[weather_bitmap_map.at(weather)], 24, 24);
          // show the weather with chinese
          int char_index = 0;
          for (int i = 0; weather[i] != 0; ++i)
          {
            char temp_ch[3] = {weather[i], weather[i + 1], weather[i + 2]};
            Serial.println(temp_ch);
            showBitmap(30 + char_index * 18, 8, text_font_allArray[textFontMap.at(temp_ch)], 16, 16, ST77XX_BLUE);
            char_index++;
            i += 2;
          }

          // show the temperature
          if (atoi(temperature) < 17)
          {
            tft.drawRGBBitmap(4, 34, bitmap_temperature_blue, 24, 24);
          }
          else if (atoi(temperature) > 24)
          {
            tft.drawRGBBitmap(4, 34, bitmap_temperature_red, 24, 24);
          }
          else
          {
            tft.drawRGBBitmap(4, 34, bitmap_temperature_green, 24, 24);
          }
          tft.setTextColor(ST77XX_BLUE);
          tft.setCursor(30, 38);
          tft.setTextSize(2);
          tft.println(temperature);
          showBitmap(55, 38, bitmap_temperature_celsius, 16, 16, ST77XX_BLUE);

          // show the humidity
          tft.drawRGBBitmap(4, 64, bitmap_humidity, 24, 24);
          char_index = 0;
          showBitmap(30 + char_index * 18, 68, text_font_allArray[textFontMap.at("湿")], 16, 16, ST77XX_BLUE);
          char_index++;
          showBitmap(30 + char_index * 18, 68, text_font_allArray[textFontMap.at("度")], 16, 16, ST77XX_BLUE);
          char_index++;
          tft.setCursor(30 + char_index * 18, 69);
          tft.print(humidity);
          tft.print("%");

          // show the wind with chinese
          char_index = 0;
          for (int i = 0; wind[i] != 0; ++i)
          {
            char temp_ch[3] = {wind[i], wind[i + 1], wind[i + 2]};
            Serial.println(temp_ch);
            showBitmap(10 + char_index * 18, 98, text_font_allArray[textFontMap.at(temp_ch)], 16, 16, ST77XX_BLUE);
            char_index++;
            i += 2;
          }
          showBitmap(10 + char_index * 18, 98, text_font_allArray[textFontMap.at("风")], 16, 16, ST77XX_BLUE);
          char_index++;

          // show the wind power
          tft.setTextColor(ST77XX_BLUE);
          tft.setTextSize(2);
          if (strcmp(windpower, "≤3") == 0)
          {
            showBitmap(10 + char_index * 18, 98, text_font_allArray[textFontMap.at("≤")], 16, 16, ST77XX_BLUE);
            char_index++;
            tft.setCursor(10 + char_index * 18, 98);
            tft.print("3");
            char_index += 2;
          }
          else
          {
            tft.setCursor(10 + char_index * 18, 99);
            tft.println(windpower);
            char_index = char_index + strlen(windpower);
          }
          showBitmap(10 + char_index * 16, 98, text_font_allArray[textFontMap.at("级")], 16, 16, ST77XX_BLUE);
        }
        time_get_data = 600;
      }
      else
      {
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // textdrawtext("[HTTPS] GET... code: ", ST77XX_WHITE);
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        // textdrawtext(https.errorToString(httpCode).c_str(), ST77XX_WHITE);
      }
      https.end();
    }
    else
    {
      Serial.printf("[HTTPS] Unable to connect\n");
      testdrawtext("[HTTPS] Unable to connect", ST77XX_BLUE);
    }
  }
}

// spaceman gif
void spaceman()
{
  for (int i = 0; i < 16; i++)
  {
    GFXcanvas16 canvas_spaceman(72, 76);
    canvas_spaceman.drawRGBBitmap(0, 0, bitmap_spaceman_allArray[i], 72, 76);
    tft.drawRGBBitmap(70, -10, canvas_spaceman.getBuffer(), 72, 76);
    delay(100);
  }
}
