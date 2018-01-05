
String ReadTimeDate(byte flag) {

  LMT();
  DateTime now = RTC.now();

  switch (flag) {
    case 0: //   DATA
      sprintf(ccc, "%02d/%02d/%04d", dy, mo, yr);
      return (ccc);
      break;
    case 1:  // ORA LOCALE

      sprintf(ccc, "LMT:%02d:%02d:%02d", hr, mn, se );
      return (ccc);
      break;
    case 2: //  ORA UTC
      sprintf(ccc, "UTC:%02d:%02d:%02d", (int)(now.hour()), (int)(now.minute()), (int)(now.second()) );
      return (ccc);
      break;
    case 3:  // JULIAN DAY

      sprintf (ccc, "JD: %7lu.%05lu", JD0(), JD1());

      return (ccc);
      break;
    case 4:
      return (OraSiderale());
      break;

  }
}
unsigned long JD0 () {
  DateTime now = RTC.now();
  return (now.unixtime() / 86400 ) + JDunix + UTC() ;
}

float UTC() {
  DateTime now = RTC.now();
  return ((float)( now.hour() / 24.0f ) + (float)( now.minute() / 1440.0f ) + (float)( now.second() / 86400.0f ) );

}
unsigned long JD1() {
  return (unsigned long)( ( UTC() + .5 ) * 100000.0f ) % 100000;
}

void LMT() {
  DateTime now = RTC.now();

  if ( IsDST() == true ) {
    DST = 1;
  } else {
    DST = 0;
  }
  DateTime local (now.unixtime() + (TZ + DST) * 3600 );
  hr = local.hour();
  mn = local.minute();
  se = local.second();
  yr = local.year();
  mo = local.month();
  dy = local.day();
  dw = local.dayOfWeek();
  uxt = local.unixtime();
}


String OraSiderale() {     // Basato su algoritmo di "The United States Naval Observatory (USNO)"
  // http://aa.usno.navy.mil/faq/docs/GAST.php
  //
  // sidereal calculation constants
const float dc = 0.06570982441908;
const float un_sid = 1.00273790935;          // Rapporto tra giorno solare medio e giorno siderale all'equinozio d'inverno
const float Cost_G = 6.697374558;

const float lc = 0.0497958000000001;
const float nc = -0.0159140999999998;

const float JD2000 = 2451545.0;
const float giorno_siderale = 23.9344699;        // Lunghezza del giorno siderale (23:56:04.9)
  double GST, LST, utc;                 // Tempo siderale di Greenwich e locale
  int dh, dm, ds;                       // Espressione del tempo locale siderale ((HH:MM:SS)



  DateTime now = RTC.now();                          // current time


  GST = Cost_G + (dc * (((now.unixtime()) / 86400L) + .5 )) + (un_sid * 24 * UTC() ); // Tempo siderale di Greenwich in formato decimale
  LST = GST + 24.0 + (float)(11.39 / 360 * giorno_siderale); // Tempo siderale locale stabilito dal GPS in formato decimale
  while (LST >= 24.0 ) {
    LST -= 24.0;  // accordo le 24 ore
  }

  DateTime sidereal = LST;

  dh = int( LST );                                           // ORA LST
  dm = int( (LST - (float)dh) * 60.0 );                      // MINUTI LST
  ds = int( (LST - (float)dh - (float)dm / 60.0) * 3600.0 ); // SECONDI LST
  sprintf(ccc, "LST:%02d:%02d:%02d", dh, dm, ds);
  return (ccc);
}

int GiorniAnno(int y, int m, int d) {
  int GiorniDelMese[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if (y % 4  == 0) {                //
    if (y % 100 != 0) {             //
      GiorniDelMese[1] = 29;        //Calcolo
    }                               //
    else {                          // del
      if (y % 400 == 0) {           //
        GiorniDelMese[1] = 29;      // bisestile
      }                             //
    }                               //
  }                                 //

  int dy = 0;
  for (int i = 0; i < m - 1; i++) {
    dy += GiorniDelMese[i];
  }

  dy += d;
  return dy;                      // Giorni dall'inizio dell'annno
}



boolean IsDST() {
  if (mo < 3 || mo > 11) {
    return false;                 // Gennaio, Febbraio e Dicembre non hanno ora legale
  }
  if (mo > 3 && mo < 11) {
    return true;                  // da Aprile a Ottobre vige l'ora legale
  }
  int previousSunday = dy - dw;
  if (mo == 3) {
    return previousSunday >= 8;  // Trova l'ultima domenica di Marzo
  }
  return previousSunday <= 0;    // e l'ultima domenica di Ottobre per il salto
}


