// -------------------- БИБЛИОТЕКИ ---------------------
#include <string.h>             // библиотека расширенной работы со строками
#include <Adafruit_GFX.h>       // Core graphics library
#include <MCUFRIEND_kbv.h>      // Библиотека для работы с дисплеем
#include "images.h"
// -------------------- БИБЛИОТЕКИ ---------------------

// -------------------- КОНСТАНТЫ ---------------------
#define BLACK   0x0000
#define GREY    0x8C71
#define BLUE    0x4B9F
#define BLUE2   0x0037
#define GREEN   0x06A0
#define GREEN2  0x23A0
#define RED     0xD824
#define RED2    0xB000
#define YELLOW  0xAD80
#define YELLOW2 0x73A0
#define PURP    0xC23F
#define PURP2   0x7017
#define PI2 3.14
#define COLOR1  0xB256
#define COLOR2  0xB2B7
#define COLOR3  0xBB58
#define COLOR4  0xC3F9
#define COLOR5  0xCC59
#define COLOR6  0xD4FA
#define COLOR7  0xDD7B
#define COLOR8  0xE63D
// -------------------- КОНСТАНТЫ ---------------------


// -------- ПЕРЕМЕННЫЕ -------------
MCUFRIEND_kbv tft;
char inputData[250];
int intValues[17];
float floatValues[6];
int ifVal[6];
unsigned long timeout = 0;
byte index = 0, red = 0;
boolean reDraw_flag = 1, updateDisplay_flag, timeOut_flag = 1, text_flag = 1;
String data_value, _time, _date;
// -------- ПЕРЕМЕННЫЕ -------------


void setup() {
  Serial.begin(9600);
  uint16_t identifier = tft.readID();
  tft.begin(identifier);//to enable ILI9327 driver code
  tft.reset();
  tft.setRotation(3);
  show_logo();
}


// ------------------------------ ОСНОВНОЙ ЦИКЛ -------------------------------
void loop() {
  parsing();
  updateDisplay();
  timeoutTick();
}
// ------------------------------ ОСНОВНОЙ ЦИКЛ -------------------------------

void show_logo() {
  tft.fillScreen(BLACK);
  tft.setTextColor(GREY, BLACK);
  tft.setTextSize(10);
  tft.setCursor(150, 40);
  tft.print("PC");
  tft.setTextSize(6);
  tft.setCursor(22, 140);
  tft.print("MONITORING");
}

void parsing() {
  while (Serial.available() > 0) {
    char aChar = Serial.read();
    if (aChar != '&') {
      inputData[index] = aChar;
      index++;
      inputData[index] = '\0';
    } else {
      char *p = inputData;
      char *str;
      index = 0;
      memset(intValues, 0, sizeof(intValues));
      memset(ifVal, 0, sizeof(ifVal));
      memset(floatValues, 0, sizeof(floatValues));
      while ((str = strtok_r(p, ";", &p)) != NULL) {
        data_value = str;
        if (index == 0) {
          _time = str;
        } else if (index == 1) { 
          _date = str;
        } else if (index > 1 && index < 19) {
          intValues[index - 2] = data_value.toInt();
        } else {
          floatValues[index - 19] = data_value.toFloat();
          ifVal[index - 19] = data_value.toInt();
        }
        index++;
      }
      index = 0;
      updateDisplay_flag = 1;
    }
    timeout = millis();
    timeOut_flag = 1;
    text_flag = 1;
  }
}

void updateDisplay() {
  if (updateDisplay_flag) {
    if (reDraw_flag) {
      tft.reset();
      tft.setRotation(3);
      tft.fillScreen(BLACK);
      draw_labels();
      reDraw_flag = 0;
    }
    if (red == 1) {
      draw_stats();
    } else {
      red++;
    }
    updateDisplay_flag = 0;
  }
}

void drowCel(int x, int y, int s, uint16_t color) {
  tft.setTextSize(s);
  tft.drawChar(x, (y - 2), 111, color, BLACK, 1);
  tft.setCursor((x + 7), y);
  tft.print("C");
}

void drowString(uint16_t x, uint16_t y, String msg) {
  tft.setCursor(x, y);
  tft.print(msg);
}

