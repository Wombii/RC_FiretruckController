

//03.11.2018: should add check that arrays are minimum as big as numberofports. weird output errors when ouputarray smaller.
//12.11.2018: added, not tested.

//12.11.2018: note from down there, a secondary 'h' function will never be output as pwmhalfvalue. should I rework?

//29.05.2019: simplified lightswitchingmain for readability and made lightswitchingreadable instead.
//29.05.2019: added simple fading.

void lightSwitchingReadablefadetest(void){
    if (NumberOfPorts > sizeof(outputLedArray))
    {
        Serial.println("Error: NumberOfPorts mismatch");
        return;
    }

    //HUMANSERIAL.print("lights");

    resetTurnSignalPhase();

    // Run through all light pins
    for (byte currentPin = 0; currentPin < NumberOfPorts ; currentPin++)//currentPin < NumberOfPorts; currentPin++)
    {
        byte activeFunctionArray = 0; 
        byte activeFunctionSetting = 0;
        byte functionSetting = 0;
        byte pwmSetting = 0;
        byte selectedPWMsetting = 0;
        byte activeLightValue = 0;
        byte calculatedLightState = 0;

        //HUMANSERIAL.print(" p:");
        //HUMANSERIAL.print(currentPin);

        

        //Load the settings for the first function in the settings array
        functionSetting = settingsArray2[currentPin][0][0]-48; //[function1][value]
        pwmSetting = settingsArray2[currentPin][0][1];
        //If the flag for the function is currently activated, use the settings for that function to control the pin.
        //HUMANSERIAL.print(" FA:");
        //HUMANSERIAL.print(flagsArray[functionSetting]);
        
        if (flagsArray[functionSetting])
        {
            activeFunctionSetting = functionSetting;
            selectedPWMsetting = pwmSetting;
        }
/*
        HUMANSERIAL.print(" s1:");
        HUMANSERIAL.print(functionSetting);
        
        HUMANSERIAL.print(" a1:");
        HUMANSERIAL.print(activeFunction);
        HUMANSERIAL.print(" w1:");
        HUMANSERIAL.print(selectedPWMsetting);

        

 */       
             
        //Load the settings for the second function in the settings array
        functionSetting = settingsArray2[currentPin][1][0]-48; //[function2][value]
        pwmSetting = settingsArray2[currentPin][1][1];
        //If the flag for the function is currently activated, use the settings for that function to control the pin.
        byte secondFunctionActive = flagsArray[functionSetting];
        if (flagsArray[functionSetting])
        {
            activeFunctionSetting = functionSetting;
            selectedPWMsetting = pwmSetting;
            activeFunctionArray = 1;
        }
/*
        HUMANSERIAL.print(" s2:");
        HUMANSERIAL.print(functionSetting);

        HUMANSERIAL.print(" a2:");
        HUMANSERIAL.print(activeFunction);
        HUMANSERIAL.print(" w2:");
        HUMANSERIAL.print(selectedPWMsetting);

*/
        //With the active settings loaded...

        //check the actual light state based on timings and the selected function.

        if (activeFunctionSetting == 8 || activeFunctionSetting == 9) //turnsignal
        {
                    //todo: if previous off, reset?
                    calculatedLightState = flashingFunctionStartActiveSimple(activeFunctionSetting, specialTimingArray[0]);
                    //HUMANSERIAL.print("t:");
                    //HUMANSERIAL.println(calculatedLightState);
        }

        else if (activeFunctionSetting == 5 || activeFunctionSetting == 6) //flashers
        {
                    calculatedLightState = flashingFunction(activeFunctionSetting, timingArray[currentPin][activeTimingArray]);
        }


        else  //If simple lightfunction
        {
                    calculatedLightState = activeFunctionSetting;
        }


        //HUMANSERIAL.print(" Cs:");
        //HUMANSERIAL.print(calculatedLightState);
                    

        //activeLightValue = calculatedLightState;
        if (selectedPWMsetting == 'l')
        {
            if (calculatedLightState)
                activeLightValue = pwmHalfValue;
            else    
                activeLightValue = 0;
        }
        else if (selectedPWMsetting == 'h')
        {
            if (calculatedLightState)
                activeLightValue = 255;
            else    
                activeLightValue = pwmHalfValue;
        }
        else
        {
            if (calculatedLightState)
                activeLightValue = 255;
            else
                activeLightValue = 0;
        }

        //HUMANSERIAL.print(" AV:");
        //HUMANSERIAL.print(activeLightValue);


        
       
        //actualLightState is an analog value 0/halfpwm/1
        


        //add fade.

        /*
        if previous output is less than current output: fade up to wanted state
        if previous output is more than current output: fade down to wanted state
        is previous output is current output: use wanted state.

        use fade curve from fade settings.
        */

        //activeFunctionArray
        //activeLightValue = simplefadertestingxenon(currentPin, activeLightValue);
        activeLightValue = simplefadertestingxenon2(currentPin, activeFunctionArray, activeLightValue);

        //HUMANSERIAL.print("P:");
        //HUMANSERIAL.println(currentPin);

        //******* Write to the LED port *******
        
        //separate write section.
        if (activeLightValue == 0)
            digitalWrite(outputLedArray[currentPin],LOW);
        else if (activeLightValue == 255)
            digitalWrite(outputLedArray[currentPin],HIGH);
        else
            analogWrite(outputLedArray[currentPin],activeLightValue);




    }

}

