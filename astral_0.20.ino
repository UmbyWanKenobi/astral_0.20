
/*
   ASTROINSEGUITORE versione 02
   (c) 2016 Lafayette Software Development for Il Poliedrico
   written by Umberto Genovese
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
  //
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  //
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
  //
  Hardware richiesto:
  1 Motherboard Arduino Mega 2560 R3 ATmega2560-16AU
  1 GY-GPS6MV1 ricevitore GPS per l'altezza del Polo Celeste (asse polare)
  1 MPU6050 accellerometro-giroscopio 6 gradi di libertà
  1 MPL3115A2 barometro - altimetro - termometro
  1 HMC5983 bussola eletttronica
  1 DS3234 orologio elettronico con quarzo stabilizzato in termperatura
  1 Controller RepRap con monitor 12864 (seriale), buzzer, encoder, letttore SD e pulsante integrati
*/
#define DEBUG   // Non commentare per test
#ifdef DEBUG                    // Per Uso  
#define sp Serial.print
#define spln Serial.println     //   di
#endif                          //  debug

#define build 0
#define revision 21
#include "note.h"
#include <Adafruit_GFX.h>// Hardware-specific library
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <SPI.h>          // for RTC
#include <RTClib.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <NMEAGPS.h>
#include <TimerThree.h>
NMEAGPS gps;
gps_fix fix;

MCUFRIEND_kbv tft;
RTC_DS3234 RTC(53);


#define gps_port Serial1
#define SCATTO   27      // CNY 75B
//#define SINISTRA 10
//#define BOLLA     9
//#define DESTRA    8
#define LED_GPS0   23
#define LED_GPS1   31
#define BUZZER_PIN 17

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit));
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit));
#define tbi(sfr, bit) (_SFR_BYTE(sfr) ^= _BV(bit));
#define MOTOR_ENGAGE        sbi(DDRL, PING0); //   digital
#define MOTOR_DISENGAGE     cbi(DDRL, PING0);  //    PIN 40
#define SPIN_CLOCK          sbi(DDRL, PINA3); //   digital
#define SPIN_ANTICLOCK      cbi(DDRL, PINA3);  //    PIN 25
#define MOTOR_OFF           PORTL &= ~(1<<4); //   digital
#define MOTOR_ON            PORTL  |= (1<<4);  //    PIN 45
#define MOTOR_TOGGLE  PINL |= 1<<5;//  tbi(DDRL, PINL5); //switch MOTORPIN state (digital PIN  44)
#define LEDPIN_TOGGLE   PINA |= 1<<3;     //switch  LEDPIN  state (digital PIN 25)

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define TS_MINX 130
#define TS_MAXX 905

#define TS_MINY 75
#define TS_MAXY 930
#define MINPRESSURE 10
#define MAXPRESSURE 1000
// Assign human-readable names to some common 16-bit color values:
#define BLACK       0x0000      /*   0,   0,   0 */
#define NAVY        0x000F      /*   0,   0, 128 */
#define DARKGREEN   0x03E0      /*   0, 128,   0 */
#define DARKCYAN    0x03EF      /*   0, 128, 128 */
#define MAROON      0x7800      /* 128,   0,   0 */
#define PURPLE      0x780F      /* 128,   0, 128 */
#define OLIVE       0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY   0xC618      /* 192, 192, 192 */
#define DARKGREY    0x7BEF      /* 128, 128, 128 */
#define BLUE        0x001F      /*   0,   0, 255 */
#define GREEN       0x07E0      /*   0, 255,   0 */
#define CYAN        0x07FF      /*   0, 255, 255 */
#define RED         0xF800      /* 255,   0,   0 */
#define MAGENTA     0xF81F      /* 255,   0, 255 */
#define YELLOW      0xFFE0      /* 255, 255,   0 */
#define WHITE       0xFFFF      /* 255, 255, 255 */
#define ORANGE      0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define PINK        0xF81F


Adafruit_GFX_Button MENU[7];
char *menu_strings[5] =
{ "Data/ora", "Calibrazione", "Start", "Reset", "Uscita"};
/******************* UI details */
#define MENU_X 120
#define MENU_Y 120
#define MENU_W 200
#define MENU_H 60
#define MENU_SPACING_X 30
#define MENU_SPACING_Y 30
#define MENU_TEXTSIZE 1
#define RADIUS 4

#define pi 3.1415926535898   // PI GRECO
#define JDunix 2440587.5              // Data giuliana a mezzanotte del 1/1/1970
int TZ = 1;
int DST = 0;


char ccc [26];
int yr, mo, dy, hr, mn, se, dw, uxt, osec = -1;
bool ESCAPE = false;

const int melody[] = {
  NOTE_D7, NOTE_E7, NOTE_C7, NOTE_C6, NOTE_G6 //Musichina
};
const int noteDurations[] = {  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  9, 7, 5, 3, 1
};

uint32_t timer;
float pulse;
float TEMPO = 60;
float DEMOLTIPLICA = 1;
float STEP = 400  ;
float DIVISORE = 128 ;
float COUNT = STEP * DIVISORE * DEMOLTIPLICA;
float DELAY = (TEMPO / COUNT);


typedef struct times {
  uint8_t year;
  uint8_t month;
  uint8_t date;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};
typedef struct latitude {
  uint8_t DD;
  uint8_t MM;
  uint8_t SS;
  float ddSS;
  String dir;
  float mantissa;
  float dec;
};
typedef struct longitude {
  uint8_t HH;
  uint8_t MM;
  uint8_t SS;
  float ddSS;
  String dir;
  float mantissa;
  float dec;
};

times TIME;
latitude LATITUDE;
longitude LONGITUDE;



void setup() {

#ifdef DEBUG                    // Per Uso  
  Serial.begin(115200);         //   di
#endif                          //  debug
   

  init_TFT();
  init_RTC();
  init_GPS();
  MENU_BUTTON();
 Musichina ();
   
}

void loop() {
  ASTRO_MENU();
    
  //Data_Ora();
  }



