#include <Arduino.h>
#include "FS.h"
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <Adafruit_PN532.h>
#include <TinyGPS++.h>
// #include <BlueToothSerial.h>

#define pin 19
TFT_eSPI tft = TFT_eSPI();

// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".
#define CALIBRATION_FILE "/TouchCalData"
// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

#define rx2 16
#define tx2 17

#define input1 35
#define input2 34
#define input3 39
#define input4 36
#define output1 26
#define output2 25
#define output3 15
#define output4 4
#define output5 5
#define output6 18
#define output7 19
#define output8 23
// #define output

#define BUTTON_W_XL 250
#define BUTTON_W_L 200
#define BUTTON_W_M 120
#define BUTTON_W_S 80
#define BUTTON_W_xS 20

#define BUTTON_H_XL 250
#define BUTTON_H_L 80
#define BUTTON_H_M 60
#define BUTTON_H_S 40
#define BUTTON_H_xS 20

// Define the Button class
class Button
{
public:
  // Member variables
  int x1;
  int y1;
  int x2;
  int y2;

  // Constructor to initialize the Button object
  Button(int xVal, int yVal, int widthVal, int heightVal)
  {
    x1 = xVal;
    y1 = yVal;
    x2 = x1 + widthVal;
    y2 = y1 + heightVal;
  }

  // Method to display the Button properties
  void display()
  {
    Serial.print("Button Position: (");
    Serial.print(x1);
    Serial.print(", ");
    Serial.print(y1);
    Serial.print("), right: ");
    Serial.print(x2);
    Serial.print(", down: ");
    Serial.println(y2);
  }
};

int16_t screen_width, screen_hight, screen_header = 40, screen_footer = 20;
uint32_t color;
String mainbuttonname[] = {"I/O", "Signal", "wifi", "bluetooth", "sub-G", "NFC", "GPS", "NRF24", "IR", "paint", "serial", "setting"};
size_t mainbuttonname_s = sizeof(mainbuttonname) / sizeof(mainbuttonname[0]); // Calculate the number of buttons
String settingbuttonname[] = {"recollibrate", "reset"};
size_t settingbuttonname_s = sizeof(settingbuttonname) / sizeof(settingbuttonname[0]);
String bluetoothbuttonname[] = {"btserial"};
size_t bluetoothbuttonname_s = sizeof(bluetoothbuttonname) / sizeof(bluetoothbuttonname[0]);
Button buttonlist[] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
size_t buttonlist_s = 0;
String lines[20];
int lineid[20];
size_t lines_s = 0;
uint16_t colors[] = {TFT_YELLOW, TFT_ORANGE, TFT_RED, TFT_GREEN, TFT_DARKGREEN, TFT_CYAN, TFT_BLUE, TFT_DARKGREY, TFT_WHITE};
int numColors = sizeof(colors) / sizeof(colors[0]); // Calculate the number of colors
bool touch;
bool first = false;
bool renderexitbutton;
int pageint;
int subpageint;
int timer;
TinyGPSPlus gps;
HardwareSerial neogps(2);
Adafruit_PN532 nfc(21, 22);
// BluetoothSerial serialbt;

void setup()
{
  Serial.begin(115200);
  tft.init();

  // check file system
  if (!SPIFFS.begin())
  {
    Serial.println("formatting file system");

    SPIFFS.format();
    SPIFFS.begin();
  }

  if (REPEAT_CAL)
  {
    // Delete if we want to re-calibrate
    SPIFFS.remove(CALIBRATION_FILE);
  }

  touch_calibrate();

  pageint = 0;
  first = true;

  tft.setRotation(0);
  screen_width = tft.width();
  screen_hight = tft.height() - screen_header - screen_footer;
  tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
  tft.drawRect(0, screen_header, screen_width, screen_hight, TFT_WHITE);
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // this code for background text colot
  tft.println("helllo");
}