byte simplefadertestingxenon2(byte currentPin, byte functionNumber, byte wantedOutput)
{
    #define FADEDISABLED  '0'
    #define FADEQUICK     '1'
    #define FADEUP        '2'
    #define FADEDOWN      '3'
    #define FADEBOTH      '4'
    //#define FADEXENON     '5'
    #define FADEXENON     '5'

    //byte fadeSettingsArray[NumberOfPorts][2] = {  {FADEQUICK,0},{FADEXENON,FADEBOTH},{FADEQUICK,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{FADEXENON,FADEDOWN} };
    static byte fadeValueArray[NumberOfPorts] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    //static byte xenonFrame[NumberOfPorts][2] =  {  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0} };
    static byte previousFadeSetting[NumberOfPorts] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    //byte test = 1;
    const byte fadeSpeed = 10;
    const byte fadeUpSlowSpeed = 10;
    
    int lastOutput = 0;
    int calculatedOutput = 0;
    
    byte fadeValue = 0;
    byte fadeSetting = 0;
    
    
    //xenon cooldown? decrease startvalue over time?
    //HUMANSERIAL.println("test");

    fadeValue = fadeSpeed;

    lastOutput = fadeValueArray[currentPin];

    fadeSetting = fadeSettingsArray[currentPin][functionNumber];

    //calculatedOutput = wantedOutput; // else

    
    
//TODO check BUGFIX!!!!!!!!!!!!!!!!!!!!!!!! fix fade down

    if (wantedOutput < lastOutput){
      if(previousFadeSetting[currentPin] >= '5'){ //fixed
        fadeSetting = previousFadeSetting[currentPin]; //xenonFrame[currentPin][functionNumber];
      }
    }
    else
    {
      fadeSetting = fadeSettingsArray[currentPin][functionNumber];
      if (previousFadeSetting[currentPin] < FADEXENON)
        previousFadeSetting[currentPin] = fadeSetting;
    }
    



    //if (currentPin == 5) HUMANSERIAL.print(fadeSetting);

    

  

    
      

    switch (fadeSetting)
    {
      case FADEQUICK:
        previousFadeSetting[currentPin] = FADEQUICK;
        if (wantedOutput > lastOutput)
          calculatedOutput = min(lastOutput + 20, wantedOutput);
        else if (wantedOutput < lastOutput)                         //changed if to else if. untested!
          calculatedOutput = max(lastOutput - 20, wantedOutput);
        else
          calculatedOutput = wantedOutput;
        break;

      case FADEUP:
        previousFadeSetting[currentPin] = FADEUP;
        if (wantedOutput > lastOutput)
          calculatedOutput = min(lastOutput + fadeUpSlowSpeed, wantedOutput);
        else
          calculatedOutput = wantedOutput;
        break;
        
      case FADEDOWN:
        previousFadeSetting[currentPin] = FADEDOWN;
        if (wantedOutput < lastOutput)
          calculatedOutput = max(0.9*lastOutput,wantedOutput);
        else
          calculatedOutput = wantedOutput;
        break;

      case FADEBOTH: //optimize here?
        previousFadeSetting[currentPin] = FADEBOTH;
        if (wantedOutput > lastOutput)
          calculatedOutput = min(lastOutput + fadeUpSlowSpeed, wantedOutput);
        else if (wantedOutput < lastOutput)
          calculatedOutput = max(0.9*lastOutput,wantedOutput);// - fadeValue, wantedOutput);
        else
        {
          calculatedOutput = wantedOutput;
          //previousFadeSetting[currentPin] = 0;
        }
        break;

      case FADEXENON:
      case FADEXENON+1:
      case FADEXENON+2:



        if (wantedOutput > lastOutput)
        {
          //HUMANSERIAL.print("\t up");
          byte flashMax = wantedOutput - (wantedOutput>>2);
          if ((previousFadeSetting[currentPin] == (FADEXENON + 0)) || (previousFadeSetting[currentPin] == 0))
          {
            //HUMANSERIAL.print("\t zero");
            
            calculatedOutput = lastOutput + (wantedOutput>>2);// = min(lastOutput + 30,flashMax); // last variable is the max value for the ignition flash// calculated is now 60 instead of 40 on run 2 if wanted is 40? FIX!
            //calculatedOutput = min(calculatedOutput,wantedOutput); //fixed
            if (calculatedOutput >= flashMax)
            {
              previousFadeSetting[currentPin]=FADEXENON + 1;
              calculatedOutput = flashMax;
              //HUMANSERIAL.print("\t done");
              //HUMANSERIAL.print(xenonFrame[currentPin]);
            }
          }
          else if (previousFadeSetting[currentPin] == (FADEXENON + 1))
          {
            //HUMANSERIAL.print("\t one");
            previousFadeSetting[currentPin]= FADEXENON + 2;
            calculatedOutput = 1;
          }
          else if (previousFadeSetting[currentPin] == (FADEXENON + 2))
          {
            //HUMANSERIAL.print("\t two");
            //HUMANSERIAL.print("\t LO:");
            //HUMANSERIAL.print(lastOutput);
            calculatedOutput = lastOutput + 1; //min(lastOutput + 1,wantedOutput);
            if (calculatedOutput >= wantedOutput)
            {
              previousFadeSetting[currentPin] = FADEXENON;
              //HUMANSERIAL.print("\t done");
            }
          }
          
          
        }
        else if (wantedOutput < lastOutput)
        {
          calculatedOutput = max(0.9*lastOutput,wantedOutput); //change to sine wave fade?
          previousFadeSetting[currentPin] = FADEXENON;
        }
        else if (wantedOutput == lastOutput)
        {
          calculatedOutput = wantedOutput;
          previousFadeSetting[currentPin] = FADEXENON;
        }


        //HUMANSERIAL.print("\t cO:   ");
        //HUMANSERIAL.println(calculatedOutput);
        break;
/*
      case FADESIN:
        previousFadeSetting[currentPin] = FADESIN;

        if (FadeOff_EffectDone[WhatLight] == 0)
        {
          const byte fadestart = 150;
          const byte fadespeed = 15;
          byte fadeHelper;
          fadeHelper = SinBlinkLight5(WhatLight, fadestart, fadespeed);
          if (fadeHelper == 0)
          {
            FadeOff_EffectDone[WhatLight] = 1;
            SinBlinkHelper[WhatLight] = 0;
          }
          
        }
  
        if fadesin is off
          loopcounter%360 to phase
        if fading up
          sinblink pin phase/blinkspeed speed   brightness = constrain(127 + 128*sin(radians(blinkspeed*(currentangle + correctedphase)) ),0,255);
        if (wantedOutput > lastOutput)
          calculatedOutput = min(lastOutput + fadeUpSlowSpeed, wantedOutput);
        else if (wantedOutput < lastOutput)
          calculatedOutput = max(0.9*lastOutput,wantedOutput);// - fadeValue, wantedOutput);
        else
        {
          calculatedOutput = wantedOutput;
          //previousFadeSetting[currentPin] = 0;
        }
        break;
*/
      default:
        calculatedOutput = wantedOutput;
        previousFadeSetting[currentPin] = 0;
        break;
/*
      case FADEXENON:
          //Can we use xenonFrame as a cooldown? if more than 0 instead?
          //Can someone explain to me how the logic behind the xenon flash should work? I don't think I've ever seen one in real life. Does it only flash when the ballast is cold? Does that mean it only flashes the first time it's turned on when the vehicle is started? does the ballast cool down after a while when the light is turned off? Will the light fade up slightly if the ballast is only partially cooled? What about bixenon where dim/dipped is low power on the bulb and full brights is high power on the same bulb, when does that flash?
        HUMANSERIAL.print("xf:");
        HUMANSERIAL.print(xenonFrame[currentPin]);
        HUMANSERIAL.print("\t wO:");
        HUMANSERIAL.print(wantedOutput);
        HUMANSERIAL.print("\t LO:");
        HUMANSERIAL.print(lastOutput);
        
        if (wantedOutput > lastOutput){
          HUMANSERIAL.print("\t up");
          switch(xenonFrame[currentPin])
          {
            
            case 0:
              HUMANSERIAL.print("\t zero");
              byte flashMax = 130;//wantedOutput - (wantedOutput/4);
              calculatedOutput = min(lastOutput + 30,flashMax); // last variable is the max value for the ignition flash// calculated is now 60 instead of 40 on run 2 if wanted is 40? FIX!
              //calculatedOutput = min(calculatedOutput,wantedOutput); //fixed
              if (calculatedOutput >= flashMax)
              {
                xenonFrame[currentPin]=1;
                calculatedOutput = 1;
                HUMANSERIAL.print("\t done");
                //HUMANSERIAL.print(xenonFrame[currentPin]);
              }
              break;

            case 1:
                HUMANSERIAL.print("\t one");
                xenonFrame[currentPin]=2;
                calculatedOutput = 1;
                //HUMANSERIAL.print(xenonFrame[currentPin]);
              break;
  
            case 2:
              HUMANSERIAL.print("\t two");
              HUMANSERIAL.print("\t LO:");
              HUMANSERIAL.print(lastOutput);
              calculatedOutput = min(lastOutput + 1,wantedOutput);
              if (calculatedOutput >= wantedOutput)
              {
                xenonFrame[currentPin] = 0;
                HUMANSERIAL.print("\t done");
              }
              //HUMANSERIAL.print(xenonFrame[currentPin]);
              break;

            default:
              HUMANSERIAL.print("\t why????");
              break;
          }
        }
        //if (wantedOutput < lastOutput){
        else if (wantedOutput < lastOutput)
        {
           HUMANSERIAL.print("\t down");
           xenonFrame[currentPin] = 0;
           calculatedOutput = max(lastOutput - fadeValue, wantedOutput);
        }
        HUMANSERIAL.print("\t cO:");
        HUMANSERIAL.println(calculatedOutput);
        break;

      default:
        //HUMANSERIAL.println("nopenope");
        break;

        

        
            
*/
    }

  
    
    //if (currentPin == 5) HUMANSERIAL.print('\t');
    //if (currentPin == 5) HUMANSERIAL.println(calculatedOutput);
    
    fadeValueArray[currentPin] = calculatedOutput;

    return calculatedOutput;
   
    /*
     * 
     * switch (fade)
     * {
     *  case u:
     *  case b:
     *    blabla
     *    break;
     *  case d:
     *  case b:
     *    blabla
     *    break;
    if ( (fadeSetting == 's') )
    {
      //is it possible to do sine blink without detecting a start condition? am I doing that in my normal blinker?
      // in light flasher with active start I'm resetting turn signal phase when it's off.
      if wanted == last
        reset phase
      else if wanted > last
        sinblink up (phase at 6 or 9)
      else if wanted < last
        sinblink down (phase at 12 or 3)
        
      if (wantedOutput > lastOutput)
      {
          calculatedOutput = min(lastOutput + 20, wantedOutput);
      }
      else if (wantedOutput < lastOutput)
      {
          calculatedOutput = max(lastOutput - 20, wantedOutput);
      }
        
    }
    */

}