String getPersentage(int val) {
  if (val < 10) {
    return String(val) + "%  ";
  } else if (val < 100) {
    return String(val) + "% ";
  } else {
    return "100%";
  }
}

String getPower(int val) {
  if (val < 10) {
    return String(val) + "W  ";
  } else if (val < 100) {
    return String(val) + "W ";
  } else {
    return String(val)+"W";
  }
}

String getSpeed(int ind) {
  if (ifVal[ind] < 10) {
    return "  " + String(floatValues[ind]);
  } else if (ifVal[ind] < 100) {
    return " " + String(floatValues[ind]);
  } else if (ifVal[ind] > 999) {
    return (String(floatValues[ind])).substring(0, 6);
  } else {
    return String(floatValues[ind]);
  }
}

void draw_labels() {
  tft.fillScreen(BLACK);
  tft.setTextSize(2);

  //CPU
  tft.setTextColor(BLUE, BLACK);
  tft.drawRoundRect(5, 5, 175, 80, 10, BLUE);
  drowString(44, 10, "i9-9900K");
  tft.drawLine(5, 29, 178, 29, BLUE);
  tft.drawBitmap(12, 36, temp, 16, 16, BLUE2);
  drowCel(55, 37, 2, BLUE);
  tft.drawBitmap(85, 36, usage, 16, 16, BLUE2);
  tft.drawBitmap(12, 59, flash, 16, 16, BLUE2);
  tft.drawBitmap(85, 59, fan, 16, 16, BLUE2);
  tft.setTextSize(1); drowString(154, 68, "RPM"); tft.setTextSize(2);

  //GPU
  tft.setTextColor(GREEN, BLACK);
  tft.drawRoundRect(188, 5, 205, 80, 10, GREEN);
  drowString(226, 10, "RTX 2080 Ti");
  tft.drawLine(188, 29, 392, 29, GREEN);
  tft.drawBitmap(192, 36, temp, 16, 16, GREEN2);
  drowCel(235, 37, 2, GREEN);
  tft.drawBitmap(265, 36, usage, 16, 16, GREEN2);
  tft.drawBitmap(328, 36, chip, 16, 16, GREEN2);
  tft.drawBitmap(195, 59, flash, 16, 16, GREEN2);
  tft.drawBitmap(298, 59, fan, 16, 16, GREEN2);
  tft.setTextSize(1); drowString(367, 68, "RPM"); tft.setTextSize(2);


  //MEM
  tft.setTextColor(RED, BLACK);
  tft.drawRoundRect(5, 93, 207, 55, 10, RED);
  drowString(73, 98, "MEMORY");
  tft.drawLine(6, 117, 210, 117, RED);
  tft.drawBitmap(12, 124, usage, 16, 16, RED2);
  tft.drawBitmap(75, 124, temp, 16, 16, RED2);
  drowCel(118, 125, 2, RED);
  tft.drawBitmap(143, 124, temp, 16, 16, RED2);
  drowCel(185, 125, 2, RED);


  //POWER
  tft.setTextColor(YELLOW, BLACK);
  tft.drawRoundRect(220, 93, 173, 55, 10, YELLOW);
  drowString(278, 98, "POWER");
  tft.drawLine(220, 117, 391, 117, YELLOW);
  tft.drawBitmap(227, 124, temp, 16, 16, YELLOW2);
  drowCel(270, 125, 2, YELLOW);
  tft.drawBitmap(315, 124, flash, 16, 16, YELLOW2);


  //DRIVES
  tft.drawRoundRect(5, 155, 278, 80, 10, PURP);
  tft.setTextColor(PURP, BLACK);
  drowString(15, 165, "C:"); 
  tft.drawBitmap(40, 164, temp, 16, 16, PURP2); 
  drowCel(85, 165, 2, PURP); 
  drowString(15, 187, "E:"); 
  tft.drawBitmap(40, 186, temp, 16, 16, PURP2); 
  drowCel(85, 187, 2, PURP); 
  drowString(15, 209, "F:"); 
  tft.drawBitmap(40, 208, temp, 16, 16, PURP2); 
  drowCel(85, 209, 2, PURP);

}

