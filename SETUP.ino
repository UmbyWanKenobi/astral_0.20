void init_TFT() {
  static uint16_t g_identifier;
  tft.reset();                 //we can't read ID on 9341 until begin()
  g_identifier = tft.readID(); //
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
    Serial.println( F("RTC is NOT running!"));
  } else {
    Serial.println( F("RTC is running!"));
  }
}
void init_GPS() {
  
  gps_port.begin(9600); // 9600 is the default for NEO-6M

  //setup satellites signal
 pinMode( LED_GPS0, OUTPUT );  // In attesa del fix dai satelliti
  pinMode( LED_GPS1, OUTPUT );  // Sistema aggangiato
   uint16_t lastToggle = millis();
  // Turn off the led until a satellite signal
  digitalWrite( LED_GPS0, LOW );
  digitalWrite( LED_GPS1, LOW );

  Serial.println( F("Waiting GPS data...") );
  while (gps.available( gps_port )) {
    fix = gps.read();  // a new fix structure has been parsed
    if ((uint16_t) millis() - lastToggle > 500) {
        lastToggle += 500;
        digitalWrite( LED_GPS0, !digitalRead(LED_GPS0) );
      }
  }
 
   
  
  DateTime now = RTC.now();
if (fix.dateTime.seconds!=  now.second()  && fix.valid.time)
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
  for (int thisNote = 0; thisNote < 5; thisNote++) {

    int noteDuration = 1000 / noteDurations[thisNote];
    int pauseBetweenNotes = noteDuration * 1.30;

    tone(BUZZER_PIN, melody[thisNote], noteDuration);
    delay(pauseBetweenNotes);
    noTone(8);
  }
}

void MOTORE () {
  
  // set port pin to output PL0 PL1 PL2 PL3 PL4 PL5
  //             76543210
  DDRL = DDRL | B00111111;
  // set port pin to output PA3
 
  DDRA = DDRA | B00001000;
  Timer3.initialize(DELAY * 1000000 );   
  Timer3.attachInterrupt(m1);  Timer3.stop();
  Serial.begin(115200);
  Serial.print("Tempo "); Serial.print ( TEMPO ); Serial.println (" secondi");
  Serial.print("Passi "); Serial.println ( COUNT );
  Serial.print("frequenza "); Serial.println ( float(1/DELAY),4 );
  Serial.print("Tempo stimato "); Serial.print ( float(( COUNT ) * ( DELAY  )) / 60  ); Serial.println (" minuti");
  
  
  MOTOR_ENGAGE;
  SPIN_CLOCK;
  
}
