#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>

#define pin 19
TFT_eSPI tft = TFT_eSPI();
uint32_t color;

uint16_t colors[] = {TFT_YELLOW, TFT_ORANGE, TFT_RED, TFT_GREEN, TFT_DARKGREEN, TFT_CYAN, TFT_BLUE, TFT_DARKGREY, TFT_WHITE};
int numColors = sizeof(colors) / sizeof(colors[0]); // Calculate the number of colors

void setup()
{
  Serial.begin(115200);
  tft.init();

  // Calibrate the touch screen and retrieve the scaling factors
  // touch_calibrate();
  uint16_t calData[5] = {243, 3605, 176, 3636, 0};
  tft.setTouch(calData);

  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
  color = TFT_RED;
  // Draw the palette
  drawPalette(colors, numColors);
}

void loop()
{
  uint16_t x, y;
  bool touch = tft.getTouch(&x, &y, 10);
  if (touch)
  {
    if (x >= 299 && y <= 201)
    {
      changecolor(x, y);
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
      Serial.print(x);
      Serial.print("_");
      Serial.println(y);
    }
  }
}

void drawPalette(uint16_t colors[], int numColors)
{
  int x = 299;     // x position of the palette
  int y = 1;       // y starting position of the palette
  int width = 20;  // width of each rectangle
  int height = 20; // height of each rectangle
  int spacing = 1; // spacing between each rectangle

  for (int i = 0; i < numColors; i++)
  {
    tft.fillRect(x, y + i * (height + spacing), width, height, colors[i]);
  }
}

void changecolor(int x, int y)
{
  if (y <= 21)
  {
    color = TFT_YELLOW;
  }
  else if (y > 21 && y <= 41)
  {
    color = TFT_ORANGE;
  }
  else if (y > 41 && y <= 61)
  {
    color = TFT_RED;
  }
  else if (y > 61 && y <= 81)
  {
    color = TFT_GREEN;
  }
  else if (y > 81 && y <= 101)
  {
    color = TFT_DARKGREEN;
  }
  else if (y > 101 && y <= 121)
  {
    color = TFT_CYAN;
  }
  else if (y > 121 && y <= 141)
  {
    color = TFT_BLUE;
  }
  else if (y > 141 && y <= 161)
  {
    color = TFT_DARKGREY;
  }
  else if (y > 161 && y <= 181)
  {
    color = TFT_WHITE;
  }
  else if (y > 181 && y <= 201)
  {
    color = TFT_BLACK;
  }
}

// Code to run a screen calibration, not needed when calibration values set in setup()
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Calibrate
  tft.fillScreen(TFT_BLACK);
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

  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Calibration complete!");
  tft.println("Calibration code sent to Serial port.");

  delay(4000);
}