//works, but needs cleaning. 'q' quick fade, up and down. 'u' fade up. 'd' fade down. 'b' fade both ways. 'x' xenon flash then slow fade up.
//Set the required fade setting on each pin in the fadeSettingsArray.
// Uses wantedOutput < lastOutput to determine if light is turning on or off.
// currently 3x21 ~  60 bytes of storage for all pins.

//Currently 1 fadesetting per pin, not function. Upgrade?

byte simplefadertestingxenon(byte currentPin, byte wantedOutput)
{
    const byte fadeSettingsArray[NumberOfPorts] = {'q','x','q',255,255,'x','q',255,255,255,255,255,255,255,255,255,255,255,255,255,'q'};
    static byte fadeValueArray[NumberOfPorts];
    
    //byte test = 1;
    const byte fadeSpeed = 10;
    const byte fadeUpSlowSpeed = 1;
    int lastOutput = 0;
    int calculatedOutput = 0;
    byte fadeValue = 0;
    byte fadeSetting = 0;
    static byte xenonFrame[NumberOfPorts];
    //xenon cooldown? decrease startvalue over time?
    //HUMANSERIAL.println("test");

    fadeValue = fadeSpeed;

    lastOutput = fadeValueArray[currentPin];

    fadeSetting = fadeSettingsArray[currentPin];

    calculatedOutput = wantedOutput; // else

    if ( (fadeSetting == 'q') )
    {
      if (wantedOutput > lastOutput)
        calculatedOutput = min(lastOutput + 20, wantedOutput);
      else if (wantedOutput < lastOutput)                         //changed if to else if. untested!
        calculatedOutput = max(lastOutput - 20, wantedOutput);
    }
    /*
     * 
     * switch (fade)
     * {
     *  case u:
     *  case b:
     *    blabla
     *    break;
     *  case d:
     *  case b:
     *    blabla
     *    break;
    if ( (fadeSetting == 's') )
    {
      //is it possible to do sine blink without detecting a start condition? am I doing that in my normal blinker?
      // in light flasher with active start I'm resetting turn signal phase when it's off.
      if wanted == last
        reset phase
      else if wanted > last
        sinblink up (phase at 6 or 9)
      else if wanted < last
        sinblink down (phase at 12 or 3)
        
      if (wantedOutput > lastOutput)
      {
          calculatedOutput = min(lastOutput + 20, wantedOutput);
      }
      else if (wantedOutput < lastOutput)
      {
          calculatedOutput = max(lastOutput - 20, wantedOutput);
      }
        
    }
    */
    if ( (fadeSetting == 'u') || (fadeSetting == 'b') )
    {
      if (wantedOutput > lastOutput)
        calculatedOutput = min(lastOutput + fadeUpSlowSpeed, wantedOutput);
    }
    if ( (fadeSetting == 'd') || (fadeSetting == 'b') || (fadeSetting == 'x'))
    {
      if (wantedOutput < lastOutput)
        calculatedOutput = max(lastOutput - fadeValue, wantedOutput);
    }

    //Can we use xenonFrame as a cooldown? if more than 0 instead?
    //Can someone explain to me how the logic behind the xenon flash should work? I don't think I've ever seen one in real life. Does it only flash when the ballast is cold? Does that mean it only flashes the first time it's turned on when the vehicle is started? does the ballast cool down after a while when the light is turned off? Will the light fade up slightly if the ballast is only partially cooled? What about bixenon where dim/dipped is low power on the bulb and full brights is high power on the same bulb, when does that flash?
    if ( fadeSetting == 'x')
    {
      if (wantedOutput > lastOutput){
        switch(xenonFrame[currentPin])
        {
          case 0:
            calculatedOutput = min(lastOutput + 30,130); // calculated is now 60 instead of 40 on run 2 if wanted is 40? FIX!
            //calculatedOutput = min(calculatedOutput,wantedOutput);
            if (calculatedOutput >= 130)
            {
              xenonFrame[currentPin]++;
              calculatedOutput = 1;
            }
            break;

          case 1:
            calculatedOutput = min(lastOutput + 1,wantedOutput);
            if (calculatedOutput >= wantedOutput)
            {
              xenonFrame[currentPin] = 0;
            }
            break;
        }
      }
      //if (wantedOutput < lastOutput){
      else
         xenonFrame[currentPin] = 0;
        
        
    }

    //HUMANSERIAL.print("xf:");
    //HUMANSERIAL.println(xenonFrame);
    
    
    
    
    fadeValueArray[currentPin] = calculatedOutput;

    return calculatedOutput;
}