void loop()
{
  uint16_t x, y;
  touch = tft.getTouch(&x, &y, 10);
  // if (touch)
  // {
  //   Serial.print(x);
  //   Serial.print("_");
  //   Serial.println(y);
  //   Serial.println(pageint);
  // }
  if (pageint != 0)
  {
    if (renderexitbutton)
    {
      tft.fillCircle(10, screen_header + 10, 7, TFT_RED);
      renderexitbutton = false;
    }
    if (x < 20 && y > screen_header && y < screen_header + 20)
    {
      if (subpageint == 0)
      {
        pageint = 0;
      }
      else
      {
        subpageint = 0;
      }
      first = true;
    }
  }
  if (pageint == 0)
  {
    page_main(x, y);
  }
  else if (pageint == 1)
  {
    page_i0(x, y);
  }
  else if (pageint == 2)
  {
    pageint = 0;
    first = false;
  }
  else if (pageint == 3)
  {
    page_wifi(x, y);
  }
  else if (pageint == 4)
  {
    if (subpageint == 0)
    {
      page_bluetooth(x, y);
    }
    else if (subpageint == 1)
    {
      page_bluetoothserial(x, y);
    }
  }
  else if (pageint == 5)
  {
    pageint = 0;
    first = false;
  }
  else if (pageint == 6)
  {
    page_NFC(x, y);
  }
  else if (pageint == 7)
  {
    page_GPS(x, y);
  }
  else if (pageint == 8)
  {
    pageint = 0;
    first = false;
  }
  else if (pageint == 9)
  {
    pageint = 0;
    first = false;
  }
  else if (pageint == 10)
  {
    page_paint(touch, x, y);
  }
  else if (pageint == 11)
  {
    page_serial(x, y);
  }
  else if (pageint == 12)
  {
    page_setting(x, y);
  }
}

