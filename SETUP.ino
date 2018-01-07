void init_TFT() {
  static uint16_t g_identifier;
  tft.reset();
  g_identifier = tft.readID();
  tft.begin(g_identifier);
  tft.setRotation(1);
  tft.setTextWrap(true);
  tft.fillScreen(BLACK);
}

void init_RTC() {
  SPI.begin();
  RTC.begin();
  RTC.enableOscillator(true, true, 3);
  if ( ! RTC.isrunning()) {
    spln( F("RTC non funziona!"));
  } else {
    spln( F("RTC in funzione"));
  }
}
void init_GPS() {

  gps_port.begin(9600); // Velocità della porta per il NEO-6M

  //setup satellites signal
  pinMode( LED_GPS0, OUTPUT );  // In attesa del fix dai satelliti
  pinMode( LED_GPS1, OUTPUT );  // Sistema aggangiato
  uint16_t lastToggle = millis();
  digitalWrite( LED_GPS0, LOW );
  digitalWrite( LED_GPS1, LOW );
#ifdef DEBUG                                      // Per Uso  
  spln( F("In attesa del GPS ...") ); //   di DEBUG
#endif

  while (gps.available( gps_port )) {
    fix = gps.read();  // a new fix structure has been parsed
    if ((uint16_t) millis() - lastToggle > 500) {
      lastToggle += 500;
      digitalWrite( LED_GPS0, !digitalRead(LED_GPS0) );
    }
  }



  DateTime now = RTC.now();
  if (fix.dateTime.seconds !=  now.second()  && fix.valid.time)
  {
    spln ("Sincing RTC");
    RTC.adjust(DateTime(fix.dateTime.year, fix.dateTime.month, fix.dateTime.date,
                        fix.dateTime.hours, fix.dateTime.minutes, fix.dateTime.seconds));
  }
  digitalWrite(LED_GPS0 , LOW);
  digitalWrite(LED_GPS1 , HIGH); // Then there are satellites, the LED switch ON

}

void Init_Splash_Draw () {
  String str;
  uint16_t pixel;
  uint16_t dx, rgb, n, wid, ht, hds, x, y;
  hds = 57;
  wid = tft.width();
  ht = tft.height();
  x = 1;
  y = 1;
  tft.fillScreen(BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(WHITE);
  dx = wid / 32;
  for (n = 0; n < 32; n++) {
    rgb = n * 7;
    rgb = tft.color565(rgb, rgb, rgb);
    tft.fillRect(n * dx, 0, dx, 60, rgb & RED);
  }
  tft.drawRoundRect(0, 0, wid, 60, 5, GREEN);
  tft.setTextSize(1);
  str = "ASTROINSEGUITORE v. ";
  str += build;
  str += ".";
  str += revision;
  spln (str);
  tft.setCursor((wid - (26 * 12)) / 2, 36);
  tft.print( str );


  str =  "https://ilpoliedrico.com";
  tft.setCursor(((wid + 60) - (24 * 12)) / 2, 305);
  tft.print( str );
  //pixel = tft.readPixel(0, 0);
#if defined(MCUFRIEND_KBV_H_)
  extern const uint8_t header[];
  tft.setAddrWindow( x, y, x +  hds - 1, y + hds - 1);
  tft.pushColors(header, 3249, 1);
#endif
}

void Musichina () {
  for (int nota = 0; nota < 5; nota++) {

    int tempo = 1000 / TempoMusicale[nota];
    int pausa = tempo * 1.40;

    tone(BUZZER_PIN, Melodia[nota], tempo);
    delay(pausa);
    noTone(8);
  }
}

void MOTORE () {

  // abilita i pin PL0 PL1 PL2 PL3 PL4 PL5 della porta L in uscita
  //             76543210
  DDRL = DDRL | B00111111;
  //  abilita i pin PA3 della porta A in uscita
  DDRA = DDRA | B00001000;
  Timer3.initialize(DELAY * 1000000 );
  Timer3.attachInterrupt(m1);  Timer3.stop();
  Serial.begin(115200);
  sp("Tempo "); sp ( TEMPO ); spln (" secondi");
  sp("Passi "); spln ( COUNT );
  sp("frequenza "); spln ( float(1 / DELAY), 4 );
  sp("Tempo stimato "); sp ( float(( COUNT ) * ( DELAY  )) / 60  ); spln (" minuti");


  MOTOR_ENGAGE;
<<<<<<< HEAD
  SPIN_ANTICLOCK;

=======
  SPIN_CLOCK;
  
>>>>>>> 6e3e3eb437bad2f01834970fe1b974bb84d2d12e
}