void lightSwitchingReadable(void){
    if (NumberOfPorts > sizeof(outputLedArray))
    {
        Serial.println("Error: NumberOfPorts mismatch");
        return;
    }
    //HUMANSERIAL.println("lights");

    resetTurnSignalPhase();

    // Run through all light pins
    for (byte currentPin = 0; currentPin < NumberOfPorts; currentPin++)
    {
        byte activeFunction = 0;
        byte functionSetting = 0;
        byte pwmSetting = 0;
        byte activePWM = 0;
        byte activeLightState = 0;

        //HUMANSERIAL.print("p:");
        //HUMANSERIAL.print(currentPin);

        //Load the settings for the first function in the settings array
        functionSetting = settingsArray2[currentPin][0][0] -48; //[function1][value]
        pwmSetting = settingsArray2[currentPin][0][1];
        //If the flag for the function is currently activated, use the settings for that function to control the pin.
        //HUMANSERIAL.print("s1:");
        //HUMANSERIAL.print(functionSetting);
        if (flagsArray[functionSetting])
        {
            activeFunction = functionSetting;
            activePWM = pwmSetting;
            //HUMANSERIAL.print("y");
        }
        //HUMANSERIAL.print("a1:");
        //HUMANSERIAL.print(activeFunction);
        //HUMANSERIAL.print("w1:");
        //HUMANSERIAL.print(activePWM);

        //HUMANSERIAL.print("s2:");
        //HUMANSERIAL.print(functionSetting);
             
        //Load the settings for the second function in the settings array
        functionSetting = settingsArray2[currentPin][1][0] -48; //[function2][value]
        pwmSetting = settingsArray2[currentPin][1][1];
        //If the flag for the function is currently activated, use the settings for that function to control the pin.
        if (flagsArray[functionSetting])
        {
            activeFunction = functionSetting;
            activePWM = pwmSetting;
        }

        //HUMANSERIAL.print("a2:");
        //HUMANSERIAL.print(activeFunction);
        //HUMANSERIAL.print("w2:");
        //HUMANSERIAL.print(activePWM);


        //With the active settings loaded...

        //check the actual light state based on timings and the selected function.

        if (activeFunction == 8 || activeFunction == 9) //turnsignal
                    //todo: if previous off, reset?
                    activeLightState = flashingFunctionStartActiveSimple(activeFunction, specialTimingArray[0]);

        else if (activeFunction == 5 || activeFunction == 6) //flashers
                    activeLightState = flashingFunction(activeFunction, timingArray[currentPin][activeTimingArray]);


        else  //If simple lightfunction
                    activeLightState = activeFunction;

        //HUMANSERIAL.print("aS:");
        //HUMANSERIAL.println(activeLightState);


        //******* Write to the LED port *******

        if (activePWM == 'l') //if active = pwm based, half/LOW
        {
            if (activeLightState){
                //Serial.println('h');
                analogWrite(outputLedArray[currentPin],pwmHalfValue);}
            else
                digitalWrite(outputLedArray[currentPin],LOW);
        }
        else if (activePWM == 'h') //if inactive = pwm based, HIGH/half
        {
            if (activeLightState)
                digitalWrite(outputLedArray[currentPin],HIGH);
            else
            {
                //Serial.println('l');
                analogWrite(outputLedArray[currentPin],pwmHalfValue);
            }
            
        }
        else //if no pwm 
        {
            if (activeLightState)
                digitalWrite(outputLedArray[currentPin],HIGH);
            else
                digitalWrite(outputLedArray[currentPin],LOW);
        }



    }

}