void page_main(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
    // neogps.end();
    buttonplacer(mainbuttonname, mainbuttonname_s, false);
    first = false;
  }
  else
  {
    if (touch)
    {
      int button = buttonselect(x, y);
      if (button > 0)
      {
        pageint = button;
        first = true;
      }
    }
  }
}
void page_i0(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);

    renderexitbutton = true;
    for (int i = 0; i < 18; i++)
    {
      buttonlist[i] = {0, 0, 0, 0};
    }
    buttonlist_s = 0;
    int x = 64;
    int y = 40 + screen_header;
    for (int i = 0; i < 12; i++)
    {
      if (i < 4)
      {
        tft.drawCircle(x + (i * x), y, 20, TFT_WHITE);
      }
      else if (i < 8)
      {
        tft.setCursor(x + ((i - 4) * x) - 25, y + 30);
        tft.fillRect(x + ((i - 4) * x) - 20, y + 40, 40, 40, TFT_BLUE);
        tft.print("pin : ");
        buttonlist[i - 4] = {((i - 4) * x) - 20, y + 40, 40, 40};
        buttonlist_s += 1;
        if (i == 4)
        {
          tft.print(output1);
        }
        else if (i == 5)
        {
          tft.print(output2);
        }
        else if (i == 6)
        {
          tft.print(output3);
        }
        else if (i == 7)
        {
          tft.print(output4);
        }
      }
      else if (i < 12)
      {
        tft.setCursor(x + ((i - 8) * x) - 25, y + 90);
        tft.fillRect(x + ((i - 8) * x) - 20, y + 100, 40, 40, TFT_BLUE);
        buttonlist[i - 4] = {x + ((i - 8) * x) - 20, y + 100, 40, 40};
        buttonlist_s += 1;
        tft.print("pin : ");
        if (i == 8)
        {
          tft.print(output5);
        }
        else if (i == 9)
        {
          tft.print(output6);
        }
        else if (i == 10)
        {
          tft.print(output7);
        }
        else if (i == 11)
        {
          tft.print(output8);
        }
      }
    }

    pinMode(input1, INPUT_PULLDOWN);
    pinMode(input2, INPUT_PULLDOWN);
    pinMode(input3, INPUT_PULLDOWN);
    pinMode(input4, INPUT_PULLDOWN);
    pinMode(output1, OUTPUT);
    pinMode(output2, OUTPUT);
    pinMode(output3, OUTPUT);
    pinMode(output4, OUTPUT);
    pinMode(output5, OUTPUT);
    pinMode(output6, OUTPUT);
    pinMode(output7, OUTPUT);
    pinMode(output8, OUTPUT);
    int timer = millis();

    first = false;
  }
  else
  {
    if (touch)
    {
      int button = buttonselect(x, y);
      if (millis() >= timer)
      {
        timer = millis() + 400;
        if (button == 1)
        {
          if (!digitalRead(output1))
          {
            tft.fillRect(buttonlist[0].x1, buttonlist[0].y1, 40, 40, TFT_CYAN);
          }
          else
          {
            tft.fillRect(buttonlist[0].x1, buttonlist[0].y1, 40, 40, TFT_BLUE);
          }
          digitalWrite(output1, !digitalRead(output1));
        }
        if (button == 2)
        {
          if (!digitalRead(output2))
          {
            tft.fillRect(buttonlist[1].x1, buttonlist[0].y1, 40, 40, TFT_CYAN);
          }
          else
          {
            tft.fillRect(buttonlist[1].x1, buttonlist[0].y1, 40, 40, TFT_BLUE);
          }
          digitalWrite(output2, !digitalRead(output2));
        }
        if (button == 3)
        {
          if (!digitalRead(output3))
          {
            tft.fillRect(buttonlist[2].x1, buttonlist[2].y1, 40, 40, TFT_CYAN);
          }
          else
          {
            tft.fillRect(buttonlist[2].x1, buttonlist[2].y1, 40, 40, TFT_BLUE);
          }
          digitalWrite(output3, !digitalRead(output3));
        }
        if (button == 4)
        {
          if (!digitalRead(output4))
          {
            tft.fillRect(buttonlist[3].x1, buttonlist[3].y1, 40, 40, TFT_CYAN);
          }
          else
          {
            tft.fillRect(buttonlist[3].x1, buttonlist[3].y1, 40, 40, TFT_BLUE);
          }
          digitalWrite(output4, !digitalRead(output4));
        }
        if (button == 5)
        {
          if (!digitalRead(output5))
          {
            tft.fillRect(buttonlist[4].x1, buttonlist[4].y1, 40, 40, TFT_CYAN);
          }
          else
          {
            tft.fillRect(buttonlist[4].x1, buttonlist[4].y1, 40, 40, TFT_BLUE);
          }
          digitalWrite(output5, !digitalRead(output5));
        }
        if (button == 6)
        {
          if (!digitalRead(output6))
          {
            tft.fillRect(buttonlist[5].x1, buttonlist[5].y1, 40, 40, TFT_CYAN);
          }
          else
          {
            tft.fillRect(buttonlist[5].x1, buttonlist[5].y1, 40, 40, TFT_BLUE);
          }
          digitalWrite(output6, !digitalRead(output6));
        }
        if (button == 7)
        {
          if (!digitalRead(output7))
          {
            tft.fillRect(buttonlist[6].x1, buttonlist[6].y1, 40, 40, TFT_CYAN);
          }
          else
          {
            tft.fillRect(buttonlist[6].x1, buttonlist[6].y1, 40, 40, TFT_BLUE);
          }
          digitalWrite(output7, !digitalRead(output7));
        }
        if (button == 8)
        {
          if (!digitalRead(output8))
          {
            tft.fillRect(buttonlist[7].x1, buttonlist[7].y1, 40, 40, TFT_CYAN);
          }
          else
          {
            tft.fillRect(buttonlist[7].x1, buttonlist[7].y1, 40, 40, TFT_BLUE);
          }
          digitalWrite(output8, !digitalRead(output8));
        }
      }
    }
    // tft.fillRect(44, 50, 40, 10, TFT_BLACK);
    // tft.fillRect(108, 50, 40, 10, TFT_BLACK);
    // tft.fillRect(172, 50, 40, 10, TFT_BLACK);
    // tft.fillRect(236, 50, 40, 10, TFT_BLACK);
    if (digitalRead(input1))
    {
      delay(10);
      if (digitalRead(input1))
      {
        // tft.setCursor(44, 55);
        // tft.print(analogRead(input1));
        tft.fillCircle(64, 80, 15, TFT_RED);
      }
    }
    else
    {
      tft.fillCircle(64, 80, 15, TFT_BLACK);
    }
    if (digitalRead(input2))
    {
      delay(5);
      if (digitalRead(input2))
      {
        // tft.setCursor(108, 55);
        // tft.print(analogRead(input2));
        tft.fillCircle(128, 80, 15, TFT_RED);
      }
    }
    else
    {
      tft.fillCircle(128, 80, 15, TFT_BLACK);
    }
    if (digitalRead(input3))
    {
      delay(10);
      if (digitalRead(input3))
      {
        // tft.setCursor(172, 55);
        // tft.print(analogRead(input3));
        tft.fillCircle(192, 80, 15, TFT_RED);
      }
    }
    else
    {
      tft.fillCircle(192, 80, 15, TFT_BLACK);
    }
    if (digitalRead(input4))
    {
      delay(10);
      if (digitalRead(input4))
      {
        // tft.setCursor(236, 55);
        // tft.print(analogRead(input4));
        tft.fillCircle(256, 80, 15, TFT_RED);
      }
    }
    else
    {
      tft.fillCircle(256, 80, 15, TFT_BLACK);
    }
  }
}
void page_wifi(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
    tft.setCursor(60, screen_header + 40);
    tft.textsize = 8;
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("wifi");
    tft.textsize = 1;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    int timer = millis();

    renderexitbutton = true;
    first = false;
  }
  else
  {
    if (millis() >= timer)
    {
      tft.fillRect(0, screen_header + 100, 320, (480 - 100 - screen_footer) - screen_header, TFT_BLACK);
      tft.setCursor(10, screen_header + 100);
      int n = WiFi.scanNetworks();
      if (n == 0)
      {
        Serial.print("no network found");
        tft.println("no network found");
      }
      else if (n > 0)
      {
        Serial.print(n);
        Serial.println(" network found");
        tft.print(n);
        tft.println(" network found");
        for (int i = 0; i < n; i++)
        {
          Serial.print(i + 1);
          Serial.print(" ");
          Serial.println(WiFi.SSID(i).c_str());
          tft.print(i + 1);
          tft.print(" ");
          tft.println(WiFi.SSID(i).c_str());
        }
        WiFi.scanDelete();
        timer = millis() + 5000;
      }
      else
      {
        Serial.print("error ");
        Serial.println(n);
        tft.print("error ");
        tft.println(n);
      }
    }
  }
}
void page_bluetooth(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
    buttonplacer(bluetoothbuttonname, bluetoothbuttonname_s, false);

    renderexitbutton = true;
    first = false;
  }
  else
  {
    if (touch)
    {
      int button = buttonselect(x, y);
      if (button == 1)
      {
        first = true;
        subpageint = button;
      }
    }
  }
}
void page_bluetoothserial(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
    for (int i = 0; i < 18; i++)
    {
      buttonlist[i] = {0, 0, 0, 0};
    }
    buttonlist_s = 0;
    int x = 64;
    int y = 440 - screen_footer;
    for (int i = 0; i < 4; i++)
    {
      tft.fillRect(x + (x * i) - 20, y, 40, 40, TFT_DARKCYAN);
      tft.setCursor(x + (x * i), y + 20);
      tft.print(i + 1);
      buttonlist[i] = {x + (x * i) - 20, y, 40, 40};
      buttonlist_s += 1;
    }
    for (int i = 0; i < 20; i++)
    {
      lines[i] = "";
      lineid[i] = 0;
    }
    tft.setCursor(60, screen_header + 40);
    tft.textsize = 8;
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("btserial");
    tft.textsize = 1;
    // serialbt.begin("esp32_handytool");

    renderexitbutton = true;
    first = false;
  }
  else
  {
    if (lines_s == 19)
    {
      for (int i = 0; i < 19; i++)
      {
        if (i <= 18)
        {
          lines[i] = lines[i + 1];
          lineid[i] = lineid[i + 1];
        }
      }
      lines_s = 18;
    }
    if (touch)
    {
      int button = buttonselect(x, y);
      if (millis() >= timer)
      {
        timer = millis() + 1000;
        if (button == 1)
        {
          writescreen("hi", 1, 2);
        }
        else if (button == 2)
        {
          writescreen("bye", 1, 2);
        }
        else if (button == 3)
        {
          writescreen("something", 1, 2);
        }
        else if (button == 4)
        {
          writescreen("another thing", 1, 2);
        }
      }
    }
    // if (serialbt.available())
    // {
    //   writescreen(serialbt.readStringUntil('\n'), 2, 2);
    // }
  }
}
void page_NFC(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
    tft.setCursor(60, screen_header + 40);
    tft.textsize = 8;
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("NFC");
    tft.textsize = 1;
    renderexitbutton = true;
    nfc.begin();
    delay(1000);

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata)
    {
      Serial.print("Didn't find PN53x board");
    }

    // Got ok data, print it out!
    Serial.print("Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware ver. ");
    Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print('.');
    Serial.println((versiondata >> 8) & 0xFF, DEC);

    // Set the max number of retry attempts to read from a card
    // This prevents us from waiting forever for a card, which is
    // the default behaviour of the PN532.
    nfc.setPassiveActivationRetries(0xFF);

    Serial.println("Waiting for an ISO14443A card");

    renderexitbutton = true;
    first = false;
  }
  else
  {
    boolean success;
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
    uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success)
    {
      Serial.println("Found a card!");
      Serial.print("UID Length: ");
      Serial.print(uidLength, DEC);
      Serial.println(" bytes");
      Serial.print("UID Value: ");
      for (uint8_t i = 0; i < uidLength; i++)
      {
        Serial.print(" 0x");
        Serial.print(uid[i], HEX);
      }
      Serial.println("");
      // Wait 1 second before continuing
      delay(1000);
    }
    else
    {
      // PN532 probably timed out waiting for a card
      Serial.println("Timed out waiting for a card");
    }
  }
}
void page_GPS(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
    neogps.begin(9600, SERIAL_8N1, rx2, tx2);
    neogps.println();
    timer = millis();
    tft.setCursor(60, screen_header + 40);
    tft.textsize = 8;
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("GPS");
    tft.textsize = 1;

    renderexitbutton = true;
    first = false;
  }
  else
  {
    if (neogps.available() > 0 && millis() >= timer)
    {
      tft.fillRect(0, screen_header + 100, 320, (480 - 100 - screen_footer) - screen_header, TFT_BLACK);
      gps.encode(neogps.read());
      tft.setCursor(10, screen_header + 100);
      tft.print("location : ");
      // if (gps.location.isValid() == 1)
      // {
      tft.println("");
      tft.print("latitude : ");
      tft.println(gps.location.lat());
      tft.print("longtitude : ");
      tft.println(gps.location.lng());
      tft.printf("altitude : %d m\n", gps.altitude.meters());
      tft.printf("speed : %d km/h\n", gps.speed.kmph());
      tft.print("satelite available : ");
      tft.println(gps.satellites.value());
      tft.print("course : ");
      tft.println(gps.course.value());
      tft.printf("time : %i:%i:%i \n", gps.time.hour(), gps.time.minute(), gps.time.second());
      tft.printf("date : %i,%i,%i \n", gps.date.year(), gps.date.month(), gps.date.day());
      // }
      // else
      // {
      //   tft.print("invalid");
      // }
      timer = millis() + 10000;
    }
  }
}
void page_setting(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
    buttonplacer(settingbuttonname, settingbuttonname_s, true);
    timer = millis();
    renderexitbutton = true;
    first = false;
  }
  else
  {
    if (touch)
    {
      int button = buttonselect(x, y);
      if (button == 1)
      {
        SPIFFS.remove(CALIBRATION_FILE);
        touch_calibrate();
      }
      else if (button == 2)
      {
        ESP.restart();
      }
    }
    if (millis() >= timer)
    {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setCursor(0, 200);

      tft.println();
      tft.print("    chipcores: ");
      tft.println(ESP.getChipCores());
      tft.print("    chipmodel: ");
      tft.println(ESP.getChipModel());
      tft.print("    chiprevision: ");
      tft.println(ESP.getChipRevision());
      tft.println();

      tft.print("    cpufreqMHZ: ");
      tft.println(ESP.getCpuFreqMHz());

      tft.print("    cyclecount: ");
      tft.println(ESP.getCycleCount());

      tft.print("    efusemac: ");
      tft.println(ESP.getEfuseMac());

      tft.print("    freePsram: ");
      tft.println(ESP.getFreePsram());

      tft.print("    sdkversion: ");
      tft.println(ESP.getSdkVersion());

      tft.println();
      tft.print("    sketchsize: ");
      tft.println(ESP.getSketchSize());
      tft.print("    freesketchspace: ");
      tft.println(ESP.getFreeSketchSpace());
      tft.print("    sketchmd5: ");
      tft.println(ESP.getSketchMD5());
      tft.println();

      tft.println();
      tft.print("    maxallocpsram: ");
      tft.println(ESP.getMaxAllocPsram());
      tft.print("    minfreepsram: ");
      tft.println(ESP.getMinFreePsram());
      tft.print("    psramsize: ");
      tft.println(ESP.getPsramSize());
      tft.println();

      tft.println();
      tft.print("    flashchipmode: ");
      tft.println(ESP.getFlashChipMode());
      tft.print("    flashchipsize: ");
      tft.println(ESP.getFlashChipSize());
      tft.print("    flashchipspeed: ");
      tft.println(ESP.getFlashChipSpeed());
      tft.println();

      tft.println();
      tft.print("    heapsize: ");
      tft.println(ESP.getHeapSize());
      tft.print("    freeheap: ");
      tft.println(ESP.getFreeHeap());
      tft.print("    maxallocheap: ");
      tft.println(ESP.getMaxAllocHeap());
      tft.print("    minfreeheap: ");
      tft.println(ESP.getMinFreeHeap());
      tft.println();

      // ESP.restart();
      // ESP.deepSleep();
      // ESP.

      timer += 1000;
    }
  }
}

