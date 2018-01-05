
void MENU_BUTTON() {


  for (uint8_t row = 0; row < 2; row++) {
    for (uint8_t col = 0; col < 2; col++) {
      MENU[col + row * 2].initButton(&tft, MENU_X + col * (MENU_W + MENU_SPACING_X),
                                     MENU_Y + row * (MENU_H + MENU_SPACING_Y), // x, y, w, h, outline, fill, text, corner radius, char dim.
                                     MENU_W, MENU_H, GREEN, NAVY, WHITE,
                                     menu_strings[col + row * 2], MENU_TEXTSIZE, RADIUS, 7, 12);

    }
  }
  MENU[5].initButton(&tft, 50,
                     290, // x, y, w, h, outline, fill, text, corner radius, char dim.
                     90, 50, GREEN, BLUE, WHITE,
                     menu_strings[4], MENU_TEXTSIZE, RADIUS, 7, 12);
}

void touch  (int i)
{

  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
    p.x = (map(p.x, TS_MINX, TS_MAXX, tft.width(), 0) - 480) * -1;
    p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
  }
  // go thru all the buttons, checking if they were pressed
  if ((MENU[i].contains(p.x, p.y)) && p.x > 10)
  {
    MENU[i].press(true);  // tell the button it is pressed
    tone(BUZZER_PIN, NOTE_E4, 50);
    MENU[i].drawButton(true);  // draw invert!
  } else {
    MENU[i].press(false);  // tell the button it is NOT pressed
  }

}

void ESC () {
  touch (5);
  if (MENU[5].isPressed() == true) {
    ASTRO_MENU();
  }

}

void ASTRO_MENU() {
  Init_Splash_Draw ();
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
        MENU[b].drawButton();  // draw normal
      }



      if (MENU[b].isPressed() == true) {
        switch (b) {
          case 0:
            Init_Splash_Draw ();
            tft.setFont(&FreeSans12pt7b);
            MENU[5].drawButton();
            while (ESCAPE != true) {
              Data_Ora();

            }
            ESCAPE = false;
            break;
          case 1:
            //      while (ESCAPE != true) {
            // calibrazione();
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
