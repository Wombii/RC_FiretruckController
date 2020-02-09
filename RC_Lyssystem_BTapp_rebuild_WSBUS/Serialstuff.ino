//For timing setup, app sends text: <T pin , comma separated text data: 6,0,20,1, 1,0,40,1, >
//For function setup, app sends bytes: <S pin 4xascii >

#define TRANSMISSIONended 2
#define TRANSMISSIONstarted 1

#define STARTbyte '<'
#define ENDbyte   '>'

#define RXPART

byte incomingSerialArray[64] = {0};



/*byte timingArray[6][2][4] = {
                          {{1,0,99,0},{1,0,0,0}},
                          {{2,0,0,0},{2,0,0,0}},
                          {{3,0,0,0},{3,0,0,0}},
                          {{4,0,0,0},{0,0,0,0}},
                          {{5,0,0,0},{0,0,0,0}},
                          {{6,0,0,0},{0,0,0,0}}
                          };

byte settingsArray2[8][2][2] =  { // 'h' = high is pwm 'l' = low is pwm
                                  {{'1','h'}, {'0','0'}},
                                  {{'2','0'}, {'0','0'}},
                                  {{'3','0'}, {'0','h'}},
                                  {{'4','0'}, {'0','0'}},
                                  {{'5','0'}, {'0','0'}},
                                  {{'6','0'}, {'0','0'}},
                                  {{'7','0'}, {'0','0'}},
                                  {{'8','0'}, {'0','0'}}
                                  };
                          

*/
/*
void setup() {
  //pinMode(19,INPUT_PULLUP);//for JY-MCU 1.06 module, arduino rx pin
  // put your setup code here, to run once:
BTSERIAL.begin(115200);
HUMANSERIAL.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  SerialInput2();
  //Serial1.println("nope");
  delay(10);
}
*/

//send bokstav, kommer fram som byteverdi hos arduino. skriv ut byte, kommer fram som tresifret verdi.