/*
 * 1: is second function active? (flag active, not HIGH) if so, send to working array.
 * 2: else, send first function to working array.
 * 
 */
void lightSwitchingMain(void){ //byte tempstate
//indexversion

if (NumberOfPorts > sizeof(outputLedArray))
{
  Serial.println("Error: NumberOfPorts mismatch");
  return;
}

  byte workingArray[NumberOfPorts][2]; //valgt funksjon av eller på (nå)
                        /* {
                            {0,0},
                            {0,0},
                            {0,0},
                            {0,0}
                            };
                            */

resetTurnSignalPhase();
                            
//run through all pins with first configured function, then again with second function, and write to the pin
    byte frame = 0;
    byte secondfunction = 0;
    while (frame < NumberOfPorts){
      //***jump into lighSwitchesIntoWorkingArray() and return***/
        workingArray[frame][secondfunction] = lightSwitchesIntoWorkingArray(frame, secondfunction); //av eller på
        frame++;
    }

    frame = 0;
    secondfunction = 1;
    while (frame < NumberOfPorts){
        workingArray[frame][secondfunction] = lightSwitchesIntoWorkingArray(frame, secondfunction);
        frame++;
    }

    frame = 0;
    while (frame < NumberOfPorts)
    {

        
        //workingArrayIndex determines if we're displaying primary or secondary function based on if the secondary function is currently switched on, not if it's HIGH.
        //This means brakelight should only override HIGH, but flashing will flash LOW+HIGH.
        byte workingArrayIndex = workingArray[frame][1];
        byte workingLightFunction = settingsArray2[frame][workingArrayIndex][0]; // selected function
        byte workingLightPWM = settingsArray2[frame][workingArrayIndex][1]; // use pwm?
        byte workingLightHigh; //light HIGH or LOW right now?

        //byte workingArrayActive = workingArray[frame][0] | workingArray[frame][1];  //shortcut if port inactive, unused?
        byte workingArrayCurrentFunctionEnabled = workingArray[frame][workingArrayIndex]; //shortcut to check if function is enabled


        // Should port + function be active right now?

        

        if (workingLightFunction == '8' || workingLightFunction == '9') //turnsignal
            //todo: if previous off, reset?
            workingLightHigh = flashingFunctionStartActiveSimple(workingArray[frame][workingArrayIndex], specialTimingArray[0]);

        else if (workingLightFunction == '5' || workingLightFunction == '6') //flashers
            workingLightHigh = flashingFunction(workingArray[frame][workingArrayIndex], timingArray[frame][activeTimingArray]);


        else  //If simple lightfunction
            workingLightHigh = workingArray[frame][workingArrayIndex];
           

        //**** drivingState modifiers ***
        
        //if (workingLightFunction > '3' && drivingState == 1) //ignition off
        //    workingLightHigh = 0;

            

            //----------------NOTE because of workingArrayIndex a secondary 'h' function will NEVER be output as pwmHalfValue) ----------//

        //******* Write to the LED port *******

            if (workingLightPWM == 'l') //if active = pwm based
            {
                if (workingLightHigh){
                    //Serial.println('h');
                    analogWrite(outputLedArray[frame],pwmHalfValue);}
                else
                    digitalWrite(outputLedArray[frame],LOW);
            }
            else if (workingLightPWM == 'h') //if inactive = pwm based
            {
                if (workingLightHigh)
                    digitalWrite(outputLedArray[frame],HIGH);
                else
                {
                    //Serial.println('l');
                    analogWrite(outputLedArray[frame],pwmHalfValue);
                }
                
            }
            else //if no pwm (can skip maybe?)
            {
                if (workingLightHigh)
                    digitalWrite(outputLedArray[frame],HIGH);
                else
                    digitalWrite(outputLedArray[frame],LOW);
            }

            //if (digital_pin_to_timer_PGM[frame] == NOT_ON_TIMER )
            //  digitalWrite(frame,HIGH);
            


        frame++;


        
    }


}

