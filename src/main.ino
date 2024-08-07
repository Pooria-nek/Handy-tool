#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>

#define pin 19
TFT_eSPI tft = TFT_eSPI();
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
  pageint = 0;
  first = true;
  // Calibrate the touch screen and retrieve the scaling factors
  // touch_calibrate();
  uint16_t calData[5] = {243, 3605, 176, 3636, 0};
  tft.setTouch(calData);

  tft.setRotation(0);
  screen_width = tft.width();
  screen_hight = tft.height() - screen_header - screen_footer;
  tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
  tft.drawRect(0, screen_header, screen_width, screen_hight, TFT_WHITE);
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

  // Calibrate
  tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);
  tft.setCursor(20, 0);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.println("Touch corners as indicated");

  tft.setTextFont(1);
  tft.println();

  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

  Serial.println();
  Serial.println();
  Serial.println("// Use this calibration code in setup():");
  Serial.print("  uint16_t calData[5] = ");
  Serial.print("{ ");

  for (uint8_t i = 0; i < 5; i++)
  {
    Serial.print(calData[i]);
    if (i < 4)
      Serial.print(", ");
  }

  Serial.println(" };");
  Serial.print("  tft.setTouch(calData);");
  Serial.println();
  Serial.println();

  tft.fillRect(0, screen_header, screen_width, screen_hight, TFT_BLACK);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Calibration complete!");
  tft.println("Calibration code sent to Serial port.");

  delay(4000);
}