void SerialInput2(void)
{
  //HUMANSERIAL.println(BTSERIAL.available());
  //Serial2.println(BTSERIAL.available());
//send bytes 254, t(?), timingbytes, 255
//need to compensate currentlyreading for byte?
  //static byte currentlyReading = 0;
  //static byte currentlyAvailable = 0;
  static byte menuSelection = 0;
  
  static byte marker = 0;
  //static byte incomingState = 0; //1 started 2 complete
  static byte transmissionState = 0;

  //static 
  //#ifdef RXPART
  if (HUMANSERIAL.available())
  {
    byte humanRXed = HUMANSERIAL.read();
    if (humanRXed == 'f')
      setupHelpOutput();
    else if (humanRXed == 'E'){
      savesetup();
      HUMANSERIAL.println("saved");}
    else if (humanRXed == 'C'){
      soundModuleCalibrate();
    }
    else if (humanRXed == 'u'){
      soundSelector(7);
    }
    else if (humanRXed == 'd'){
      soundSelector(8);
    }
    else
      HUMANSERIAL.println("f-print,E-save");
      //Serial2.println("f");
    
  }
  
  if (BTSERIAL.available() > 0)
  {
    HUMANSERIAL.print("mkay:");
    HUMANSERIAL.print((char)BTSERIAL.peek());
    HUMANSERIAL.print("byte");
    HUMANSERIAL.println(BTSERIAL.peek());
    
  /*
    static byte currentlyReading = 0;
    static byte incomingStartPin = 0;
    static byte incomingSerialArray[64] = {0};
    static byte timingArray[6][2][4] = {0};
    static byte marker = 0;
    static byte incomingTimingComplete = 0;
  
    
  */

  //NEW TEST

  /***********************************
   * From BT serial:
   * case 0: look for startbyte
   * case S: Receive human readable functions for 1 pin. First byte is pin number.  //app sends bytes: <S pin 4xascii >
   * case T: Receive human readable timing values for 1 pin. First byte is pin number.//app sends text: <T pin , comma separated text data: 6,0,20,1, 1,0,40,1, >
   * case t: 
   * case r: Readout full timingarray.
   * case f: Readout 1 pin functions array.
   * case F: Readout 1 pin fade settings array.
   * case E: Save to eeprom.
   * case A: Receive setup stuff.
   * case a: Readout setup stuff.
   * case B: Readout SBUS 16 channels. //not implemented
   */
 
  switch(menuSelection)
  {
    case 0:
      
      //Look for startbyte
      if (transmissionState == 0)
      {
        byte rxed = 0;
        byte currentlyAvailable = BTSERIAL.available();              //take a snapshot of BTSERIAL.available() to prevent hanging for a long time
        while(currentlyAvailable && transmissionState == 0)
        {
          rxed = BTSERIAL.read();
          if (rxed == STARTbyte)
          {
            transmissionState = TRANSMISSIONstarted;
            Serial.println("started");
          }
          else
          {
            currentlyAvailable--;                             //Read all the bytes in serialbuffer at the time we started this loop
          }

        }
      }
      //First byte after startbyte is menuSelection
      else
      {
        if (BTSERIAL.available())
        {
          menuSelection = BTSERIAL.read();
          //Serial.print("selected:");
          //Serial.println(menuSelection);
        }
      }
      break;

    case 'S': //1 pin. Also works from Arduino Serial console. 
              // Expected: startbyte pin '9' 'h' '2' '0' endbyte ('9' means 9+48 = 57. text as bytes)

      if (transmissionState == TRANSMISSIONstarted)
      {
        HUMANSERIAL.println("reading S");
        //read into buffer
        byte currentlyAvailable = BTSERIAL.available(); 
        //Serial.print("CA:");
        //Serial.println(currentlyAvailable);
        
        byte rxed = 0;
        while(currentlyAvailable && transmissionState == TRANSMISSIONstarted)
        {
          rxed = BTSERIAL.read();
          HUMANSERIAL.println((char)rxed);
          currentlyAvailable--;
          //Serial.print("read:");
          //Serial.println(rxed);
          //Serial.print("CA:");
          //Serial.println(currentlyAvailable);
          //Serial.print("tS");
          //////Serial.println(transmissionState);
          if (rxed != ENDbyte)
          {
            incomingSerialArray[marker] = rxed; //subtract 48 if sending bytevalues
            marker++;

            //If we fill the incoming buffer without getting endbyte, assume failed transmission and clean up.
            if (marker == sizeof(incomingSerialArray)-1)
            {
              Serial.println("failed");
              marker = 0;
              menuSelection = 0;
              transmissionState = 0;
            }
            
          }
          else
          {
            transmissionState = TRANSMISSIONended;
            HUMANSERIAL.println("ended");
          }
        }
      }

      if (transmissionState == TRANSMISSIONended)
      {
        static byte incomingStartPin = 255;
        //byte startIndex = 1;
        
        HUMANSERIAL.println("copying");
        //Serial.println(marker);

        printArray('I');
      
        
        //Copy from the incoming serial array to settingsArray
        //if (sizeof(incomingSerialArray)<6)
        //  incomingStartPin = incomingSerialArray[0]-48;
        //else{
          incomingStartPin = ( (incomingSerialArray[0]-48) *10);
          incomingStartPin += (incomingSerialArray[1]-48);
          //startIndex = 2;
        //}

        //At this point we should have something like: incomingSerialArray[] = {startbyte,pin,'9','h','2','0',endbyte}
        //If we add fadesetting per pin: incomingSerialArray[] = {startbyte,pin,'x','9','h','2','0',endbyte}
        HUMANSERIAL.println(sizeof(incomingSerialArray));
        HUMANSERIAL.println(incomingStartPin);
        byte L = 2; 
        byte k = 0;
        byte j = 0;

        //Sneak in fadesettingsarray here if we have one fadesetting per pin, not function.
        //HUMANSERIAL.print("fade:");
        fadeSettingsArray[incomingStartPin][0] = incomingSerialArray[L++];
        //HUMANSERIAL.print(fadeSettingsArray[incomingStartPin][0]);
        fadeSettingsArray[incomingStartPin][1] = incomingSerialArray[L++];
        //HUMANSERIAL.println(fadeSettingsArray[incomingStartPin][1]);

        while(L < marker)
        {
          //printArray('S');
          settingsArray2[incomingStartPin][j][k++] = incomingSerialArray[L++];
          if (k > 1) //1 less than array definition
          {
            k = 0;
            j++;
          }
          if (j > 1) // 1 less than array definition
          {
            j = 0;
            incomingStartPin++;
          }
        }

        //HUMANSERIAL.print("fade:");
        printArray('F'); 
        //HUMANSERIAL.print("settings:");
        printArray('S');

        //cleanup
    
        marker = 0;
        incomingStartPin = 255;

        memset(incomingSerialArray, 0, sizeof(incomingSerialArray)); //incomingSerialArray = {0};

        menuSelection = 0;
        transmissionState = 0;
      }

      break;

 //timingarray, 1 pin, both arrays
    case 'T': // receives ø1,10,11,12,13,14,15,16,17,æ (send numbers as text, including comma separator.)

      

      if (transmissionState == TRANSMISSIONstarted)
      {
        Serial.println("reading T");
        //read into buffer
        byte currentlyAvailable = BTSERIAL.available(); 
        //Serial.print("CA:");
        //Serial.println(currentlyAvailable);
        
        byte rxed = 0;
        byte rxedNumber;
        static byte stringMarker = 0;
        static byte incomingString[5];
        
        while(currentlyAvailable && transmissionState == TRANSMISSIONstarted)
        {
          rxed = BTSERIAL.read();
          HUMANSERIAL.println((char)rxed);
          currentlyAvailable--;
          //Serial.print("read:");
          //Serial.println(rxed);
          //Serial.print("CA:");
          //Serial.println(currentlyAvailable);
          //Serial.print("tS");
          //////Serial.println(transmissionState);
          
          if (rxed != ENDbyte)
          {
            if (rxed == ',')
            {
                incomingString[stringMarker] = '\0';
                rxedNumber = atoi(incomingString);
                Serial.print("rxedNumber");
                Serial.println(rxedNumber);
                stringMarker = 0;

                incomingSerialArray[marker] = rxedNumber; //remove 48 when sending bytevalues
                marker++;

            }
            else
            {
                incomingString[stringMarker] = rxed;
                stringMarker++;
            }
            //If we fill the incoming buffer without getting endbyte, assume failed transmission and clean up.
            if (marker == sizeof(incomingSerialArray)-1)
            {
              Serial.println("failed");
              marker = 0;
              menuSelection = 0;
              transmissionState = 0;
            }
          }
          else
          {
            transmissionState = TRANSMISSIONended;
            Serial.println("ended");
          }
        }
      }
      
      if (transmissionState == TRANSMISSIONended)
      {
        static byte incomingStartPin = 255;

        printArray('i');
        
        Serial.print("copying:");
        //switch for currentlyreading

        //Serial1.write(incomingSerialArray,marker-1);
        //Serial1.write('\n');
        //Serial1.print("timingArray:");

        //for (byte i = 0; i < marker; i++)
        //{
        //  timingArray[incomingStartPin][0][i] = incomingSerialArray[i];
        //}

        //Copy from the incoming serial array to timingArray

        Serial.println(incomingStartPin = incomingSerialArray[0]) ;
        byte L = 1;
        byte k = 0;
        byte j = 0;
        while(L < marker)
        {
          timingArray[incomingStartPin][j][k++] = incomingSerialArray[L++];
          if (k > 3)
          {
            k = 0;
            j++;
          }
          if (j > 1)
          {
            j = 0;
            incomingStartPin++;
          }
        }
        
        printArray('t');

        memset(incomingSerialArray, 0, sizeof(incomingSerialArray));

        marker = 0;
        incomingStartPin = 255;
        transmissionState = 0;
        menuSelection = 0;
      }
      
      break;
      

    case 't': //No longer used. Send values as bytevalues instead of human text.

      if (transmissionState == TRANSMISSIONstarted)
      {
        Serial.println("reading t");
        //read into buffer
        byte currentlyAvailable = BTSERIAL.available(); 
        //Serial.print("CA:");
        //Serial.println(currentlyAvailable);
        
        byte rxed = 0;
        while(currentlyAvailable && transmissionState == TRANSMISSIONstarted)
        {
          rxed = BTSERIAL.read();
          currentlyAvailable--;
          //Serial.print("read:");
          //Serial.println(rxed);
          //Serial.print("CA:");
          //Serial.println(currentlyAvailable);
          //Serial.print("tS");
          //////Serial.println(transmissionState);
          if (rxed != ENDbyte)
          {
            incomingSerialArray[marker] = rxed; //remove 48 when sending bytevalues
            marker++;

            //If we fill the incoming buffer without getting endbyte, assume failed transmission and clean up.
            if (marker == sizeof(incomingSerialArray)-1)
            {
              Serial.println("failed");
              marker = 0;
              menuSelection = 0;
              transmissionState = 0;
            }
            
          }
          else
          {
            transmissionState = TRANSMISSIONended;
            Serial.println("ended");
          }
        }
      }
      
      if (transmissionState == TRANSMISSIONended)
      {
        static byte incomingStartPin = 255;
        
        Serial.println("copying");
        //switch for currentlyreading

        //Serial1.write(incomingSerialArray,marker-1);
        //Serial1.write('\n');
        //Serial1.print("timingArray:");

        //for (byte i = 0; i < marker; i++)
        //{
        //  timingArray[incomingStartPin][0][i] = incomingSerialArray[i];
        //}

        //Copy from the incoming serial array to timingArray

        incomingStartPin = incomingSerialArray[0]-48 ;
        byte L = 1;
        byte k = 0;
        byte j = 0;
        while(L < marker)
        {
          timingArray[incomingStartPin][j][k++] = incomingSerialArray[L++];
          if (k > 3)
          {
            k = 0;
            j++;
          }
          if (j > 1)
          {
            j = 0;
            incomingStartPin++;
          }
        }
        
        printArray('t');

        memset(incomingSerialArray, 0, sizeof(incomingSerialArray));

        marker = 0;
        incomingStartPin = 255;
        transmissionState = 0;
        menuSelection = 0;
      }
      break;

    case 'r': // readout timing array full

      {
      //<T0,1,0,99,0,1,0,0,0,><T1,2,0,0,0,2,0,0,0,>...
      byte pin = 0;
      byte subArray = 0;
      byte dataMarker = 0;
      while(pin < NumberOfPorts)
      {
        BTSERIAL.print((char)STARTbyte);
        BTSERIAL.print("T");
        BTSERIAL.print(pin);
        BTSERIAL.print(",");
        while(subArray < 2)
        {
          BTSERIAL.print(timingArray[pin][subArray][dataMarker++]);
          BTSERIAL.print(",");
          
          if (dataMarker > 3) //1 less than array definition
          {
            dataMarker = 0;
            subArray++;
          }
        }
        subArray = 0;
        pin++;
        BTSERIAL.print((char)ENDbyte);
        //delay(10);
      }
      menuSelection = 0;
      transmissionState = 0;
      }

      break;

    case 'f':

      {
       //readout 1 pin, both sub arrays
      //<S01h00><S12000><S2300h>...
      byte pin = 0;
      byte subArray = 0;
      byte dataMarker = 0;
      while(pin < NumberOfPorts)
      {
        BTSERIAL.print((char)STARTbyte);
        BTSERIAL.print("S");
        if (pin <10)
          BTSERIAL.print("0");
        BTSERIAL.print(pin);
        
        BTSERIAL.print(",");
        HUMANSERIAL.print((char)STARTbyte);
        HUMANSERIAL.print("S");
        if (pin <10)
          HUMANSERIAL.print("0");
        HUMANSERIAL.print(pin);
        HUMANSERIAL.print(",");
        while(subArray < 2)
        {
          BTSERIAL.print((char)settingsArray2[pin][subArray][dataMarker]);
          HUMANSERIAL.print((char)settingsArray2[pin][subArray][dataMarker]);
          dataMarker++;
          if (dataMarker > 1) //1 less than array definition
          {
            dataMarker = 0;
            subArray++;
          }
        }
        subArray = 0;
        pin++;
        BTSERIAL.print((char)ENDbyte);
        HUMANSERIAL.print((char)ENDbyte);
        //delay(10);
      }

      menuSelection = 0;
      transmissionState = 0;

      }

      break;

    case 'F':

      {
        //Readout fade settings
       //readout 1 pin
      //<F00,50><F01,00><F02,10>...
      byte pin = 0;
      byte subArray = 0;
      byte dataMarker = 0;
      while(pin < NumberOfPorts)
      {
        BTSERIAL.print((char)STARTbyte);
        BTSERIAL.print("F");
        if (pin <10)
          BTSERIAL.print("0");
        BTSERIAL.print(pin);
        
        BTSERIAL.print(",");
        
        HUMANSERIAL.print((char)STARTbyte);
        HUMANSERIAL.print("F");
        if (pin <10)
          HUMANSERIAL.print("0");
        HUMANSERIAL.print(pin);
        HUMANSERIAL.print(",");
        
        BTSERIAL.print((char)fadeSettingsArray[pin][0]);
        BTSERIAL.print((char)fadeSettingsArray[pin][1]);

        HUMANSERIAL.print((char)fadeSettingsArray[pin][0]);
        HUMANSERIAL.print((char)fadeSettingsArray[pin][1]);

        pin++;
        
        
        BTSERIAL.print((char)ENDbyte);
        HUMANSERIAL.print((char)ENDbyte);
        //delay(10);
      }

      menuSelection = 0;
      transmissionState = 0;

      }

      break;

    case 'E':

      savesetup();
      //BTSERIAL.read();
      HUMANSERIAL.println("saved");
      menuSelection = 0;
      transmissionState = 0;
      break;

    case 'a':

      
       // receive setup stuff: (<A10,0,0,1,50,35,100,>)
       // number of ports
       // throttle reversed
       // steering reversed
       // reverse type
       // signalInterval
       // reverseDelayTicks
       // * neutralband?
       //

      if (transmissionState == TRANSMISSIONstarted)
      {
        Serial.println("reading A");
        //read into buffer
        byte currentlyAvailable = BTSERIAL.available(); 
        
        byte rxed = 0;
        byte rxedNumber;
        static byte stringMarker = 0;
        static byte incomingString[5];
        
        while(currentlyAvailable && transmissionState == TRANSMISSIONstarted)
        {
          rxed = BTSERIAL.read();
          HUMANSERIAL.println((char)rxed);
          currentlyAvailable--;
          
          if (rxed != ENDbyte)
          {
            if (rxed == ',')
            {
                incomingString[stringMarker] = '\0';
                rxedNumber = atoi(incomingString);
                Serial.print("rxedNumber");
                Serial.println(rxedNumber);
                stringMarker = 0;

                incomingSerialArray[marker] = rxedNumber; //remove 48 when sending bytevalues
                marker++;

            }
            else
            {
                incomingString[stringMarker] = rxed;
                stringMarker++;
            }
            //If we fill the incoming buffer without getting endbyte, assume failed transmission and clean up.
            if (marker == sizeof(incomingSerialArray)-1)
            {
              Serial.println("failed");
              marker = 0;
              menuSelection = 0;
              transmissionState = 0;
            }
          }
          else
          {
            transmissionState = TRANSMISSIONended;
            Serial.println("ended");
          }
        }
      }
      
      if (transmissionState == TRANSMISSIONended)
      {
        static byte incomingStartPin = 255;

        printArray('i');
        
        Serial.print("copying:");

        //Copy from the incoming serial array to timingArray


        //numberOfPorts = incomingSerialArray[0];
       // throttleReversed = incomingSerialArray[1];
       // steeringReversed = incomingSerialArray[2];
        reverseType2 = incomingSerialArray[3];
        signalInterval = incomingSerialArray[4];
        reverseDelayTicks = incomingSerialArray[5];

        memset(incomingSerialArray, 0, sizeof(incomingSerialArray));

        marker = 0;
        incomingStartPin = 255;
        transmissionState = 0;
        menuSelection = 0;
      }
      
      break;
       

    case 'A':

      
       // readout setup stuff:
       

      byte tempPrintArray[6];

      //tempPrintArray[0] = numberOfPorts;
    //  tempPrintArray[1] = throttleReversed;
    //  tempPrintArray[2] = steeringReversed;
      tempPrintArray[3] = reverseType2;
      tempPrintArray[4] = signalInterval;
      tempPrintArray[5] = reverseDelayTicks;

      BTSERIAL.print((char)STARTbyte);
      for(byte i=0; i<30; i++)
      {
        BTSERIAL.print(incomingSerialArray[i]);
        BTSERIAL.print(",");
      }
      BTSERIAL.print((char)ENDbyte);

      HUMANSERIAL.println("setupstuff:<");
      for(byte i=0; i<30; i++)
      {
        HUMANSERIAL.print(incomingSerialArray[i]);
        HUMANSERIAL.print(",");
      }
      HUMANSERIAL.println(">");

      menuSelection = 0;
      transmissionState = 0;
      break;
        

    default:

      HUMANSERIAL.println("no valid selection");
      menuSelection = 0;
      transmissionState = 0;

    break;
  }
  

        

     
  }

  //#endif

  
}


