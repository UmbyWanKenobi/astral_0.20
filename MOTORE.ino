
void m1 () {
  MOTOR_TOGGLE;
  LEDPIN_TOGGLE;
  IMPULSO ++;
}
void STEPPER() {
  
  IMPULSO=0;
  timer=millis();
 spln (timer);
  Timer3.start();
  while((millis() -timer) < (TEMPO * 1000)) {
//    
      }

  Timer3.stop();
  spln (IMPULSO);
  sp("Fine in ");spln (millis() -timer);
sp("scarto "); spln( IMPULSO - COUNT );
spln((360 / COUNT)*(IMPULSO - COUNT),4);
 
}
