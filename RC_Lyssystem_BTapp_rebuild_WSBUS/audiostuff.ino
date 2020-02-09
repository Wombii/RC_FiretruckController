

/*

mikro sound module configured as follows:

    engine sound increases in both directions from center
    soundfile scania v8micro
    pos1 (+step*0) is ignition
    pos2 (+step*1) is rev
    pos3 (+step*2) is horn
    pos4 is brake hiss
    pos5 is siren
    pos6 is siren
    pos7 is volup
    pos8 is voldn
    //pos6 reverse beep

    To activate a sound:
    soundControlServo.writeMicroseconds( soundControlServoSound1 + soundStepsize * soundNumber );
    then wait (70ms is tested and works) and return servo value to default value
    soundControlServo.writeMicroseconds( soundControlServoDefault );

      in soundFunction:
      soundControlServo.writeMicroseconds( soundControlServoSound1 + soundStepsize * soundNumber );
      soundControlServoTimestamp = millis();

      in main loop:
      if (millis() - soundControlServoTimestamp > 70)
        soundControlServo.writeMicroseconds( soundControlServoDefault );

    if reverseflag
      reversebeep
      engine sound low value

    if brakeflag
      engine sound neutral
      wasBraking = 1

    else
      if (wasBraking)
        sound brake hiss
        wasBraking = 0
      engine sound = throttleAxis
    
    


*/

void soundModuleCalibrate (void) {
  
  HUMANSERIAL.println("SC");
  delay(1000);
  soundThrottleServo.writeMicroseconds(map(throttleCenter,0,2000,800,2200));
  //HUMANSERIAL.println(map(throttleCenter,0,2000,800,2200));
  soundControlServo.writeMicroseconds(soundControlServoDefault);
  //HUMANSERIAL.println(soundControlServoDefault);
  clicklearn();
//delay(1000);

  soundThrottleServo.writeMicroseconds(map(throttleNeutralUpperLimit,0,2000,800,2200));
  //HUMANSERIAL.println(map(throttleNeutralUpperLimit,0,2000,800,2200));
  //clicklearn();
  click12positionSetup(1200);                            //1200 is the chosen value for the first position
  //HUMANSERIAL.println("c12");
//delay(1000);
  soundThrottleServo.writeMicroseconds(map(throttleFull,0,2000,800,2200));
  //HUMANSERIAL.println(map(throttleFull,0,2000,800,2200));
  //clicklearn();
  click12positionSetup(1200);                            //1200 is the chosen value for the first position
  //HUMANSERIAL.println("c12");
  //delay(1000);
  soundThrottleServo.writeMicroseconds(map(throttleCenter,0,2000,800,2200));
  //HUMANSERIAL.println(map(throttleCenter,0,2000,800,2200));
  

  int value = 1250;                                 //1200 is the chosen value for the first position
  click12positionSetup(value);
  HUMANSERIAL.println(value);
  for(int i=0; i<= 12 ;i++)
  {
    click12positionSetup(value+=soundStepsize);
    HUMANSERIAL.println(value);
  }
}


void clicklearn(){                                //Bridge LRN pins for half a second
  pinMode(soundLearnpin,OUTPUT);
  delay(500);
  pinMode(soundLearnpin,INPUT);
  delay(500);
  
}

//
void click12positionSetup(int value){                  //"Click" the 12 position rotary switch button
  soundControlServo.writeMicroseconds(value);
  HUMANSERIAL.print(value);
  delay(400);                                        //400 ms seems like a safe minimum
  soundControlServo.writeMicroseconds(soundControlServoDefault);
  HUMANSERIAL.print(soundControlServoDefault);
  delay(700);
  HUMANSERIAL.println("c12");
}

//Play a sound by emulating the 12 position encoder and set a time to return to center.
void soundSelector(int soundNumber){
  soundControlServo.writeMicroseconds( soundControlServoSound1 + soundStepsize * (soundNumber-1) );
  soundControlServoTimestamp = millis();
}

//Return 12 position encoder emulation to center after 70 ms.
void resetSoundSelector()
{
  if (millis() - soundControlServoTimestamp > 70)
     soundControlServo.writeMicroseconds( soundControlServoDefault );
}
