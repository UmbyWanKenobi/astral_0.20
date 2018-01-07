
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
  1 DS3234 orologio elettronico con quarzo stabilizzato in termperatura
  1 display TFT a colori da 3,5" Kuman UNO R3 3.5
  1  17HS19-1684S-PG100 https://goo.gl/YhCh6W
*/
#define DEBUG   // Non commentare per test
#ifdef DEBUG                    // Per Uso  
#define sp Serial.print
#define spln Serial.println     //   di
#endif                          //  debug

#define build 0                 // Numero versione
#define revision 22             // Numero di revisione
#include "note.h"               // Per il beep
#include <Adafruit_GFX.h>       // Libreria hardware del display
#include <TouchScreen.h>        //                per il touchscreen 
#include <MCUFRIEND_kbv.h>      // Primitive di testo
#include <SPI.h>                // necessaria per l'RTC
#include <RTClib.h>
#include <Fonts/FreeSansBold12pt7b.h> // Font 
#include <Fonts/FreeSans12pt7b.h>     // Font
#include <NMEAGPS.h>            // Parser GPS
#include <TimerThree.h>         // Controllo PWM motore
NMEAGPS gps;
gps_fix fix;

MCUFRIEND_kbv tft;
RTC_DS3234 RTC(53);


#define gps_port Serial1       // Porta Serial del GPS
#define SCATTO   27            // fotoaccoppiatore per lo scatto remoto CNY 75B
//#define SINISTRA 10
//#define BOLLA     9
//#define DESTRA    8
#define LED_GPS0   23         // LED di stato GPS inattivo
#define LED_GPS1   31         // LED di stato GPS attivo
#define BUZZER_PIN 17         // PIN del buzzer
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))     // Imposta il bit allo stato basso
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))     // Imposta il bit allo stato alto
#endif
#ifndef tbi
#define tbi(sfr, bit) (_SFR_BYTE(sfr) ^= _BV(bit))     // inverte lo stato del bit
#endif
#define MOTOR_ENGAGE       cbi (PORTL, PINL1);   // PORTL &= ~(1<<1); //   PIN digitale 48
#define MOTOR_DISENGAGE    sbi (PORTL, PINL1);   // PORTL |=  (1<<1); //   Se alto spegne il motore
#define SPIN_CLOCK         cbi (PORTL, PINL3);   // PORTL &= ~(1<<3); //   PIN digitale 46
#define SPIN_ANTICLOCK     sbi (PORTL, PINL3);   // PORTL |=  (1<<3); //   Se alto gira in senso antiorario
#define MOTOR_TOGGLE       tbi (PORTL, PINL5);   // PINL  |= 1<<5;    //   scambia lo stato del motore (PIN digitale 44)
#define LEDPIN_TOGGLE      tbi (PORTA, PINA3);   // PINA  |= 1<<3;    //   scambia lo stato del LED    (PIN digitale 25)


#define YP A2  // PIN analogico del touch
#define XM A3  // PIN analogico del touch
#define YM 8   // PIN digitale  del touch
#define XP 9   // PIN digitale  del touch
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define TS_MINX 130
#define TS_MAXX 905

#define TS_MINY 75
#define TS_MAXY 930
#define MINPRESSURE 10
#define MAXPRESSURE 1000

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

#define pi 3.1415926535898      // PI GRECO
#define JDunix 2440587.5        // Data giuliana a mezzanotte del 1/1/1970
int TZ = 1;                     // Fuso orario locale (Europa centrale, Berlino, Roma)
int DST = 0;                    // Daylight Saving Time -- Ora Legale


char ccc [26];
int yr, mo, dy, hr, mn, se, dw, uxt, osec = -1;
bool ESCAPE = false;

const int Melodia[] = { NOTE_D7, NOTE_E7, NOTE_C7, NOTE_C6, NOTE_G6 };  // Musichina
const int TempoMusicale[] = {  9, 7, 5, 3, 1 };                         // Tempo musicale: 4 = un quarto, 8 = un ottavo0, etc.:

uint32_t timer;
float IMPULSO;
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


  init_TFT();                   // Abilita lo schermo
  init_RTC();                   // Abilita l'orologio in tempo reale
  init_GPS();                   // Abilita il GPS
  MENU_BUTTON();                // Crea i bottoni
  Musichina ();                 // Suono iniziale

}

void loop() {
  ASTRO_MENU();

  //Data_Ora();
}



