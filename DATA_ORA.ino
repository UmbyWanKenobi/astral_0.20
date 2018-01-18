void Data_Ora () {

  while (gps.available( gps_port )) {
    fix = gps.read();  // Nuova lettura del GPS
    plot();
    PRINT_INFO();
    ESC();
  }
}
void PRINT_INFO() {
//timer=millis();
  byte flag;      // Flag del dato richiesto: 0 per la data, 1 per UTC, 2 per l'ora locale,
  // 3 per JDN, 4 per l'ora siderale locale
 
  flag = 0; // DATA
  tft.setCursor(5, 70);
  tft.print(ReadTimeDate(flag));
  flag = 3;  //  JULIAN DAY
  tft.setCursor(210, 70);
  tft.print(ReadTimeDate(flag));

  flag = 1;  // ORA LOCALE
  tft.setCursor(50, 130); 
  tft.print(ReadTimeDate(flag));

  flag = 2;  // ORA UTC
  tft.setCursor(50, 150);
  tft.print(ReadTimeDate(flag));

  tft.setCursor(280, 150);
  tft.print(TSL());

  flag = 4;  // ORA SIDERALE LOCALE
  tft.setCursor(280, 130);
  tft.print(ReadTimeDate(flag));


  flag = 0; //LATITUDINE  
  
  tft.setCursor (210 , 90);
  tft.print (ReadLatLong(flag));


  flag = 1; // LONGITUDINE
  
  tft.setCursor (210, 110);
  tft.print (ReadLatLong(flag));

  // PRESSIONE ATMOSFERICA
  
  tft.setCursor(210, 170);
  tft.print(MPL.readPressure() / 100);

  // TEMPERATURA DELL'ARIA
 
  tft.setCursor(210, 190);
  tft.print(MPL.readTemp());
  
  // QUOTA BAROMETRICA
  
  tft.setCursor(210, 210);
  tft.print(MPL.readAltitude() / 131.23);
  int BatteryS = analogRead(BATTERY);
 float Volt = BatteryS * (4.64 / 1023.0) * 2;
 spln (Volt);
 //spln (millis()-timer);
}


void mask(){
  tft.setFont();
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor (5, 70);
  tft.print (F("DATA: "));
  tft.setCursor (5 , 90);
  tft.print (F("Latitudine:"));
  tft.setCursor (5, 110);
  tft.print (F("Longitudine:"));
  tft.setCursor (5, 130);
  tft.print (F("LMT:             SMT:"));
  tft.setCursor (5, 150);
  tft.print (F("UTC:             LUX:"));
  tft.setCursor(5, 170);
  tft.print(F("Pressione:               ettopascal"));
   tft.setCursor(5, 190);
  tft.print(F("Temperatura:             celsius"));
  tft.setCursor(5, 210);
  tft.print(F("Quota barom.:            metri s.l.m."));
  
}
String ReadLatLong(byte flag) {
  switch (flag) {
    case 0:  // LATITUDINE
      /*
            sprintf( ccc, "Lat.:  %02d\°%02d\'%02d.%04d\"   \(%02d.%06lu\)", LATITUDE.DD,
                   LATITUDE.MM, LATITUDE.SS,
                   (int)(LATITUDE.ddSS * 10000) % 10000, LATITUDE.DD, (unsigned long)(LATITUDE.mantissa * 1000000 ) % 1000000);
      */
      sprintf( ccc, "%02d%c%02d\'%02d.%04d\" ", LATITUDE.DD, 248,
               LATITUDE.MM, LATITUDE.SS,
             (int)(LATITUDE.ddSS * 10000) % 10000);
      return (ccc);
           break;
    case 1:  // LONGITUDINE

      sprintf( ccc, "%02d\h%02d\'%02d.%04d\" ", LONGITUDE.HH,
               LONGITUDE.MM, LONGITUDE.SS, (int)(LONGITUDE.ddSS * 10000) % 10000);
      return (ccc);
      break;
  }

}

void plot() {

  if (!fix.valid.location) {                        // Se non ci sono satelliti aggangiati
  cbi (PORTC, PIN6); // Spegni il LED
    sbi (PORTA, PINA1);   // Accendi il LED del disasccoppiamento
    spln();
    spln( F("-----------------------------------") );
    sp  ( F("|-------------- Stato ------------>") );
    if (fix.valid.status)
      sp  ( fix.status );
    spln();
    spln( F(" |") );
    spln( F("|---- In attesa dell'aggangio ----|") );
    spln( F("-----------------------------------") );
    spln();
  } else {
    cbi (PORTA, PINA1);        // Spengi il LED di attesa
sbi (PORTC, PIN6);      // Accendi il LED se ci sono satelliti aggangiati

/*     
      if (fix.valid.time ) {
DateTime now = RTC.now();
  spln ("Sincing RTC");
     RTC.adjust(DateTime(fix.dateTime.year, fix.dateTime.month, fix.dateTime.date,
                        fix.dateTime.hours, fix.dateTime.minutes, fix.dateTime.seconds));
   
      TIME.hour = fix.dateTime.hours;
      TIME.minute = fix.dateTime.minutes;
      TIME.second = fix.dateTime.seconds;
      TIME.date = fix.dateTime.date;
      TIME.month = fix.dateTime.month;
      TIME.year = fix.dateTime.year;
      
      }
    */
  
    LATITUDE.dec = fix.latitude();
    LATITUDE.DD = (int)LATITUDE.dec;
    LATITUDE.mantissa = LATITUDE.dec - LATITUDE.DD ;
    LATITUDE.MM  = (int)(LATITUDE.mantissa * 60);
    LATITUDE.SS = (int)((LATITUDE.mantissa * 60 - LATITUDE.MM) * 60);
    LATITUDE.ddSS = (LATITUDE.mantissa * 3600 - (int)(LATITUDE.mantissa * 3600));
    LATITUDE.dir = "N";


    LONGITUDE.dec = (fix.longitude());
    LONGITUDE.HH = (int)LONGITUDE.dec;
    LONGITUDE.mantissa = LONGITUDE.dec - LONGITUDE.HH ;
    LONGITUDE.MM  = (int)(LONGITUDE.mantissa * 60);
    LONGITUDE.SS = (int)((LONGITUDE.mantissa * 60 - LONGITUDE.MM) * 60);
    LONGITUDE.ddSS = (LONGITUDE.mantissa * 3600 - (int)(LONGITUDE.mantissa * 3600));
    LONGITUDE.dir = "E";
 
  }
}
