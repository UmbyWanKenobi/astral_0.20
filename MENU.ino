
void MENU_BUTTON() {                      // I pulsanti del menÃ¹ principale
  for (uint8_t row = 0; row < 2; row++) {
    for (uint8_t col = 0; col < 2; col++) {
      MENU[col + row * 2].initButton(&tft, MENU_X + col * (MENU_W + MENU_SPACING_X),
                                     MENU_Y + row * (MENU_H + MENU_SPACING_Y), // x, y, w, h, outline, fill, text, corner radius, char dim.
                                     MENU_W, MENU_H, GREEN, NAVY, WHITE,
                                     menu_strings[col + row * 2], MENU_TEXTSIZE, RADIUS, 7, 12);

    }
  }
  // Il pulsante di uscita
  MENU[5].initButton(&tft, 50,
                     290, // x, y, w, h, outline, fill, text, corner radius, char dim.
                     90, 50, GREEN, BLUE, WHITE,
                     menu_strings[4], MENU_TEXTSIZE, RADIUS, 7, 12);
}

void touch  (int i) {                           // Routine touchscreen
#ifdef DEBUG                      // Per Uso  
  digitalWrite(13, HIGH);         //   di
#endif                            //  debug
  TSPoint p = ts.getPoint();
 #ifdef DEBUG                      // Per Uso  
  digitalWrite(13, HIGH);         //   di
#endif                            //  debug

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)  {
    p.x = (map(p.x, TS_MINX, TS_MAXX, tft.width(), 0) - 480) * -1;
    p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
  }
  // Controlla che qualche pulsante sia stato premuto
  if ((MENU[i].contains(p.x, p.y)) && p.x > 10) {
    MENU[i].press(true);                        // Se un pulsante viene premuto
   // tone(BUZZER_PIN, NOTE_E4, 50);              // Suona una nota
    MENU[i].drawButton(true);                   // Inverte il suo colore
  } else {
    MENU[i].press(false);                       // Altrimenti non fa niente
  }

}

void ESC () {                                   // Routine pulsante di uscita2
  touch (5);
  if (MENU[5].isPressed() == true) {
    ASTRO_MENU();
  }

}

void ASTRO_MENU() {         // Routine menÃ¹ principale
  Init_Splash_Draw ();      // la banda in cima
  tft.setFont(&FreeSansBold12pt7b);
  for (uint8_t row = 0; row < 2; row++) {
    for (uint8_t col = 0; col < 2; col++) {
      MENU[col + row * 2].drawButton();
    }
  }
  while (true) { 
    for (uint8_t b = 0; b < 4; b++) {
      touch (b);
      if (MENU[b].justReleased()) {
        MENU[b].drawButton();  
      }



      if (MENU[b].isPressed() == true) {
        switch (b) {
          case 0:
            Init_Splash_Draw ();
            tft.setFont(&FreeSans12pt7b);
            MENU[5].drawButton();
             mask();
            while (ESCAPE != true) {
              Data_Ora();

            }
            ESCAPE = false;
            break;
          case 1:
                 while (ESCAPE != true) {
           TSL();
             }
            ESCAPE = false;
            break;
          case 2:
            //       while (ESCAPE != true) {
            // start();
            break;
          case 3:
            //      while (ESCAPE != true) {
            // reset();
            break;
          default:
            ;
        }
      }
    }
  }
}