// no longer used without lightswitchingmain
/*****************************************************************************
 * 
 * frame -> transistor output number
 * settingsArray2[frame][0 or 1][0] -> chosen function for the pin
 * external dependencies: settingsArray2[][2][2]
 * assists with loading the correct enabledflag for the function into a temporary array
 * 
 * Custom special functions for switching dependent enabled flags (like turning off parking
 * lights when turning on main headlights could be set here.
 * 
 * ***************************************************************************
 */
/*
 #define defaultOff flagsArray[0]
#define parkLights flagsArray[1]
#define frontNear flagsArray[2]
#define fullBeam flagsArray[3]
#define fullBeam2 flagsArray[4]
#define warningFlash flagsArray[5]
#define beaconAmber flagsArray[6]
#define floodLight flagsArray[7]
#define turnSignalLeft flagsArray[8]
#define turnSignalRight flagsArray[9]
#define brakeLight flagsArray[10]
#define reverseLight flagsArray[11]
*/

byte lightSwitchesIntoWorkingArray(byte frame, byte arraycounter)
{
    //byte frame = 2; //skip pin 0 and 1 to preserve tx/rx -- moved to the digitalWrite
    //while(frame <= 13){ 
        byte workingArrayValue = 0;
        //Serial.println(settingsArray2[frame][arraycounter][0]);

        // workingArrayValue = flagsArray[settingsArray2[frame][arraycounter][0]-'0'];
        
        switch(settingsArray2[frame][arraycounter][0]){
            
            case '1':
                workingArrayValue = parkLights;
                //workingArrayValue = turnSignalLeft; // turnSignalLeft = the value of flagsArray in spot x defined by the define list in globals.h
                //turnSignal(array[frame], frame);
	            break;
            case '2':
                workingArrayValue = frontNear;
                //turnSignal(array[frame], frame);
                break;
            case '3':
                workingArrayValue = fullBeam;
                break;
            case '4':
                //brake
                workingArrayValue = fullBeam2;
                break;
            case '5':
                workingArrayValue = warningFlash;
                break;
            case '6':
                workingArrayValue = beaconAmber;
                break;
            case '7':
                workingArrayValue = floodLight;
                break;  
            case '8':
                workingArrayValue = turnSignalLeft;
                break;  
            case '9':
                workingArrayValue = turnSignalRight;
                break;
            case ':':
                workingArrayValue = brakeLight;
                break;
            case ';':
                workingArrayValue = reverseLight;
                break;
            case '0':
                workingArrayValue = 0;
                break;  
            
        }

        return workingArrayValue;
        
}

