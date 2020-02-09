void logicandflagsSBUS(void){

    // ---- SETTING FLAGS ---- //
    // ----------------------- //

    //Read throttleAxis and set flags. Also check flag for reversed throttle channel.
    // |  brake    | neutral | forward
    // | reverse   |
    //-------------------------------------------------------------------------------
    byte forwardFlag = 0, brakeFlag = 0, reverseFlag = 0, neutralFlag = 0; //nullstilles hver gang????? arduino ide setter static hvis ikke = 0?


    if (throttleAxisReversed)
    {
        if (throttleAxis < throttleNeutralLowerLimit)
        forwardFlag = 1;
        
        else if (throttleAxis > throttleNeutralUpperLimit){
        brakeFlag = 1;
        reverseFlag = 1;
        }  
        
        else //if (throttleAxis < throttleNeutralUpperLimit && throttleAxis > throttleNeutralLowerLimit)
        neutralFlag = 1;

    }
    else //if not reversed
    {
        if (throttleAxis > throttleNeutralUpperLimit)
        forwardFlag = 1;
        

        else if (throttleAxis < throttleNeutralLowerLimit){
        brakeFlag = 1;
        reverseFlag = 1; 
        } 
        
        else //if (throttleAxis < throttleNeutralUpperLimit && throttleAxis > throttleNeutralLowerLimit)
        neutralFlag = 1;
    }

    //Read auxAxis and set flag to be sent to auxSwitch function
    //----------------------------------------------------------
    /*byte auxLevel;
    if (auxAxis > 1500)
    auxLevel = 1;
    else
    auxLevel = 0;*/

    

    //Read steering axis and set flags 
    //  Left | Center | Right
    //----------------------------------------------------------

    
    byte steeringLeftFlag = 0;
    byte steeringRightFlag = 0;
    byte steeringCenterFlag = 0; 
    

    if (steeringAxisReversed)
    {
        if (steeringAxis > steeringCenter+20)
        {
            steeringLeftFlag = 1;
        }
        else if (steeringAxis < steeringCenter-20)
        {
            steeringRightFlag = 1;
        }
        else
        {
            steeringCenterFlag = 1;
        }
    }
    else //if not reversed
    {
        if (steeringAxis < steeringCenter-20)
        {
            steeringLeftFlag = 1;
        }
        else if (steeringAxis > steeringCenter+20)
        {
            steeringRightFlag = 1;
        }
        else
        {
            steeringCenterFlag = 1;
        }
    }
    
    

    // --- USING FLAGS --- //
    // ------------------- //

    //Enable or disable reverselights and brakelights based on axis flags.
    //--------------------------------------------------------------------
    if (forwardFlag){
    reverseLight = 0;
    brakeLight = 0;}

    if (neutralFlag){
    brakeLight = 0;}

    if (brakeFlag)
    brakeLight = 1;

    //Reverse logic
    //-------------------------------
    static byte reverseFlag2 = 0;
    
    //Double pump reverse
    if (reverseType2){
        static byte reverseFlagCount = 0;
        static byte lastWasReverseFlag = 0;
        if (reverseFlag)// && !lastReverseFlag)
        {
            if (lastWasReverseFlag == 0){
            reverseFlagCount++;
            lastWasReverseFlag =1;
            }
        }
        else // if reverseflag
            lastWasReverseFlag = 0;
        if (forwardFlag)
            reverseFlagCount = 0;
        if (reverseFlagCount >= 2){
            //Serial.println("rev");
            reverseFlagCount = 2; //prevent overflow
            reverseLight = 1;
            brakeLight = 0;
            //reverseFlag2 = 1;
        }
    }

    //Delayed reverse
    else
    {   
        static byte reverseFlagCount = 0;
        if (reverseFlag)
        {
            reverseFlagCount++;
            if (reverseFlagCount > reverseDelayTicks){
                reverseLight = 1;
                brakeLight = 0;
                reverseFlagCount = reverseDelayTicks; //prevent overflow
                }
        }
        else
        {
            reverseFlagCount = 0;
            reverseLight = 0;
        }
    } 

    //Turn off turn signal when crossing center
    //-------------------------------------------

    static byte steeringSameSideAsTurnSignal = 0;

    if (turnSignalLeft && !turnSignalRight)
    {
        if (steeringLeftFlag)
        {
            steeringSameSideAsTurnSignal = 1;
        }
        if (steeringCenterFlag && steeringSameSideAsTurnSignal)
        {
            steeringSameSideAsTurnSignal = 0;
            turnSignalLeft = 0;
        }
    }
    else if (!turnSignalLeft && turnSignalRight)
    {
        if (steeringRightFlag)
        {
            steeringSameSideAsTurnSignal = 1;
        }
        if (steeringCenterFlag && steeringSameSideAsTurnSignal)
        {
            steeringSameSideAsTurnSignal = 0;
            turnSignalRight = 0;
        }
    }




    // -------- SBUS channels ---------//
    // --------------------------------//

/*


           #define channelTurnSignals       4
           #define channelEMFLashers        10 //9
           #define channelHeadlights        5 //6
           #define channelHeadlightstoggle  6 //3
           #define channelFullBeam2         7
           #define channelFloodLights       8 //11
           #define channelIgnition          9 //8
           #define channelAmberflash        11
           #define channelAudioSwitch       12 //14
           
           #define testServoChannel         12 //??? remove
           #define pumpWinchEnableChannel   13
           #define pumpWinchServoChannel    14
           #define monitorXChannel          15
           #define monitorYChannel          16



    if (SBUS.channels[5] > 1000)
        ;//something
    else
       ; //something else

       */

    // Turn Signals
    //Serial.println(SBUS.channels[channelTurnSignals-1]);    //note to self: why did I do this and not just =1? Possibly because of something smart with cancelling?
/*    if (SBUS.channels[channelTurnSignals-1] < 700)
    {
        turnSignalLeft++;
        turnSignalRight--;
    }
    else if(SBUS.channels[channelTurnSignals-1] > 1300)
    {
        turnSignalLeft--;
        turnSignalRight++;
    }
    if (turnSignalLeft > 1) //limit value to 0 - 1
        turnSignalLeft = 1;
    if (turnSignalLeft < 0)
        turnSignalLeft = 0;
    if (turnSignalRight > 1)
        turnSignalRight = 1;
    if (turnSignalRight < 0)
        turnSignalRight = 0;
*/
    ///*should test if this works and replace
    if (SBUS.channels[channelTurnSignals-1] < 700)
    {
        turnSignalLeft  =1;
        turnSignalRight =0;
    }
    else if(SBUS.channels[channelTurnSignals-1] > 1300)
    {
        turnSignalLeft  =0;
        turnSignalRight =1;
    }
    // */

   //Flashers
    if (SBUS.channels[channelEMFLashers-1] > 1000)
        warningFlash = 1;
    else
        warningFlash = 0;

    if (SBUS.channels[channelAmberflash-1] < 1000)
        beaconAmber = 1;
    else
        beaconAmber = 0;

   //Headlights main switch | off - park - drive ?
    if (SBUS.channels[channelHeadlights-1] < 700)
    {
        parkLights = 0;
        frontNear = 0;  
        fullBeam = 0;  
        cabDomeLight = 1;
        //ignitionState = 0; 
    }
    else if(SBUS.channels[channelHeadlights-1] > 1300)
    {
        parkLights = 1;
        frontNear = 1;
        cabDomeLight = 0; //hack
        //fullBeam = 1; //remove
        //ignitionState = 2;
    }
    else // centered
    {
        parkLights = 1;
        frontNear = 0;
        fullBeam = 0;
        //ignitionState = 1;
    }

   //Headlights ver 2, toggle on elevator stick. | stick returns to center. Up = temporary, down = toggle
    static char fullBeamtogglehelper = 0;
    if (SBUS.channels[channelHeadlightstoggle-1] > 1300) //up
    {
      fullBeam = 1;
      fullBeamtogglehelper = 2; //is up
    }
    else if (SBUS.channels[channelHeadlightstoggle-1] < 700) //down
    {
      if (fullBeamtogglehelper == 0) // if crossing limit from center to down (only do this once, when crossing limit)
      {
        /*if (fullBeam)
          fullBeam = 0;
        else
          fullBeam = 1;*/
        fullBeam = !fullBeam; //toggle fullbeam
        fullBeamtogglehelper = 1; // is down
      }
      if (!frontNear) //only full beam if dipped is on.
        fullBeam = 0;
      // else if held down, do nothing
    }
    else //stick centered
    {
      if (fullBeamtogglehelper == 2) // if crossing limit from up to center
      {
        fullBeam = 0;
        //fullBeam2 = 0;
      }
      fullBeamtogglehelper = 0; // is centered
    }

    // fullbeam 2. Off = off. On = on if fullbeam is on. Actual switching is slaved to fullbeam.
    if (SBUS.channels[channelFullBeam2-1] > 1000)
    {
        if (fullBeam)
        {
            fullBeam2 = 1; //fullBeam 2 only if fullBeam is on
        }
        else
        {
            fullBeam2 = 0;
        }
    }
    else
        fullBeam2 = 0;

/*
    if (SBUS.channels[channelFloodLights-1] > 1000)
    //modify to only enabled if parked.
        floodLight = 1;
    else
        floodLight = 0;
*/
    /*
        if (frontNear)
        {
            floodlight = 0;
        }
        else
        {
            floodlight = 1;
        }
    else
    {
        floodlight = 0;
    }

    */


    if (SBUS.channels[channelFloodLights-1] < 700)
    {
        floodLight = 0;
    }
    else if(SBUS.channels[channelFloodLights-1] > 1300)
    {
        floodLight = 1;
        if (SBUS.channels[pumpWinchEnableChannel-1] < 1300) mastPosition = 1; //some kind of safety for light mast. not while driving?
        
    }
    else // centered
    {
        floodLight = 1;
        mastPosition = 0;
        
    }

    // ----  ignition switch  ---- //
    // --------------------------- //

    if (SBUS.channels[channelIgnition-1] < 700)
    {
        ignitionState = 0;
    }
    else if(SBUS.channels[channelIgnition-1] > 1300)
    {
        ignitionState = 2;
    }
    else // centered
    {
        ignitionState = 1;
    }


    //Turn off all lights except park and hazard when ignition is off.
    if (ignitionState == 0)
    {
      if (frontNear)
        parkLights = 0; //if the lights knob is in a normal setting, parking lights are off.
      frontNear = 0;
      fullBeam = 0;
      fullBeam2 = 0;
      warningFlash = 0;
      beaconAmber = 0;
      floodLight = 0;
      brakeLight = 0;
      reverseLight = 0;
      cabDomeLight = 1;
    }
    if (ignitionState != 0)
      cabDomeLight = 0;
  

    // ----  Servo logic  ---- //
    // ----------------------- //      

#if DISABLED
    //safety check for mast servo (in case end stop malfunction).
    //check channel every x seconds. if unchanged (+/- 10) since last, return to neutral to save motor life.
    static unsigned long testServoTimer = 0;
    static byte currentMastState = 0;
    static byte lastMastState = 0;
    static int testServoValue = 1300;
    const int testServoCenter = 1300;
    
    if (millis() - testServoTimer > 1000)
    {
      if mast is moving - if channel < center - 20 or center + 20
        inc counter
      else
        nothing?
      reset timer
      
    }
    if counter > 10
      center servo
      reset counter
    else
      move servo
      (inc counter?)
    

#endif

    //HUMANSERIAL.println(SBUS.channels[testServoChannel-1]); //~685 center?
    //int testServoMappedValue = map(SBUS.channels[testServoChannel-1],0,2000,1000,2000);
    //testServo.writeMicroseconds( map(SBUS.channels[testServoChannel-1],0,2000,0,2000) ); //1300 current center. +/-30 to move. 1200,1400?
    //testServo.writeMicroseconds( testServoMappedValue );
    //HUMANSERIAL.println(testServoMappedValue);

    static unsigned long mastServoTimer = 0;
    if (mastPosition == 1) {
      mastServo.writeMicroseconds( mastServoCenter - mastServoUpUs );
      //HUMANSERIAL.println(mastServoCenter - mastServoUpUs);
    }
    else {
      mastServo.writeMicroseconds( mastServoCenter + mastServoDownUs );
      //HUMANSERIAL.print("down:");
      //HUMANSERIAL.println(mastServoCenter + mastServoDownUs);
    }


    //mastServo.writeMicroseconds( map( SBUS.channels[monitorYChannel-1], 0,2000 , 1000,2000 ) );
    //HUMANSERIAL.println(map( SBUS.channels[monitorYChannel-1], 0,2000 , 1000,2000 ));

    
    //Roof monitor servos
    roofMonitorXServo.writeMicroseconds( map( SBUS.channels[monitorXChannel-1],200,1800 , 1000,1600 ) );
    roofMonitorYServo.writeMicroseconds( map( SBUS.channels[monitorYChannel-1],200,1800 , 1000,2000 ) );


    if (SBUS.channels[pumpWinchEnableChannel-1] < 700)
    {
        pumpServo.writeMicroseconds( map( SBUS.channels[pumpWinchServoChannel-1],0,2000 , 1000,2000 ) );
        pinMode(pumpEnablePin,OUTPUT);
        roofMonitorLight = 1;
        //Serial.println(map( SBUS.channels[pumpWinchServoChannel-1],0,2000 , 1000,2000 ) );
    }
    else if(SBUS.channels[pumpWinchEnableChannel-1] > 1300)
    {
        pumpServo.writeMicroseconds(1500);
        pinMode(pumpEnablePin,INPUT);
        //Serial.println("PUMP OFF");
        roofMonitorLight = 0;
    }
    else // centered
    {
        pumpServo.writeMicroseconds(1500);
        pinMode(pumpEnablePin,INPUT);
        //Serial.println("PUMP OFF");
    }
    
    
    /*HUMANSERIAL.print("ServoY in:\t");
    HUMANSERIAL.print(SBUS.channels[monitorYChannel-1]);
    HUMANSERIAL.print("ServoY out:\t");
    HUMANSERIAL.println(map( SBUS.channels[monitorYChannel-1],200,1800 , 1000,2000 ));*/

/*
    if (millis() - mastServoTimer > 1000)
    {
      mastServo.writeMicroseconds( mastServoCenter );
    }
*/

    // ----  testmodeRadioLED ---- //
    // --------------------------- //
    
    diagRadioLedArrayState[0] = forwardFlag;
    diagRadioLedArrayState[1] = neutralFlag + steeringCenterFlag;
    diagRadioLedArrayState[2] = brakeFlag;
    diagRadioLedArrayState[3] = reverseFlag;
    
    diagRadioLedArrayState[4] = steeringLeftFlag;
    diagRadioLedArrayState[5] = steeringRightFlag;


    // ---- SOUND ---- //
    // --------------- //

    
    

      audioLogic();

    

        
}  
/*
void ESPaudioLogic (void)
{
  //Horn and siren
    if (SBUS.channels[channelAudioSwitch-1] < 700)      
    {
      hornFlag ^= 1;  
    }
    else if(SBUS.channels[channelAudioSwitch-1] > 1200 && SBUS.channels[channelAudioSwitch-1] < 1600) 
    { 
      //siren1
      sirenFlag ^= 1;
      //sirenNumber = 0;
      
    }
    else if(SBUS.channels[channelAudioSwitch-1] > 1600)
    {
      //siren2
      sirenFlag ^= 1;
      //sirenNumber = 1;
    }

    //sirenTimerSwitch();
    //soundNo = 1+sirenNumber;  // 1 = siren
    
}

void ESPaudioLogic2 (void)
{
  //Horn and siren
    if (SBUS.channels[channelAudioSwitch-1] < 700)      
    {
      hornFlag = 0;  
    }
    else if(SBUS.channels[channelAudioSwitch-1] > 700 && SBUS.channels[channelAudioSwitch-1] < 1200) 
    { 
      hornFlag = 1;
      sirenFlag = 0;
    }
    else if(SBUS.channels[channelAudioSwitch-1] > 1200 && SBUS.channels[channelAudioSwitch-1] < 1700) 
    {
      hornFlag = 1;
      sirenFlag = 1;
    }
    else
    {
      hornFlag = 0;
      sirenFlag = 1;
    }
    
}
*/
void audioLogic (void){


    static byte engineRunning = 0;

    static byte wasBraking = 0;

    static byte sirenSelector = 0;

    static byte sirenButtonReady = 1;



    switch(ignitionState)
    {
        case 0:
            //HUMANSERIAL.println("i0");
            engineRunning = 0;
            break;
        
        case 1:
            //HUMANSERIAL.println("i1");
            if (engineRunning == 1)
            {
              HUMANSERIAL.println("IGNITIONSOUND off");
              soundSelector(IGNITIONSOUND);
              engineRunning = 0;
            }

    
            //Horn and siren
            if (SBUS.channels[channelAudioSwitch-1] < 700)      
            {
              HUMANSERIAL.println("HORN"); 
              soundSelector(HORNSOUND);  
            }
            else if(SBUS.channels[channelAudioSwitch-1] > 1200 && SBUS.channels[channelAudioSwitch-1] < 1600) 
            { 
              if (sirenButtonReady)
              {
                sirenButtonReady = 0;
                HUMANSERIAL.println("SIREN"); 
                soundSelector(SIRENSOUND);
              }
              
            }
            else if(SBUS.channels[channelAudioSwitch-1] > 1600)
            {
              if (sirenButtonReady)
              {
                sirenButtonReady = 0;
                HUMANSERIAL.println("SIREN2"); 
                soundSelector(SIRENSOUND2);
              }
            }
            else
            {
              sirenButtonReady = 1;
            }


            break;

        case 2:

            //HUMANSERIAL.println("i2");
            if (engineRunning == 0)
            {
              HUMANSERIAL.println("IGNITIONSOUND on");
              soundSelector(IGNITIONSOUND);
              engineRunning = 1;
            }

            //Horn and siren
            if (SBUS.channels[channelAudioSwitch-1] < 700)      
            {
              HUMANSERIAL.println("HORN"); 
              soundSelector(HORNSOUND);  
            }
            else if(SBUS.channels[channelAudioSwitch-1] > 1200 && SBUS.channels[channelAudioSwitch-1] < 1600) 
            { 
              if (sirenButtonReady)
              {
                sirenButtonReady = 0;
                HUMANSERIAL.println("SIREN"); 
                soundSelector(SIRENSOUND);
              }
              
            }
            else if(SBUS.channels[channelAudioSwitch-1] > 1600)
            {
              if (sirenButtonReady)
              {
                sirenButtonReady = 0;
                HUMANSERIAL.println("SIREN2"); 
                soundSelector(SIRENSOUND2);
              }
            }
            else
            {
              sirenButtonReady = 1;
            }

            /*
            //Horn and siren if the sound module had on and off switches.
            if (SBUS.channels[channelAudioSwitch-1] < 700)      { soundSelector(HORNSOUND);  }
            else if(SBUS.channels[channelAudioSwitch-1] > 1300)
            { 
              if (sirenSelector)
              {
                soundSelector(SIRENSOUND);
                sirenSelector = 0;
              }
              else
              {
                soundSelector(SIRENSOUND2);
                sirenSelector = 1;
              }
            }
            */

            
            //Engine sound and brake hiss
            if (reverseLight)
            {
                int engineSound = 0;
                engineSound = (throttleAxis-throttleCenter)/4;
                soundThrottleServo.writeMicroseconds( map(engineSound+throttleCenter,0,2000,800,2200) );
                //soundSelector(REVERSESOUND)
                
            }
            else if (brakeLight)
            {
                soundThrottleServo.writeMicroseconds( map(throttleCenter,0,2000,800,2200) );
                wasBraking = 1;
            }
            else
            {
              if (wasBraking)
                {
                    //soundSelector(BRAKESOUND);
                    //soundControlServoTimestamp = millis();
                    wasBraking = 0;
                }
                //sound brake hiss
                //wasBraking = 0
              //engine sound = throttleAxis
            
              soundThrottleServo.writeMicroseconds( map(throttleAxis,0,2000,800,2200) );
            }
            
            

            break;

        default:

            break;
    }

}