void printArray(byte printSelection)
{
  
  Serial.write('\n');

  switch(printSelection)
  {
    case 'S':

      for(byte i=0; i<30; i++)
      {
        Serial.print((char)settingsArray2[0][0][i]);
        Serial.print(",");
        //Serial1.write(timingArray[0][0][i]); //sends bytevalues to app
      }
      break;

    case 'F':
      for(byte i=0; i<30; i++)
      {
        Serial.print((char)fadeSettingsArray[0][i]);
        Serial.print(",");
        //Serial1.write(timingArray[0][0][i]); //sends bytevalues to app
      }
      break;

    case 't':

      for(byte i=0; i<30; i++)
      {
         Serial.print(timingArray[0][0][i]); //sends human number
         Serial.print(",");
        //Serial1.write(timingArray[0][0][i]); //sends bytevalues to app
      }
      break;

    case 'I':

      for(byte i=0; i<30; i++)
      {
        Serial.print((char)incomingSerialArray[i]);
        //Serial1.write(timingArray[0][0][i]); //sends bytevalues to app
      }
      break;

    case 'i':

      for(byte i=0; i<30; i++)
      {
        Serial.print(incomingSerialArray[i]);
        Serial.print(",");
        //Serial1.write(timingArray[0][0][i]); //sends bytevalues to app
      }
      break;

    default:

      Serial.print("?");
    
    break;
      

  }

  Serial.write('\n');
  
}

