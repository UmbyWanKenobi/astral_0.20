
void m1 () {
  MOTOR_TOGGLE;
  LEDPIN_TOGGLE;
  pulse ++;
}
void STEPPER() {
  
  pulse=0;
  t=millis();
 Serial.println (t);
  Timer3.start();
  while((millis() -t) < (TEMPO * 1000)) {
//    
      }

  Timer3.stop();
  Serial.println (pulse);
  Serial.print("Fine in ");Serial.println (millis() -t);
Serial.print("scarto "); Serial.println( pulse - COUNT );
Serial.println((360 / COUNT)*(pulse - COUNT),4);
 
}