void timingCalibration(void){
   //----TIME CALIBRATION---//
    //Calibrate reverse delay timer and turn signal interval based on how long it takes to run the loop.
    //This will match timings across different radio setups.
    //------------------------------------------------------
    looptime = micros() - looptime;
    //Serial.println(looptime);
    //runCount++;

    //if (looptime == 14)

        //signalInterval = 35; //500ms delay / 14ms
        //reverseDelayTicks = 142;
/*
    const int smoothingStrength = 4;
    static int smoothedLooptime = 5000;
    //smoothedLooptime = looptime;
    smoothedLooptime = (smoothedLooptime + ((looptime - smoothedLooptime) >> smoothingStrength));
    //smoothedLooptime = smoothedLooptime / 1000;
  */  
    
    /*
    static byte shortcounter = 0;
    static byte longcounter = 0;
    if (looptime < shortestLooptime){
        shortcounter++;
        if (shortcounter > 5)
        {
            shortestLooptime = looptime;
            // tickspersecond = (1 000 000 micro / looptime micro). interval = tps * 0.7 -> 700 000 / looptime
            signalInterval = (signalDelay * 1000L) / shortestLooptime;  // REMEMBER L to make it long!
            reverseDelayTicks = (reverseDelay * 1000L) / shortestLooptime;
            auxMaxTXDelayTicks = (auxMaxTXDelay * 1000L) / shortestLooptime;
        }
    if (looptime > shortestLooptime){
        longcounter++;
        if (longcounter > 6)
        {
            shortestLooptime = shortestLooptime + shortestLooptime;
        
        }
    }
     
      
    }
    if (runCount%10 == 0){
        shortcounter = 0;
        longcounter = 0;
        currentlooptime = looptime; //debugging
        DIAGLED_TOGGLE; //debugging
    }*/

    #if DEBUGTIME
    HUMANSERIAL.print("loop:");
    HUMANSERIAL.print('\t');
    HUMANSERIAL.print(looptime); 
    HUMANSERIAL.print('\t');
    
    #endif   
    looptime = micros();
    /*
    signalInterval    = (long)(signalDelay*1000L)/smoothedLooptime;
    reverseDelayTicks = (long)(reverseDelay*1000L)/smoothedLooptime;

    if (runCount%50 == 0)
    {
      specialTimingArray[0][2] = signalInterval+signalInterval;
      specialTimingArray[0][3] = signalInterval;
    }
*/
    /*
    if(digitalRead(diagSwitchArray[9]) == 0)
      delay(12);
    HUMANSERIAL.print(smoothedLooptime); 
    HUMANSERIAL.print('\t');  
    HUMANSERIAL.print(runCount%50); 
    HUMANSERIAL.print('\t');  
    HUMANSERIAL.print(signalInterval); 
    HUMANSERIAL.print('\t');
    HUMANSERIAL.print('\n');*/
}