void setupHelpOutput (void)
 {
      
      byte pin = 0;
      byte subArray = 0;
      byte dataMarker = 0;
      HUMANSERIAL.println("F:");
      for (byte i=0; i<NumberOfPorts; i++)
      {
        HUMANSERIAL.print("{'");
        HUMANSERIAL.print((char)fadeSettingsArray[i][0]);
        HUMANSERIAL.print("','");
        HUMANSERIAL.print((char)fadeSettingsArray[i][1]);
        HUMANSERIAL.print("'},\n");
      }
      
      HUMANSERIAL.println("S:");
      while(pin < NumberOfPorts)
      {
        HUMANSERIAL.print("{{");
        //HUMANSERIALSERIAL.print("S");
        //HUMANSERIALSERIAL.print(pin);
        while(subArray < 2)
        {
          HUMANSERIAL.print("'");
          HUMANSERIAL.print((char)settingsArray2[pin][subArray][dataMarker++]);
          HUMANSERIAL.print("'");

          if (dataMarker == 1)
            HUMANSERIAL.print(",");

          if (dataMarker == 2 && subArray == 0)
            HUMANSERIAL.print("},{");
          
          if (dataMarker > 1) //1 less than array definition
          {
            dataMarker = 0;
            subArray++;
          }
        }
        subArray = 0;
        pin++;
        HUMANSERIAL.println("}},");
       
        //delay(10);
      }

  
 }
 