void draw_stats() {
  
  //CPU
  tft.setTextColor(BLUE, BLACK);
  drowString(30, 37, String(intValues[0]));
  drowString(105, 37, getPersentage(intValues[1]));
  drowString(30, 60, getPower(intValues[2]));
  drowString(105, 60, (intValues[3] < 1000 ? " "+String(intValues[3]) : String(intValues[3])));

  //GPU
  tft.setTextColor(GREEN, BLACK);
  drowString(210, 37, String(intValues[4]));
  if (intValues[5] < 100) {
    drowString(285, 37, getPersentage(intValues[5]).substring(0, 3));
  } else {
    drowString(285, 37, "100");
  }
  if (intValues[6] < 100) {
    drowString(348, 37, getPersentage(intValues[6]).substring(0, 3));
  } else {
    drowString(348, 37, "100");
  }
  drowString(213, 60, getPower(intValues[7]));
  drowString(318, 60, (intValues[8] < 1000 ? " "+String(intValues[8]) : String(intValues[8])));

  //MEM
  tft.setTextColor(RED, BLACK);
  drowString(30, 125, getPersentage(intValues[9]));
  drowString(93, 125, String(intValues[10]));
  drowString(160, 125, String(intValues[11]));

  //POWER
  tft.setTextColor(YELLOW, BLACK);
  drowString(245, 125, String(intValues[13]));
  drowString(333, 125, getPower(intValues[12]));

  //DRIVERS
  tft.setTextColor(PURP, BLACK);
  drowString(60, 165, String(intValues[14])); 
  drowString(115, 165, getSpeed(0) + " " + getSpeed(1));
  drowString(60, 187, String(intValues[15])); 
  drowString(115, 187, getSpeed(2) + " " + getSpeed(3));
  drowString(60, 209, String(intValues[16])); 
  drowString(115, 209, getSpeed(4) + " " + getSpeed(5));

  tft.setTextColor(GREY, BLACK); 
  tft.setTextSize(3);
  drowString(298, 170, _time);
  tft.setTextSize(2);
  drowString(294, 203, _date);
}

void timeoutTick() {
  if ((millis() - timeout > 3000) && timeOut_flag) {
    if (text_flag) {
      tft.fillScreen(BLACK);
      tft.setTextColor(GREY, BLACK);
      text_flag = 0;
      tft.setTextSize(6);
      tft.setCursor(74, 120);
      tft.print("WAITING");
      tft.setTextSize(6);
      tft.setCursor(56, 176);
      tft.print("FOR DATA");
      red = 0;
    }
    for (int i = 8; i > 0; i--) {
      tft.fillCircle(200 + 40 * (cos(-i * PI2 / 4)), 60 + 40 * (sin(-i * PI2 / 4)), 10,  COLOR1); delay(15);
      tft.fillCircle(200 + 40 * (cos(-(i + 1)*PI2 / 4)), 60 + 40 * (sin(-(i + 1)*PI2 / 4)), 10,  COLOR2); delay(15);
      tft.fillCircle(200 + 40 * (cos(-(i + 2)*PI2 / 4)), 60 + 40 * (sin(-(i + 2)*PI2 / 4)), 10,  COLOR3); delay(15);
      tft.fillCircle(200 + 40 * (cos(-(i + 3)*PI2 / 4)), 60 + 40 * (sin(-(i + 3)*PI2 / 4)), 10,  COLOR4); delay(15);
      tft.fillCircle(200 + 40 * (cos(-(i + 4)*PI2 / 4)), 60 + 40 * (sin(-(i + 4)*PI2 / 4)), 10,  COLOR5); delay(15);
      tft.fillCircle(200 + 40 * (cos(-(i + 5)*PI2 / 4)), 60 + 40 * (sin(-(i + 5)*PI2 / 4)), 10,  COLOR6); delay(15);
      tft.fillCircle(200 + 40 * (cos(-(i + 6)*PI2 / 4)), 60 + 40 * (sin(-(i + 6)*PI2 / 4)), 10,  COLOR7); delay(15);
      tft.fillCircle(200 + 40 * (cos(-(i + 7)*PI2 / 4)), 60 + 40 * (sin(-(i + 7)*PI2 / 4)), 10,  COLOR8); delay(15);
    }
    reDraw_flag = 1;
  }
}
