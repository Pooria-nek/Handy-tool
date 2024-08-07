#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>

#define pin 19
TFT_eSPI tft = TFT_eSPI();
uint32_t color;
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
  tft.fillRect(299 , 1 , 20 , 20 , TFT_YELLOW);
  tft.fillRect(299 , 21 , 20 , 20 , TFT_ORANGE);
  tft.fillRect(299 , 41 , 20 , 20 , TFT_RED);
  tft.fillRect(299 , 61 , 20 , 20 , TFT_GREEN);
  tft.fillRect(299 , 81 , 20 , 20 , TFT_DARKGREEN);
  tft.fillRect(299 , 101 , 20 , 20 , TFT_CYAN);
  tft.fillRect(299 , 121 , 20 , 20 , TFT_BLUE);
  tft.fillRect(299 , 141 , 20 , 20 , TFT_DARKGREY);
  tft.fillRect(299 , 161 , 20 , 20 , TFT_WHITE);
  tft.drawRect(299 , 181 , 20 , 20 , TFT_WHITE);
  tft.drawRect(304 , 186 , 10 , 10 , TFT_WHITE);
}

void loop()
{
  uint16_t x, y;
  bool touch = tft.getTouch(&x, &y, 10);
  if(touch)
  {
    if(x >= 299 && y <= 201)
    {
      changecolor(x , y);
    }
    else
    {
      if(color == TFT_BLACK)
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
void changecolor(int x , int y)
{
  if(y <= 21)
  {
    color = TFT_YELLOW;
  }
  else if (y > 21 && y <= 41)
  {
    color = TFT_ORANGE;
  }
  else if(y > 41 && y <= 61)
  {
    color = TFT_RED;
  }
  else if(y > 61 && y <= 81)
  {
    color = TFT_GREEN;
  }
  else if(y > 81 && y <= 101)
  {
    color = TFT_DARKGREEN;
  }
  else if(y > 101 && y <= 121)
  {
    color = TFT_CYAN;
  }
  else if(y > 121 && y <= 141)
  {
    color = TFT_BLUE;
  }
  else if(y > 141 && y <= 161)
  {
    color = TFT_DARKGREY;
  }
  else if(y > 161 && y <= 181)
  {
    color =TFT_WHITE;
  }
  else if(y > 181 && y <= 201)
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