void page_serial(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
    for (int i = 0; i < 18; i++)
    {
      buttonlist[i] = {0, 0, 0, 0};
    }
    buttonlist_s = 0;
    int x = 64;
    int y = 440 - screen_footer;
    for (int i = 0; i < 4; i++)
    {
      tft.fillRect(x + (x * i) - 20, y, 40, 40, TFT_DARKCYAN);
      tft.setCursor(x + (x * i), y + 20);
      tft.print(i + 1);
      buttonlist[i] = {x + (x * i) - 20, y, 40, 40};
      buttonlist_s += 1;
    }
    for (int i = 0; i < 20; i++)
    {
      lines[i] = "";
      lineid[i] = 0;
    }
    lines_s = 0;
    tft.setCursor(0, 20 + screen_header);
    int timer = millis() + 1000;
    renderexitbutton = true;
    first = false;
  }
  else
  {
    if (lines_s == 19)
    {
      for (int i = 0; i < 19; i++)
      {
        if (i <= 18)
        {
          lines[i] = lines[i + 1];
          lineid[i] = lineid[i + 1];
        }
      }
      lines_s = 18;
    }
    if (touch)
    {
      int button = buttonselect(x, y);
      if (millis() >= timer)
      {
        timer = millis() + 1000;
        if (button == 1)
        {
          writescreen("hi", 1, 1);
        }
        else if (button == 2)
        {
          writescreen("bye", 1, 1);
        }
        else if (button == 3)
        {
          writescreen("something", 1, 1);
        }
        else if (button == 4)
        {
          writescreen("another thing", 1, 1);
        }
      }
    }
    if (Serial.available())
    {
      writescreen(Serial.readStringUntil('\n'), 2, 1);
    }
  }
}

