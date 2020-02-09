  /*
  //byte direction;
  byte floodLight;
  byte fullBeam = 0;
  byte fullBeam2 = 0; //add
  byte warningFlash=0;
  byte brakeLight = 1;
  byte reverseLight;
  byte frontNear = 1;
  byte turnSignalLeft = 1;
  byte turnSignalRight = 1;

void testmodeSwitches2(void)
  {
    // set all to 0 first?
    floodLight |= digitalRead();
    fullBeam |= digitalRead();
    fullBeam2 |= digitalRead();
    warningFlash |= digitalRead();
    brakeLight |= digitalRead();
    reverseLight |= digitalRead();
    frontNear |= digitalRead();
    //servotest
    lightMast |= digitalRead();
  }

#define FLOODLIGHTFLAG = flagsArray[0]
#define FULLBEAMFLAG = flagsArray[1]
#define FULLBEAM2FLAG = flagsArray[2]
#define WARNINGFLASHFLAG = flagsArray[3]
#define BRAKELIGHTFLAG = flagsArray[4]
#define REVERSELIGHTFLAG = flagsArray[5]
#define FRONTNEARFLAG = flagsArray[6]
#define TURNSIGNALLEFTFLAG = flagsArray[7]
#define TURNSIGNALRIGHTFLAG = flagsArray[8]


flagsArray[] = 
{
  0,  //FLOODLIGHTFLAG
  0,  
  0,
  0,
  0,
  0,
  0,
  0,
}
*/
/*
  void testmodeSwitches3(void)
  {
    byte readPin = 
    for (byte i = 0; i< ; i++){
      flagsArray[i] |= digitalRead(readPin);
      readPin++;
    }
    //lightmastflag

  }
*/

// No longer used?
/*
void testmodeSwitches(void)
{
  byte readPin = firstTestModeSwitchPin;
  activeTimingArray = !digitalRead(readPin);
  //Serial.println(!digitalRead(readPin));
  readPin+=2; //first switch is active timing array
  if (sBus.Failsafe() == 4) //if no radio, set all flags to 0 to make switches absolute. if radio available, switches can only enable.
  {
    for (byte i = 1; i<sizeof(flagsArray) ; i++){
    flagsArray[i] = 0;
    }
  }
  for (byte i = 1; i<sizeof(flagsArray) ; i++){
    flagsArray[i] |= !digitalRead(readPin);
    Serial.print(i);
    Serial.print("-");
    Serial.print(flagsArray[i]);
    Serial.println(readPin);
    readPin+=2;
  }
  //activeTimingArray = digitalRead(readPin);
  
  //lightmastflag
  

}

*/

// Read switches into flagsArray
void testmodeSwitches2(void)
{
  byte readPin = 0;
  //HUMANSERIAL.println("TS\t");
  //activeTimingArray = !digitalRead(readPin);
  //Serial.println(!digitalRead(readPin));
  
  readPin = diagSwitchArray[0]; 

  if (1)//(sizeof(flagsArray)-1 == sizeof(diagSwitchArray))
  {
    
    if (SBUS.failsafe == 4) //if no radio, set all flags to 0 to make switches absolute. if radio available, switches can only enable.
    {
      for (byte i = 1; i<sizeof(flagsArray) ; i++){
      flagsArray[i] = 0;
      }
    }
    for (byte i = 1; i<=sizeof(diagSwitchArray) ; i++){
      flagsArray[i] |= !digitalRead(diagSwitchArray[i-1]);
      //HUMANSERIAL.println((byte)flagsArray[i]);
      /*Serial.print(i);
      Serial.print("-");
      Serial.print(flagsArray[i]);
      Serial.print("-");
      Serial.print(readPin);
      Serial.print('\t');*/

      //readPin+=2;
    }
  //activeTimingArray = digitalRead(readPin);
  
  //lightmastflag
  }
  

}

/*
void testmodeDisplayLED(void)
{
  byte LEDpin = firstTestModeDisplayPin;
  for (byte i = 1; i<sizeof(flagsArray) ; i++){
    digitalWrite(LEDpin,flagsArray[i]);
    LEDpin+=2;
  }
}*/

void testmodeDisplayLED2(void)
{
  
  if (1)//(sizeof(flagsArray)-1 == sizeof(diagLedArray)) //cheap way to make sure we don't read outside the diag arrays
  {
  
    for (byte i = 0; i<sizeof(diagLedArray) ; i++){
      digitalWrite(diagLedArray[i],flagsArray[i+1]);
    }
  }
  for (byte i = 0; i<sizeof(diagRadioLedArray) ; i++){
      digitalWrite(diagRadioLedArray[i],diagRadioLedArrayState[i]);
    }
}

//void testmodeDisplayRadioLED(void) ligger i SBUS_logic.ino

