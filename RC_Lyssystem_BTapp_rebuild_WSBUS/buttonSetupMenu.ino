void buttonSetupCalibration (void){
  //Hold Left turn signal button on interface panel during power up to get here

  //Sets throttle and steering directions, sends radio setup variables to sound module.

  //release button
  //hold full throttle, click indicated button
  //hold left steering, click indicated button
  //click indicated button to finish. Sound module is then updated.

  static byte buttonSetupMenuSelection = 0;
  
  // TODO: neutralUpperOffset, neutralLowerOffset
  //move throttlefull here static?
  HUMANSERIAL.println("calibration");

  switch (buttonSetupMenuSelection)
  {
    case 0:
      while(digitalRead(diagSwitchArray[7]) == LOW)
      {
         //HUMANSERIAL.println(digitalRead(diagSwitchArray[7]));
         HUMANSERIAL.println("release");
         //flash cross
         for(int i=0; i<sizeof(diagRadioLedArray) ; i++)
         {
            digitalWrite(diagRadioLedArray[i],HIGH);//flashingFunctionStartActiveSimple(1,specialTimingArray[1])); 
         } 
         delay(50);
         for(int i=0; i<sizeof(diagRadioLedArray) ; i++)
         {
            digitalWrite(diagRadioLedArray[i],LOW);//flashingFunctionStartActiveSimple(1,specialTimingArray[1]));
         } 
         delay(50);
      }
       
      buttonSetupMenuSelection = 1;
      break;

    case 1:
      HUMANSERIAL.println("full throttle");
      //flash full throttle
      digitalWrite(diagRadioLedArray[0],flashingFunctionStartActiveSimple(1,specialTimingArray[1]));
      //flash diagbutton led 8
      digitalWrite(diagLedArray[8],flashingFunctionStartActiveSimple(1,specialTimingArray[1]));
      
      if(digitalRead(diagSwitchArray[8]) == 0)
      {
        throttleFull = throttleAxis;
  
        if (throttleFull > throttleCenter)  { throttleAxisReversed = 0; }
        else                                { throttleAxisReversed = 1; }
  
        buttonSetupMenuSelection = 2;
        //** specialtiming phase to 255?

      }
      break;

    case 2:
      HUMANSERIAL.println("steering left");
      digitalWrite(diagRadioLedArray[0],LOW);
      digitalWrite(diagLedArray[8],LOW);
      
      //flash left
      digitalWrite(diagRadioLedArray[4],flashingFunctionStartActiveSimple(1,specialTimingArray[1]));
      //flash diagbutton led 9
      digitalWrite(diagLedArray[9],flashingFunctionStartActiveSimple(1,specialTimingArray[1]));

      if(digitalRead(diagSwitchArray[9]) == 0)
      {
        int steeringLeft = steeringAxis;
  
        if (steeringLeft < steeringCenter)  { steeringAxisReversed = 0; }
        else                                { steeringAxisReversed = 1; }
  
        buttonSetupMenuSelection = 3;
        //** specialtiming phase to 255?

      }
      break;

    case 3:
      HUMANSERIAL.println("done?");

      digitalWrite(diagRadioLedArray[4],LOW);
      digitalWrite(diagLedArray[9],LOW);
      
      //flash left
      //digitalWrite(diagRadioLedArray[4],flashingFunctionStartActiveSimple(1,specialTimingArray[1]));
      //flash diagbutton led 9
      digitalWrite(diagLedArray[10],flashingFunctionStartActiveSimple(1,specialTimingArray[1]));

      if(digitalRead(diagSwitchArray[10]) == 0)
      {
        //flash cross 2 seconds

        //eeprom save reversed

        soundModuleCalibrate();
        //ESP32soundModuleCalibrate();

      
        buttonSetupCalibrationActive = 0;
  
        buttonSetupMenuSelection = 0;
        //** specialtiming phase to 255?

      }
      
      
      break;

  }
      
}



#if DISABLED
const byte numberOfButtons = 3 ;
byte buttonStateSaved = 0;
byte buttonStateCounter = 0;

//byte buttonReleased = 0;
byte buttonValueArray[numberOfButtons] = {10 , 33, 57};




void buttonSetupMainAnalogread(void)
{
    const byte buttonDebounceCount = 1;
    const byte buttonHoldCount = 6;
    const byte buttonValueTolerance = 10;

    const byte buttonStateUnpressed = 240;
    
    delay(1);
    byte buttonStateCurrent = analogRead(A0)/4;
    //Serial.println(buttonStateSaved);

    if (buttonStateCurrent < buttonStateUnpressed) // if pressed
    {
        //Serial.println(buttonStateCounter);
        //if (buttonStateCurrent < buttonStateSaved + buttonValueTolerance || buttonStateCurrent > buttonStateSaved -buttonValueTolerance)
        //{
            buttonStateCounter++;
        //}
        //else
        //{
            //buttonStateCounter = 0;
            buttonStateSaved = buttonStateCurrent;
        //}
    }
    else
    {
        //Serial.println("up");
        if (buttonStateCounter > buttonDebounceCount)
        {
            //Serial.println("deb");
            buttonStateCounter = 0;
            for( int i = 0; i < sizeof(buttonValueArray); i++)
                {
                    //Serial.println(i);
                    
                    byte buttonMax = constrain(buttonValueArray[i]+buttonValueTolerance,0,255);
                    byte buttonMin = constrain(buttonValueArray[i]-buttonValueTolerance,0,255);
                    //Serial.println(buttonMax);
                    //Serial.println(buttonMin);
                    //Serial.println(buttonStateSaved);
                    if ( buttonStateSaved >  buttonMin && buttonStateSaved < buttonMax)
                    {
                        //buttonPressed(i);
                        buttonPressed(i);
                    }
                }
        }

    }

    return;



}

void buttonPressed(byte buttonNumber)
{
    Serial.println(buttonNumber);
    switch (buttonNumber){
        case 1:
            togglebit(warningFlash);
            break;
        
        default:
            break;
    }
    return;
}

/*
    switch (buttonNumber)
        case 1:
            togglebit(warningFlash)
            break;
        
        default:
            break;
*/
#endif
