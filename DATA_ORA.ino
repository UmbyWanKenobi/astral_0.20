void Data_Ora () {
  tft.setFont();
  tft.setTextSize(3);
  tft.setCursor(5, 80);
  tft.setTextColor(WHITE, BLACK);
  while (gps.available( gps_port )) {
    fix = gps.read();  // Nuova lettura del GPS
    plot();
    ESC();
  }
}
void PRINT_INFO() {
  tft.setFont();
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  byte flag;      // Flag del dato richiesto: 0 per la data, 1 per UTC, 2 per l'ora locale, 
                  // 3 per JDN, 4 per l'ora siderale locale
                  // Seleziona il dato richiesto 5 per la latitudine, 6 per la longitudine

  flag = 0; // DATA
  tft.setCursor(5, 100);
  tft.print(ReadTimeDate(flag));

  flag = 1;  // ORA LOCALE
  tft.setCursor(220, 100);
  tft.print(ReadTimeDate(flag));

  flag = 2;  // ORA UTC
  tft.setCursor(5, 120);
  tft.print(ReadTimeDate(flag));


  flag = 4;  // ORA SIDERALE LOCALE
  tft.setCursor(220, 120);
  tft.print(ReadTimeDate(flag));

  flag = 3;  //  JULIAN DAY
  tft.setCursor(5, 190);
  tft.print(ReadTimeDate(flag));

  flag = 5;
  tft.setCursor (5 , 140);
  tft.print (F("Latitudine:"));
  tft.setCursor (220 , 140);
  tft.print (ReadLatLong(flag));


  flag = 6;
  tft.setCursor (5, 160);
   tft.print (F("Longitudine:"));
  tft.setCursor (220, 160);
  tft.print (ReadLatLong(flag));
}



String ReadLatLong(byte flag) {
  switch (flag) {
    case 5:  // LATITUDINE
/*
      sprintf( ccc, "Lat.:  %02d\°%02d\'%02d.%04d\"   \(%02d.%06lu\)", LATITUDE.DD,
             LATITUDE.MM, LATITUDE.SS,
             (int)(LATITUDE.ddSS * 10000) % 10000, LATITUDE.DD, (unsigned long)(LATITUDE.mantissa * 1000000 ) % 1000000);
             */
      sprintf( ccc, "%02d%c%02d\'%02d.%04d\" ", LATITUDE.DD,248,
               LATITUDE.MM, LATITUDE.SS,
               (int)(LATITUDE.ddSS * 10000) % 10000);
      return (ccc);
      break;
    case 6:  // LONGITUDINE

      sprintf( ccc, "%02d\h%02d\'%02d.%04d\" ", LONGITUDE.HH,
               LONGITUDE.MM, LONGITUDE.SS, (int)(LONGITUDE.ddSS * 10000) % 10000);
      return (ccc);
      break;
  }

}

void plot() {

  if (fix.valid.location) {
    digitalWrite(LED_GPS0 , LOW);       // Spengi il LED di attesa
    digitalWrite(LED_GPS1 , HIGH);      // Accendi il LED se ci sono satelliti aggangiati
    if (fix.valid.time ) {
    
      TIME.hour = fix.dateTime.hours;
      TIME.minute = fix.dateTime.minutes;
      TIME.second = fix.dateTime.seconds;
      TIME.date = fix.dateTime.date;
      TIME.month = fix.dateTime.month;
      TIME.year = fix.dateTime.year;
    }

    LATITUDE.dec = fix.latitude();
    LATITUDE.DD = (int)LATITUDE.dec;
    LATITUDE.mantissa = LATITUDE.dec - LATITUDE.DD ;
    LATITUDE.MM  = (int)(LATITUDE.mantissa * 60);
    LATITUDE.SS = (int)((LATITUDE.mantissa * 60 - LATITUDE.MM) * 60);
    LATITUDE.ddSS = (LATITUDE.mantissa * 3600 - (int)(LATITUDE.mantissa * 3600));
    LATITUDE.dir = "N";
    

    LONGITUDE.dec = (fix.longitude());//11.3851413;
    LONGITUDE.HH = (int)LONGITUDE.dec;
    LONGITUDE.mantissa = LONGITUDE.dec - LONGITUDE.HH ;
    LONGITUDE.MM  = (int)(LONGITUDE.mantissa * 60);
    LONGITUDE.SS = (int)((LONGITUDE.mantissa * 60 - LONGITUDE.MM) * 60);
    LONGITUDE.ddSS = (LONGITUDE.mantissa * 3600 - (int)(LONGITUDE.mantissa * 3600));
    LONGITUDE.dir = "E";
    
    PRINT_INFO();
  } else {                        // Se non ci sono satelliti aggangiati
    digitalWrite(LED_GPS1 , LOW); // Spegni il LED
 digitalWrite(LED_GPS0 , HIGH);   // Accendi il LED del disasccoppiamento
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
  }
}
