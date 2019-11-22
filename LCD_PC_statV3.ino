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
#define ORANGE  0xFC08
#define ORANGE2 0xCA00
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
String values[23];
unsigned long timeout = 0;
byte index = 0, red = 0;
boolean reDraw_flag = 1, updateDisplay_flag, timeOut_flag = 1, text_flag = 1;
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
      while ((str = strtok_r(p, ";", &p)) != NULL) {
        if (index < 23) {
          values[index] = String(str);
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
  tft.drawRoundRect(5, 93, 140, 55, 10, RED);
  drowString(40, 98, "MEMORY");
  tft.drawLine(6, 117, 143, 117, RED);
  tft.drawBitmap(12, 124, usage, 16, 16, RED2);
  tft.drawBitmap(75, 124, temp, 16, 16, RED2);
  drowCel(118, 125, 2, RED);
  //tft.drawBitmap(143, 124, temp, 16, 16, RED2);
  //drowCel(185, 125, 2, RED);


  //POWER
  tft.setTextColor(YELLOW, BLACK);
  tft.drawRoundRect(152, 93, 155, 55, 10, YELLOW);
  drowString(200, 98, "POWER");
  tft.drawLine(153, 117, 305, 117, YELLOW);
  tft.drawBitmap(159, 124, temp, 16, 16, YELLOW2);
  drowCel(202, 125, 2, YELLOW);
  tft.drawBitmap(230, 124, flash, 16, 16, YELLOW2);


  //NETWORK
  tft.setTextColor(ORANGE, BLACK);
  tft.drawRoundRect(314, 93, 80, 55, 10, ORANGE);
  tft.drawFastHLine(315, 120, 78, ORANGE);


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
  drowString(30, 37, values[2]);
  drowString(105, 37, values[3]);
  drowString(30, 60, values[4]);
  drowString(105, 60, values[5]);
  
  //GPU
  tft.setTextColor(GREEN, BLACK);
  drowString(210, 37, values[6]);
  drowString(285, 37, values[7]);
  drowString(348, 37, values[8]);
  drowString(213, 60, values[9]);
  drowString(318, 60, values[10]);

  //MEM
  tft.setTextColor(RED, BLACK);
  drowString(30, 125, values[11]);
  drowString(93, 125, values[12]);

  //POWER
  tft.setTextColor(YELLOW, BLACK);
  drowString(177, 125, values[14]);
  drowString(248, 125, values[13]);

  //DRIVERS
  tft.setTextColor(PURP, BLACK);
  drowString(60, 165, values[15]); 
  drowString(115, 165, values[18]);
  drowString(60, 187, values[16]); 
  drowString(115, 187, values[19]);
  drowString(60, 209, values[17]); 
  drowString(115, 209, values[20]);


  //NETWORK
  tft.setTextSize(1);
  tft.setTextColor(ORANGE2, BLACK);
  drowString(319, 105, values[21]);
  drowString(319, 130, values[22]);

  //TAIM & DATE
  tft.setTextColor(GREY, BLACK); 
  tft.setTextSize(3);
  drowString(298, 170, values[0]);
  tft.setTextSize(2);
  drowString(294, 203, values[1]);
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