// Outputs flagsArray over HUMANSERIAL
void testmodeSerial(void)
{/*
  for (byte i = 0; i<sizeof(flagsArray) ; i++){
    Serial.write(flagsArray[i]+48);
  }
  Serial.print(":)");
  Serial.println(activeTimingArray);
  */
  /*for (byte i= 0; i<sizeof(diagSwitchArray)-1 ; i++)
  {
    HUMANSERIAL.print(i+1);
    HUMANSERIAL.print(":");
    HUMANSERIAL.print(digitalRead(diagSwitchArray[i]));
    HUMANSERIAL.print('\t');
  }
  HUMANSERIAL.print(sizeof(diagSwitchArray));
  HUMANSERIAL.print(":");
  HUMANSERIAL.println(digitalRead(diagSwitchArray[sizeof(diagSwitchArray)-1]));
*/
  for (byte i= 0; i<sizeof(flagsArray) ; i++)
  {
    HUMANSERIAL.print(i);
    HUMANSERIAL.print(":");
    HUMANSERIAL.print((byte)flagsArray[i]);
    HUMANSERIAL.print('\t');
  }
  HUMANSERIAL.print('\n');
  /*
  HUMANSERIAL.print(sizeof(flagsArray)-1);
  HUMANSERIAL.print(":");
  HUMANSERIAL.println((byte)flagsArray[sizeof(flagsArray)-1]);*/
}

//flash each main LED channel for a half second.
void testmodesequenceSerial(void)
{
  for (int i=0; i< sizeof(outputLedArray); i++){
    digitalWrite(outputLedArray[i]-1,LOW);
    digitalWrite(outputLedArray[i],HIGH);
    delay(500);
  }
}

//click each switch in sequence, outputs an array to be pasted into global.h
void testmodeSortSwitches(void)
{

  byte testsortingArray[11];
  byte lastpin =0;
  byte currentpin = 0;
  byte readState = 0;
  byte j=0;

  HUMANSERIAL.println("clicky:");

  while(j<11){
   for (byte i = 0; i<11 ; i++){
    readState = digitalRead(diagSwitchArray[i]);
    //HUMANSERIAL.println(readState);
    
    if (readState == 0)
    {
      //HUMANSERIAL.println("LOW");
      currentpin = diagSwitchArray[i];
      if (currentpin != lastpin && currentpin != 0)
      {
        lastpin = currentpin;
        testsortingArray[j] = currentpin;
        
        //HUMANSERIAL.print(j);
        //HUMANSERIAL.print(":");
        //HUMANSERIAL.println(currentpin);
        j++;
      }
    }

   }
  }

  
    
  
   for (byte i = 0; i<11; i++){
     HUMANSERIAL.print(testsortingArray[i]);
     HUMANSERIAL.print(',');
   }
}

/*
detect LOW
store switch number in array at ++index 
if switch number is same as at array index, do not store
if index == sizeofArray, end
*/

// Infinite loop! add this to donoradiostufflist to get the correct order for the interface panel connections.
void sortInterfacePanelLoop (void)
{
  while(1)
  {

    if (HUMANSERIAL.available() > 0)
    {
      char readchar = Serial.read();
      switch(readchar)
      {
        //click each switch in sequence, outputs an array to be pasted into global.h
        case 'I':
        {
          byte numberOfPins = sizeof(diagSwitchArray);
          HUMANSERIAL.println("first switch pin:");
          int firstPin = HUMANSERIAL.parseInt();
          byte tempArray[numberOfPins];
          byte arrayIndex = 0;
          while( arrayIndex < numberOfPins )
          {
            for(int i = firstPin; i< firstPin+numberOfPins; i++)
            {
              if ( digitalRead(i) == LOW) 
              {
                byte detectedSwitch = i;
                if ( detectedSwitch != tempArray[arrayIndex] )
                {
                  arrayIndex++;
                  tempArray[arrayIndex] = detectedSwitch;
                }
              }
            }
          }
          for (byte i = 0; i<numberOfPins; i++){
            HUMANSERIAL.print(tempArray[i]);
            HUMANSERIAL.print(',');
          }
          break;
        }

        //Sort the 11 switch/flag LEDs
        case 'L':
        {
          byte numberOfPins = sizeof(diagLedArray);
          HUMANSERIAL.println("first LED pin:");
          int firstPin = HUMANSERIAL.parseInt();
          byte tempArray[numberOfPins];
          byte arrayIndex = 0;

          for (byte i = 0; i<sizeof(tempArray);i++)
          {
            tempArray[i]=firstPin+i;
          }
          HUMANSERIAL.println("x to exit");
          
          int i = 0;
          while(readchar != 'x')
          {
            if (HUMANSERIAL.available() > 0)
            {
              readchar = HUMANSERIAL.read();
            }

            digitalWrite(tempArray[i],HIGH);
            HUMANSERIAL.print(i);
            HUMANSERIAL.print(":");
            HUMANSERIAL.println(tempArray[i]);

            if (digitalRead(diagSwitchArray[7]) == LOW)  
              tempArray[i]++;
            if (digitalRead(diagSwitchArray[7]) == LOW)  
              tempArray[i]--; 
            if ( tempArray[i] > firstPin + numberOfPins) 
              tempArray[i] = firstPin + numberOfPins; 
            else if ( tempArray[i] < firstPin ) 
              tempArray[i] = firstPin;  

            if (digitalRead(diagSwitchArray[7]) == LOW)
              i++;
            if (digitalRead(diagSwitchArray[7]) == LOW)
              i--;
            if ( i > 10 )
              i = 10;
            else if ( i < 0 )
              i = 0;
    
          }
          for (byte i = 0; i<numberOfPins; i++){
            HUMANSERIAL.print(tempArray[i]);
            HUMANSERIAL.print(',');
          }
          break;
        }
            
          
      }
    }
  }
}