void resetTurnSignalPhase (void) {

    //HUMANSERIAL.print("checking... ");

    //if both turn signals are off, reset phase placeholder to 255 to let flashing function recognize first start.
    if (turnSignalLeft || turnSignalRight)
    {
        ;//HUMANSERIAL.println("noreset");
    }
    else
    {
        //HUMANSERIAL.println("RESET");
        specialTimingArray[0][1] = 255;
    }
}



/****************************************************************************
 * Flashing function, master of the flashing universe, also works for turnsignals.
 * //external dependencies: runCount (inc 1 for every main loop) 
 * //expected tempArray: 3,0,15,1
 * Each main loop is a frame. Divide the infinite row of frames into repeating sections with length of "numberOfFrames".
 * Use math to check if the current frame is active. Phase delays first active frame by x number of frames.
 * numberOfFlashes = 2, numberOfFrames = 6, numberOfActiveFrames = 1:
 *  x0x000 x0x000 x0x000 x0x000
 *  phase=3:
 *  000x0x 000x0x 000x0x 000x0x
 *
 * Simple math without multiple flashes: frames 3, active frames 1.
 *  frame 1: 1/3 = 0. 1 left over. 1 < 1 false. 
 *  frame 2: 2/3 = 0. 2 left over. 2 < 1 false.
 *  frame 3: 3/3 = 1. 0 left over. 0 < 1 true.
 *  frame 4: 4/3 = 1. 1 left over. 1 < 1 false.
 *  frame 5: 5/3 = 1. 2 left over. 2 < 1 false.
 *  frame 6: 6/3 = 2. 0 left over. 0 < 1 true.
 *  if ( runCount % frames < active frames )
 *      LED on
 *  
 * 
 * **************************************************************************
 */

