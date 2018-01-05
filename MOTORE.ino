
void m1 () {
  MOTOR_TOGGLE;
  LEDPIN_TOGGLE;
  pulse ++;
}
void STEPPER() {
  pulse = 0;
  timer = millis();
  Serial.println (timer);
  Timer3.start();
  while ((millis() - timer) < (TEMPO * 1000)) {
    //
  }

  Timer3.stop();
  cbi(DDRA, PINA1);
  Serial.println (pulse);
  Serial.print("Fine in "); Serial.println (millis() - timer);
  Serial.print("scarto "); Serial.println( pulse - COUNT );
  Serial.println((360 / COUNT) * (pulse - COUNT), 4);
 
}
