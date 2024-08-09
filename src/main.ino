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

#define output1 35
#define output2 34
#define output3 39
#define output4 36

// #define input

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
size_t mainbuttonname_s = sizeof(mainbuttonname) / sizeof(mainbuttonname[0]); // Calculate the number of colors
String settingbuttonname[] = {"recollibrate", "reset"};
size_t settingbuttonname_s = sizeof(settingbuttonname) / sizeof(settingbuttonname[0]);
Button buttonlist[] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
size_t buttonlist_s = 0;
uint16_t colors[] = {TFT_YELLOW, TFT_ORANGE, TFT_RED, TFT_GREEN, TFT_DARKGREEN, TFT_CYAN, TFT_BLUE, TFT_DARKGREY, TFT_WHITE};
int numColors = sizeof(colors) / sizeof(colors[0]); // Calculate the number of colors
bool touch;
bool first = false;
bool renderexitbutton;
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
    page_i0(x, y);
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
    buttonplacer(mainbuttonname, mainbuttonname_s, false);
    first = false;
  }
  else
  {
    if (touch)
    {
      int button = buttonselect(x, y);
      if (button != 0)
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
    tft.drawCircle(64, 60, 20, TFT_WHITE);
    tft.drawCircle(128, 60, 20, TFT_WHITE);
    tft.drawCircle(192, 60, 20, TFT_WHITE);
    tft.drawCircle(256, 60, 20, TFT_WHITE);

    pinMode(output1, INPUT_PULLDOWN);
    pinMode(output2, INPUT_PULLDOWN);
    pinMode(output3, INPUT_PULLDOWN);
    pinMode(output4, INPUT_PULLDOWN);

    first = false;
  }
  else
  {
    if (digitalRead(output1))
    {
      tft.fillCircle(64, 60, 15, TFT_RED);
    }
    else
    {
      tft.fillCircle(64, 60, 15, TFT_BLACK);
    }
    if (digitalRead(output2))
    {
      tft.fillCircle(128, 60, 15, TFT_RED);
    }
    else
    {
      tft.fillCircle(128, 60, 15, TFT_BLACK);
    }
    if (digitalRead(output3))
    {
      tft.fillCircle(192, 60, 15, TFT_RED);
    }
    else
    {
      tft.fillCircle(192, 60, 15, TFT_BLACK);
    }
    if (digitalRead(output4))
    {
      tft.fillCircle(256, 60, 15, TFT_RED);
    }
    else
    {
      tft.fillCircle(256, 60, 15, TFT_BLACK);
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
    first = false;
    renderexitbutton = true;
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

    first = false;
    renderexitbutton = true;
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
    renderexitbutton = true;
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
void buttonplacer(String name[], int buttoncount, bool emptyspacebelow)
{
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
  return round((float)(((screen_hight - screen_footer - screen_header) - (bh * count)) / (count + 1)));
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
  // y = (y - 80) - y % 60;
  // int index = y / 60;
  // if (index <= 2 && index >= 0)
  // {
  //   pageint = index + 1;
  //   first = true;
  // }
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
    first = true;
  }
}