byte flashingFunction (byte enabledCheck, byte tempArray[4])
{
    
    byte numberOfFlashes = tempArray[0];
    byte phase = tempArray[1];
    byte numberOfFrames = tempArray[2];
    byte numberOfActiveFrames = tempArray[3];
    byte flasherState = 0;
    byte flasherTempCount = 0;
    
    //Serial.println(flashing);
    //phase = phase - 1; //why?
    
    //shortcut if enabled flag is false
    if (enabledCheck == 0)
            return 0;
            
    //shortcut if light is constant on
    //if (numberOfFrames == 1 && numberOfActiveFrames == 1)
    //        return 1;
    
    //if PWM, return 30 for low instead of off.
    //if enabledCheck > 1 means PWM?

    //flasherState = 1;
    while (flasherTempCount < numberOfFlashes)
     {
        //mod=(runCount+phase)%numberOfFrames;
        if ((runCount+phase)%numberOfFrames >= (numberOfActiveFrames*2*flasherTempCount) && (runCount+phase)%numberOfFrames < numberOfActiveFrames * (1 + 2 * flasherTempCount))
           flasherState = 1;
        //else? no else. start with flasherState 0 and if results in 1 if true. the while loop makes it an OR for all incs of flashertempcount. 
        flasherTempCount++;
     }
  
   return flasherState;
}


// For funcions that should go active immediately when turned on, like turn signals. Cheat by using moving the phase to the current runcount.
byte flashingFunctionStartActiveSimple (byte enabledCheck, byte tempArray[4])
{
    byte phase = tempArray[1];
    byte numberOfFrames = tempArray[2];
    byte numberOfActiveFrames = tempArray[3];
    byte flasherState = 0;

    if (enabledCheck == 0)
            return 0;

    //HUMANSERIAL.print("Phase1:");
    //HUMANSERIAL.print(phase);

    if (phase == 255)
    {
        phase = numberOfFrames - (runCount % numberOfFrames );
        specialTimingArray[0][1] = phase;
    }

    //*** probably need to reset phase to 255? (solved for turn signal, check buttonsetupmenu***

    //HUMANSERIAL.print("Phase2:");
    //HUMANSERIAL.println(phase);

    if ((runCount + phase)%numberOfFrames < numberOfActiveFrames )
    {
        flasherState = 1;
    }

    return flasherState;
}
/*
byte flashingFunctionAvinor1(byte enabledCheck, byte tempArray[4])
{
     static byte b = 241;                                       // 0000 1111 0001 or 4 low, 4 high, then 4 bits that changes for each sequence.
     const byte numberOfFrames = 12;                            // 4+4+4 = 12
     byte phase = tempArray[1];
     
     localFrameNumber = (runCount + phase) % numberOfFrames;    // split the "infinite" number of loops into sequences of 12 frames.
  
     if (enabledCheck == 0)                                     // I don't remember why I'm doing this check. I should probably consider removing it.
       return 0;
  
     if (localFrameNumber < 4)                                  // First 4 should be low
        flasherState = 0;
     else                                                       // the next 8 is in the bit pattern of the byte
        flasherState = b & (1>>localFrameNumber);
  
     if (localFrameNumber >= numberOfFrames)
        b = b++;
     if (b > 246)
        b = 241;

     return flasherState;

}



*/
