#include <Arduino.h>
#include "FS.h"
#include <TFT_eSPI.h>
#include <WiFi.h>

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

int16_t screen_width, screen_hight, screen_header = 40, screen_footer = 20;
uint32_t color;
String mainbuttonname[] = {"setting", "serial", "paint"};
uint16_t colors[] = {TFT_YELLOW, TFT_ORANGE, TFT_RED, TFT_GREEN, TFT_DARKGREEN, TFT_CYAN, TFT_BLUE, TFT_DARKGREY, TFT_WHITE};
int numColors = sizeof(colors) / sizeof(colors[0]); // Calculate the number of colors
bool touch;
bool first = false;
int pageint;

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
  if (touch)
  {
    Serial.print(x);
    Serial.print("_");
    Serial.println(y);
    Serial.println(pageint);
  }
  if (pageint == 0)
  {
    page_main(x, y);
  }
  else if (pageint == 1)
  {
    page_setting(x, y);
  }
  else if (pageint == 2)
  {
    page_serial(x, y);
  }
  else if (pageint == 3)
  {
    page_paint(touch, x, y);
  }
}

void page_main(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
    mainpagebutton();
    first = false;
  }
  else
  {
    if (touch && y > 40 && y < 460)
    {
      if (x >= 60 && x <= 260)
      {
        buttonselect(y);
      }
    }
  }
}

void page_setting(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);

    first = false;
  }
  else
  {
    /* code */
  }
}

void page_serial(int x, int y)
{
  if (first)
  {
    tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);

    first = false;
  }
  else
  {
    /* code */
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

    first = false;
  }
  else
  {
    if (touch && y > 40 && y < 460)
    {
      if (x >= 299 && y <= 201)
      {
        changecolor(y);
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
void mainpagebutton()
{
  int x = 60;
  int y = 80;
  int width = 200;
  int height = 60;
  int spacing = 10;
  for (int i = 0; i < 3; i++)
  {
    tft.fillRect(x, y + i * (height + spacing), width, height, TFT_CYAN);
    tft.drawRect(x, y + i * (height + spacing), width, height, TFT_WHITE);
    tft.setCursor(x + 20, y + i * (height + spacing) + 30);
    tft.println(mainbuttonname[i]);
  }
}
void buttonselect(int y)
{
  y = (y - 80) - y % 60;
  int index = y / 60;
  if (index < 0)
    index = 0;
  else if (index <= 2)
  {
    pageint = index + 1;
    first = true;
  }
}
void drawPalette(uint16_t colors[], int numColors)
{
  int x = 299;     // x position of the palette
  int y = 41;      // y starting position of the palette
  int width = 20;  // width of each rectangle
  int height = 20; // height of each rectangle
  int spacing = 1; // spacing between each rectangle

  for (int i = 0; i < numColors; i++)
  {
    tft.fillRect(x, y + i * (height + spacing), width, height, colors[i]);
  }
}

void changecolor(int y)
{
  // Calculate the index based on the y value
  y = (y - 40) - y % 20;
  int index = y / 20;
  // Ensure index is within bounds
  if (index < 0)
    index = 0;
  else if (index >= sizeof(colors) / sizeof(colors[0]))
    index = sizeof(colors) / sizeof(colors[0]) - 1;

  // Set the color
  color = colors[index];
}

// Code to run a screen calibration, not needed when calibration values set in setup()
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}