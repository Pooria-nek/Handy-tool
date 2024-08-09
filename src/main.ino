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

#define BUTTON_W_XL 250
#define BUTTON_W_L 200
#define BUTTON_W_M 120
#define BUTTON_W_S 80
#define BUTTON_W_xS 40

#define BUTTON_H_XL 250
#define BUTTON_H_L 200
#define BUTTON_H_M 120
#define BUTTON_H_S 80
#define BUTTON_H_xS 40

int16_t button[][4] = {
    {0, 0, 10, 10}};

int16_t screen_width, screen_hight, screen_header = 40, screen_footer = 20;
uint32_t color;
String mainbuttonname[] = {"I/O", "Signal", "wifi", "bluetooth", "sub-G", "NFC", "GPS", "NRF24", "IR", "paint", "serial", "setting"};
size_t mainbuttonname_s = sizeof(mainbuttonname) / sizeof(mainbuttonname[0]); // Calculate the number of colors
String settingbuttonname[] = {"recollibrate"};
size_t settingbuttonname_s = sizeof(settingbuttonname) / sizeof(settingbuttonname[0]);
uint16_t colors[] = {TFT_YELLOW, TFT_ORANGE, TFT_RED, TFT_GREEN, TFT_DARKGREEN, TFT_CYAN, TFT_BLUE, TFT_DARKGREY, TFT_WHITE};
int numColors = sizeof(colors) / sizeof(colors[0]); // Calculate the number of colors
bool touch;
bool first = false;
bool renderbutton;
int pageint;
int timer;

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
  if (touch)
  {
    Serial.print(x);
    Serial.print("_");
    Serial.println(y);
    Serial.println(pageint);
  }
  if (pageint != 0)
  {
    if (renderbutton)
    {
      tft.fillCircle(10, screen_header + 10, 7, TFT_RED);
      renderbutton = false;
    }
    else
    {
      if (x < 20 && y > screen_header && y < screen_header + 20)
      {
        first = true;
        pageint = 0;
      }
    }
  }
  if (pageint == 0)
  {
    page_main(x, y);
  }
  else if (pageint == 1)
  {
    page_paint(touch, x, y);
  }
  else if (pageint == 2)
  {
    page_serial(x, y);
  }
  else if (pageint == 3)
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
    buttonplacer(mainbuttonname, mainbuttonname_s);
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
    buttonplacer(settingbuttonname, settingbuttonname_s);
    timer = millis();
    first = false;
    renderbutton = true;
  }
  else
  {
    if (x > 60 && x < 260 && y > 80 && y < 140)
      touch_calibrate();
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

    first = false;
    renderbutton = true;
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
    renderbutton = true;
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
void buttonplacer(String name[], int buttoncount)
{
  // int buttoncount = sizeof(name) / sizeof(name[0]);
  int width;
  int height = 60;
  int vspacing = 10;
  int spacing;
  int y = screen_header + vspacing;

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
    spacing = calculateWidth_m(c, width);
    for (int i = 0; i < buttoncount; i++)
    {
      tft.fillRect(spacing, y + i * (height + vspacing), width, height, TFT_CYAN);
      tft.drawRect(spacing, y + i * (height + vspacing), width, height, TFT_WHITE);
      tft.setCursor(spacing + 20, y + i * (height + vspacing) + 30);
      tft.setTextColor(TFT_BLACK, TFT_CYAN);
      tft.println(name[i]);
    }
    break;

  case 2:
    width = BUTTON_W_M;
    spacing = calculateWidth_m(c, width);
    for (int i = 0; i < buttoncount; i++)
    {
      if (i % 2 == 0)
      {
        tft.fillRect(spacing, y, width, height, TFT_CYAN);
        tft.drawRect(spacing, y, width, height, TFT_WHITE);
        tft.setCursor(spacing + 20, y + 30);
        tft.setTextColor(TFT_BLACK, TFT_CYAN);
        tft.println(name[i]);
      }
      else if (i % 2 == 1)
      {
        tft.fillRect(width + (2 * spacing), y, width, height, TFT_CYAN);
        tft.drawRect(width + (2 * spacing), y, width, height, TFT_WHITE);
        tft.setCursor(width + (2 * spacing) + 20, y + 30);
        tft.setTextColor(TFT_BLACK, TFT_CYAN);
        tft.println(name[i]);
        y = y + vspacing + height;
      }
    }
    break;

  case 3:
    width = BUTTON_W_S;
    spacing = calculateWidth_m(c, width);
    for (int i = 0; i < buttoncount; i++)
    {
      if (i % 3 == 0)
      {
        tft.fillRect(spacing, y, width, height, TFT_CYAN);
        tft.drawRect(spacing, y, width, height, TFT_WHITE);
        tft.setCursor(spacing + 20, y + 30);
        tft.setTextColor(TFT_BLACK, TFT_CYAN);
        tft.println(name[i]);
      }
      else if (i % 3 == 1)
      {
        tft.fillRect(width + (2 * spacing), y, width, height, TFT_CYAN);
        tft.drawRect(width + (2 * spacing), y, width, height, TFT_WHITE);
        tft.setCursor(width + (2 * spacing) + 20, y + 30);
        tft.setTextColor(TFT_BLACK, TFT_CYAN);
        tft.println(name[i]);
      }
      else if (i % 3 == 2)
      {
        tft.fillRect((2 * width) + (3 * spacing), y, width, height, TFT_CYAN);
        tft.drawRect((2 * width) + (3 * spacing), y, width, height, TFT_WHITE);
        tft.setCursor((2 * width) + (3 * spacing) + 20, y + 30);
        tft.setTextColor(TFT_BLACK, TFT_CYAN);
        tft.println(name[i]);
        y = y + vspacing + height;
      }
    }
    break;

  default:
    break;
  }
  // if (buttoncount <= 6)
  // {

  // }
  // else if (buttoncount <= 12)
  // {
  //   width = 120;
  //   for(int i = 0; i < 2; i++)
  //   {
  //     if(i == 0)
  //     {
  //       tft.fillRect(x, y , width, height, TFT_CYAN);
  //     tft.drawRect(x, y , width, height, TFT_WHITE);
  //     tft.setCursor(x + 20, y + i * (height + spacing) + 30);
  //     tft.setTextColor(TFT_BLACK, TFT_CYAN);
  //     tft.println(name[i]);
  //     }
  //     else
  //     {
  //       tft.fillRect(x + width + spacing, y , width, height, TFT_CYAN);
  //     tft.drawRect(x + width + spacing, y , width, height, TFT_WHITE);
  //     tft.setCursor(x + 20, y + i * (height + spacing) + 30);
  //     tft.setTextColor(TFT_BLACK, TFT_CYAN);
  //     tft.println(name[i]);
  //     }
  //   }
  //   for (int i = 2; i < buttoncount; i++)
  //   {

  //   }
  // }
  // else
  // {
  //   width = 80;
  // }
}
int calculateWidth_m(int count, int bw)
{
  return round((float)((screen_width - (bw * count)) / (count + 1)));
}
void buttonselect(int y)
{
  y = (y - 80) - y % 60;
  int index = y / 60;
  if (index <= 2 && index >= 0)
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
  }
}