void page_paint(bool touch, int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
    drawPalette(colors, numColors);
    color = colors[2];

    renderexitbutton = true;
    first = false;
  }
  else
  {
    if (touch && y > 40 && y < 460)
    {
      if (x >= 299 && y <= 240)
      {
        int button = buttonselect(x, y);
        if (button > 0)
        {
          color = colors[button - 1];
        }
      }
      else
      {
        if (color == TFT_BLACK)
        {
          tft.fillCircle(x, y, 5, color);
        }
        else
        {
          tft.fillCircle(x, y, 2, color);
        }
      }
    }
  }
}
void writescreen(String text, int id, int writeplace)
{
  Serial.println(text);
  lines[lines_s] = text;
  lineid[lines_s] = id;
  lines_s += 1;
  tft.fillRect(10, 20 + screen_header, 280, 300, TFT_BLACK);
  tft.setCursor(0, 20 + screen_header);
  for (int i = 0; i < lines_s; i++)
  {
    if (lineid[i] == 1)
    {
      tft.setTextColor(TFT_DARKGREEN);
    }
    else if (lineid[i] == 2)
    {
      tft.setTextColor(TFT_BLUE);
    }
    if (writeplace == 1)
    {
      Serial.println(lines[i]);
    }
    else if (writeplace == 2)
    {
      // serialbt.println(lines[i]);
    }
    tft.print("   ");
    tft.println(lines[i]);
  }
}
void buttonplacer(String name[], int buttoncount, bool emptyspacebelow)
{
  // empty button list
  for (int i = 0; i < buttonlist_s; i++)
  {
    buttonlist[i] = {0, 0, 0, 0};
  }
  buttonlist_s = 0;
  // int buttoncount = sizeof(name) / sizeof(name[0]);
  int width;
  int height;
  int vspacing;
  int spacing;
  int y = screen_header;
  // int buttoncount = 11;
  int c;
  if (buttoncount <= 6)
  {
    c = 1;
  }
  else if (buttoncount <= 12)
  {
    c = 2;
  }
  else if (buttoncount <= 18)
    c = 3;

  switch (c)
  {
  case 1:
    width = BUTTON_W_XL;
    height = BUTTON_H_M;
    spacing = calculateWidth_m(c, width);
    if (emptyspacebelow)
    {
      vspacing = 10;
    }
    else
    {
      vspacing = calculateheight_m(buttoncount, height);
    }
    y += vspacing;
    for (int i = 0; i < buttoncount; i++)
    {
      // tft.fillRect(spacing, y + i * (height + vspacing), width, height, TFT_CYAN);
      // tft.drawRect(spacing, y + i * (height + vspacing), width, height, TFT_WHITE);
      // tft.setCursor(spacing + 20, y + i * (height + vspacing) + 30);
      // tft.setTextColor(TFT_BLACK, TFT_CYAN);
      // tft.println(name[i]);
      makebutton(spacing, y + (i * (height + vspacing)), width, height, name[i], i);
    }
    break;

  case 2:
    width = BUTTON_W_M;
    height = BUTTON_H_S;
    spacing = calculateWidth_m(c, width);
    vspacing = calculateheight_m(6, height);
    y += vspacing;
    for (int i = 0; i < buttoncount; i++)
    {
      if (i % 2 == 0)
      {
        // tft.fillRect(spacing, y, width, height, TFT_CYAN);
        // tft.drawRect(spacing, y, width, height, TFT_WHITE);
        // tft.setCursor(spacing + 20, y + 30);
        // tft.setTextColor(TFT_BLACK, TFT_CYAN);
        // tft.println(name[i]);
        makebutton(spacing, y, width, height, name[i], i);
      }
      else if (i % 2 == 1)
      {
        // tft.fillRect(width + (2 * spacing), y, width, height, TFT_CYAN);
        // tft.drawRect(width + (2 * spacing), y, width, height, TFT_WHITE);
        // tft.setCursor(width + (2 * spacing) + 20, y + 30);
        // tft.setTextColor(TFT_BLACK, TFT_CYAN);
        // tft.println(name[i]);
        makebutton(width + (2 * spacing), y, width, height, name[i], i);
        y = y + vspacing + height;
      }
    }
    break;

  case 3:
    width = BUTTON_W_S;
    height = BUTTON_H_S;
    spacing = calculateWidth_m(c, width);
    vspacing = calculateheight_m(6, height);
    y += vspacing;
    for (int i = 0; i < buttoncount; i++)
    {
      if (i % 3 == 0)
      {
        // tft.fillRect(spacing, y, width, height, TFT_CYAN);
        // tft.drawRect(spacing, y, width, height, TFT_WHITE);
        // tft.setCursor(spacing + 20, y + 30);
        // tft.setTextColor(TFT_BLACK, TFT_CYAN);
        // tft.println(name[i]);
        makebutton(spacing, y, width, height, name[i], i);
      }
      else if (i % 3 == 1)
      {
        // tft.fillRect(width + (2 * spacing), y, width, height, TFT_CYAN);
        // tft.drawRect(width + (2 * spacing), y, width, height, TFT_WHITE);
        // tft.setCursor(width + (2 * spacing) + 20, y + 30);
        // tft.setTextColor(TFT_BLACK, TFT_CYAN);
        // tft.println(name[i]);
        makebutton(width + (2 * spacing), y, width, height, name[i], i);
      }
      else if (i % 3 == 2)
      {
        // tft.fillRect((2 * width) + (3 * spacing), y, width, height, TFT_CYAN);
        // tft.drawRect((2 * width) + (3 * spacing), y, width, height, TFT_WHITE);
        // tft.setCursor((2 * width) + (3 * spacing) + 20, y + 30);
        // tft.setTextColor(TFT_BLACK, TFT_CYAN);
        // tft.println(name[i]);
        makebutton((2 * width) + (3 * spacing), y, width, height, name[i], i);
        y = y + vspacing + height;
      }
    }
    break;

  default:
    break;
  }
}
void makebutton(int x, int y, int width, int height, String name, int i)
{
  tft.fillRect(x, y, width, height, TFT_CYAN);
  tft.drawRect(x, y, width, height, TFT_WHITE);
  tft.setCursor(x + 20, y + (height / 2));
  tft.setTextColor(TFT_BLACK, TFT_CYAN);
  tft.println(name);
  buttonlist[i] = {x, y, width, height};
  buttonlist_s += 1;
}
int calculateWidth_m(int count, int bw)
{
  return round((float)((screen_width - (bw * count)) / (count + 1)));
}
int calculateheight_m(int count, int bh)
{
  return round((float)(((screen_hight) - (bh * count)) / (count + 1)));
}
int buttonselect(int x, int y)
{
  for (int i = 0; i < buttonlist_s; i++)
  {
    if (x != 0 && y != 0 && x > buttonlist[i].x1 && x < buttonlist[i].x2 && y > buttonlist[i].y1 && y < buttonlist[i].y2)
    {
      return i + 1;
      break;
    }
  }
}
void drawPalette(uint16_t colors[], int numColors)
{
  int x = 299;     // x position of the palette
  int y = 41;      // y starting position of the palette
  int width = 20;  // width of each rectangle
  int height = 20; // height of each rectangle
  int spacing = 1; // spacing between each rectangle
  for (int i = 0; i < 18; i++)
  {
    buttonlist[i] = {0, 0, 0, 0};
  }
  buttonlist_s = 0;
  for (int i = 0; i < numColors; i++)
  {
    tft.fillRect(x, y + i * (height + spacing), width, height, colors[i]);
    buttonlist[i] = {x, y + i * (height + spacing), width, height};
  }
}

// void changecolor(int y)
// {
//   // Calculate the index based on the y value
//   y = (y - 40) - y % 20;
//   int index = y / 20;
//   // Ensure index is within bounds
//   if (index < 0)
//     index = 0;
//   else if (index >= sizeof(colors) / sizeof(colors[0]))
//     index = sizeof(colors) / sizeof(colors[0]) - 1;

//   // Set the color
//   color = colors[index];
// }

// Code to run a screen calibration, not needed when calibration values set in setup()
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE))
  {
    File f = SPIFFS.open(CALIBRATION_FILE, "r");
    if (f)
    {
      if (f.readBytes((char *)calData, 14) == 14)
        calDataOK = 1;
      f.close();
    }
  }

  if (calDataOK && !REPEAT_CAL)
  {
    // calibration data valid
    tft.setTouch(calData);
  }
  else
  {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL)
    {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f)
    {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
    first = true;
  }
}