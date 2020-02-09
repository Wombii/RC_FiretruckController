/***********************************************************************************
 * Light, servo and sound control for R/C vehicles, designed for a 1:14 scale
 * airport fire truck.
 * 
 * Hardware: Arduino Mega 2560, bluetooth module, Frsky SBUS receiver, Benedini or
 * dasMikro sound module.
 * 
 * SBUS reading is now handled with custom code instead of a library to ease
 * troubleshooting and development.
 * 
 * This program may look complex, but it's not too bad if we break it down.
 * This tab: Initialize libraries, set up some global variables. The setup function
 * sets up I/O pins, serial ports and loads settings from the eeprom. The loop
 * function determines if we're getting SBUS data, and calls the rest of the functions.
 * 
 * Read the SBUS data -> process SBUS data -> switch lights and output servo pulses.
 * Serial input and the button panel is also checked every loop.
 * 
 * SBUS_logic tab: Processing of SBUS data. This is where the values of every channel
 * is determines what to switch or move.
 * 
 * newlightswitching tab: Uses the flags set in the SBUS_logic tab and the light
 * settings arrays to actually switch the output pins for the lights.
 * 
 * Serial commands are used to assign light functions and flash timing to output pins.
 */




/***********************************************************************************
*Notater
*without serialcommands: 7,5k .... lol... not anymore...
*
*
*DDRA |= (1<<3); //pin3 on port A is output 
DDRA &= ~(1<<5); //pin5 of port A is input 
PORTA |= (1<<3);; //set the output high 
PORTA &= ~(1<<3); //set the input low 
PORTA ^= (1<<3); //toggle the output 
if (PINA & (1<<5)) { // test if input is high 

Pins: atmega328
------------------------------
        D2  D3  D4  D5  D6  D7
RX  TX  02  03* 04  05* 06* 07

B0  B1  B2  B3  B4  B5
08  09* 10* 11* 12  13

C0  C1  C2  C3  C4  C5
A0  A1  A2  A3  A4  A5
*
*
*
************************************************************************************/

/*********************************************
SBUS:
The protocol is 25 Byte long and is sent every 14ms (analog mode) or 7ms (highspeed mode).
One Byte = 1 startbit + 8 databit + 1 paritybit + 2 stopbit (8E2), baudrate = 100'000 bit/s
14  15  16  17  18  19
-------------------------------
3569 10 11
*
From somewhere about SBUS:
-----
The highest bit is send first. The logic is inverted (Level High = 1)

[startbyte] [data1] [data2] .... [data22] [flags][endbyte]

startbyte = 11110000b (0xF0)

data 1-22 = [ch1, 11bit][ch2, 11bit] .... [ch16, 11bit] (ch# = 0 to 2047)
channel 1 uses 8 bits from data1 and 3 bits from data2
channel 2 uses last 5 bits from data2 and 6 bits from data3
etc.

flags = bit7 = ch17 = digital channel (0x80)
bit6 = ch18 = digital channel (0x40)
bit5 = Frame lost, equivalent red LED on receiver (0x20)
bit4 = failsafe activated (0x10)
bit3 = n/a
bit2 = n/a
bit1 = n/a
bit0 = n/a

endbyte = 00000000b

[ch17][ch18][frame lost][failsafe]

----
modified sbus rx cpp: start at fs state 4, then 0 is ok. 1 is frame lost and 3 is failsafe.

Servo library sets pinmode to output when attaching servo. does NOT set it back to input when detaching

*********************************************/
//#define SETUPVERSION 1

#define clearbit(a,z) (a &= ~_BV(z)) 
#define setbit(a,z) (a |= _BV(z)) 
#define togglebit(a) (a ^= 1)

#define SOUNDMAP(a) map(a,0,2000,800,2280)

//#define DIAGLED_TOGGLE PINB |= 1<<5;

#include <avr/eeprom.h>     //Helps write and read to eeprom.
//#include <FUTABA_SBUS_RX.h> //SBUS library from <insert name> configured to 16 channels and modified to receive only and more failsafe info.
//#include <Streaming.h>    //simplifies serial output.
#include "Globals.h"        //Moved most global variables here.
#include <Servo.h>          //Arduino servo library.


//FUTABA_SBUS sBus;           //

#define DEBUG 1             //old?:Serial debug output.
#define DEBUGTIME 0         //outputs looptime to serial.
//#define DEBUGSERIAL
//#define BTDEBUGSERIAL
//#define DX2S
//#define DEBUGGEAR         //old:replace actual auxswitch with serial output
//#define GEARSERIAL
#define GEAR 0              //old:Logic to control gear servo based on 1/2/3 clicks of a switch.
//#define atmega8
//#define BTapp 1           //old:Use bluetooth
//#define RADIOLOW          //old:Use active low version of my radioRead function.
#define SOUND 1             //Using sound module?
#define DISABLED 0          //Utility for #if DISABLED #endif

/* Set up servo names */

#if GEAR
  //#include <Servo.h>
  Servo gearservo;
#endif

#if SOUND
  Servo soundControlServo;
  Servo soundThrottleServo;
#endif

Servo mastServo;

Servo roofMonitorXServo;
Servo roofMonitorYServo;

Servo pumpServo;



/* Set up serial aliases */

#define BTSERIAL Serial3
#define HUMANSERIAL Serial
//Serial 1 used by SBUS
#define SBUSserial Serial1


/* --  Settings  -- */

//TODO: move user settings?

//Hold a button on startup to save to eeprom?

  byte throttleAxisReversed = 0;          //User setting. Overwritten by value stored in eeprom. Set in buttonSetupMenu.
  byte steeringAxisReversed = 0;          //User setting. Overwritten by value stored in eeprom. Set in buttonSetupMenu.

  byte reverseType2 = 1;                  //User setting. Overwritten by value stored in eeprom, need a way to change it. Delayed or double pump reverse. 

  byte neutralUpperOffset = 20;           //User setting. Overwritten by value stored in eeprom, need a way to change it. Dead band setting for throttle.
  byte neutralLowerOffset = 20;           //User setting. Overwritten by value stored in eeprom, need a way to change it. Dead band setting for throttle.
  
  int throttleNeutralUpperLimit = 1440;   //Calculated on startup.
  int throttleNeutralLowerLimit = 1415;   //Calculated on startup.
  int throttleCenter = 1430;              //Recorded on startup.
  int throttleFull = 1800;                //Used for soundmodule? Set in buttonSetupMenu.

  int steeringCenter = 1500;              //Recorded on startup. 

  //int gear_first = 1500, gear_third = 1500,  gear_second = 1500;
  //byte gear_first = 90, gear_second = 90, gear_third = 90; //used in: setup, main loop?
  


/* --  Light Control variables  -- */

  //byte currentpin;          //Not currently used. Moved to local in each function.
  
  //const byte diagLED = 13;  //Not currently used.


/* --  Radio variables  -- */

  int throttleAxis = 1500, steeringAxis = 1500, gearAxis = 1600; 


  //byte auxClickglobal = 0;  //Not currently used.


/* -- Other variables -- */

  unsigned int runCount=0;                    //Running count of number of loops. Used for periodic light functions.

  unsigned long shortestLooptime = 40000;
  unsigned long looptime = 0;
  unsigned long currentlooptime = 0;

  byte haveAttachedServos = 0;


  byte buttonSetupCalibrationActive = 0;
  byte buttonSoundUpdateActive = 0;


/* -- SBUS stuff -- */

#define sbusdebug 0

  //The tempArray and the struct in this union occupies the same memory locations.
  //The struct used "bit fields" to split up the bytes from the SBUS stream.
  // startbyte,22 bytes of channel data (11 bits per channel), failsafe+digital channel byte, endbyte
  union SBUSconverter
  {
    struct
    {
      byte startframe:8;
      unsigned int ch1 : 11;
      unsigned int ch2 : 11;
      unsigned int ch3 : 11;
      unsigned int ch4 : 11;
      unsigned int ch5 : 11;
      unsigned int ch6 : 11;
      unsigned int ch7 : 11;
      unsigned int ch8 : 11;
      unsigned int ch9 : 11;
      unsigned int ch10 : 11;
      unsigned int ch11 : 11;
      unsigned int ch12 : 11;
      unsigned int ch13 : 11;
      unsigned int ch14 : 11;
      unsigned int ch15 : 11;
      unsigned int ch16 : 11;
      byte d17 : 1;
      byte d18 : 1;
      byte frameLost: 1;
      byte failsafe: 1;
      byte test : 4; //junk
      byte endframe :8;
    } pieces;
    uint8_t tempArray[25];
  } SBUShelper;
  
  struct sbusdata
  {
    unsigned int channels[18];
    byte failsafe = 4;
    byte channelsReady = 0;
  } SBUS;




void setup() {
        
        
        
      /* -- Set pin modes -- */

     
      // Set pinMode for the main light channels.
      for(int i=0; i<sizeof(outputLedArray) ; i++)
      {
        pinMode(outputLedArray[i],OUTPUT);
      }


      // Set pinMode for diagLedArray (LEDs on the interface panel). 
      //Currently leaving them as input and using the internal pullups to make them dimmer.
      //for(int i=0; i<sizeof(diagLedArray) ; i++)
      //{
      //  pinMode(diagLedArray[i],OUTPUT);
      //}

      // Set pinMode for diagSwitchArray (Switches on the interface panel).
      for(int i=0; i<sizeof(diagSwitchArray) ; i++)
      {
        pinMode(diagSwitchArray[i],INPUT_PULLUP);
      }

      // Test the LEDs on the interface panel.
      
      byte LEDdelay = 50;
      
      // Light up the switch flag LEDs.
      for(int i=0; i<sizeof(diagLedArray) ; i++)
      {
        digitalWrite(diagLedArray[i],HIGH);
        delay(LEDdelay);
      } 
      delay(LEDdelay);
      // Turn off switch flag LEDs.
      for(int i=sizeof(diagLedArray)-1; i>=0 ; i--)
      {
        digitalWrite(diagLedArray[i],LOW);
        delay(LEDdelay);
      } 
      // Light up radio cross, start at top, go down, then start left and go right.
      for(int i=0; i<sizeof(diagRadioLedArray) ; i++)      
      {
        digitalWrite(diagRadioLedArray[i],HIGH);
        delay(LEDdelay);
      } 
      delay(LEDdelay);
      // Turn off radio cross LEDs.
      for(int i=sizeof(diagRadioLedArray)-1; i>=0 ; i--)
      {
        digitalWrite(diagRadioLedArray[i],LOW);
        delay(LEDdelay);
      } 


   
      
      //pinMode(0,INPUT_PULLUP);//for JY-MCU 1.06 module, arduino rx pin. Change to correct pin!

      
      #if GEAR
      //gearservo.attach(A4);
      #endif
      
          


      //sBus.begin();
      
      BTSERIAL.begin(115200);
      HUMANSERIAL.begin(115200);
      //Serial2.begin(115200);
      SBUSserial.begin(100000);

      

     


      /* --- EEPROM save/load/initialize --- */

      const byte eepromVersion = 1; /// !!! Change this to destroy and rebuild the eeprom data, for development !!! /// 
      
      Serial.println(readVersion());
      Serial.println(readEepromNumberOfPorts());

      //This should run on the very first setup of the arduino, or if the number of output ports changes from what is saved in the eeprom.
      //In case numberofports has been increased, fill arrays with default data:
      //(Why the double check? Because I will probably forget to update the version number)
      
      if((readVersion() != eepromVersion) || (readEepromNumberOfPorts() != NumberOfPorts) )
      {
        for (byte i=0; i<NumberOfPorts; i++)
        {
          settingsArray2[i][0][0] = '1';
          settingsArray2[i][0][1] = '0';
          settingsArray2[i][1][0] = '0';
          settingsArray2[i][0][1] = '0';
        }
        for (byte i=0; i<NumberOfPorts; i++)
        {
          timingArray[i][0][0] = 3;
          timingArray[i][0][1] = 0;
          timingArray[i][0][2] = 60;
          timingArray[i][0][3] = 4;
          timingArray[i][1][0] = 3;
          timingArray[i][1][1] = 0;
          timingArray[i][1][2] = 60;
          timingArray[i][1][3] = 4;
        }
        for (byte i=0; i<NumberOfPorts; i++)
        {
          fadeSettingsArray[i][0] = '0';
          fadeSettingsArray[i][1] = '0';
        }
        savesetup();
        saveEepromNumberOfPorts(NumberOfPorts);
        saveVersion(eepromVersion);
      }
      
      //if it looks like eeprom contains the right data, read settings
      else
      {
        delay(500);

        readeeprom();
  
        delay(500);
      }

      


      

     

      
      Serial.println("Hi");
      //Serial.write(settingsArray2[0][0],sizeof(settingsArray2));
      //Serial.write('\n');
      //Serial.write(timingArray[0][0],sizeof(timingArray));
      
      //Serial.print(sizeof(settingsArray2),DEC);

      

      /*
      #if DEBUG
                Serial.write(',');
              Serial.print(throttleAxisReversed);
              Serial.write(',');
              Serial.print(reverseType2);
              Serial.write(',');
              Serial.print(auxMaxTXDelay);
              Serial.write(',');
              Serial.print(reverseDelay);
              Serial.write(',');
              Serial.print(signalDelay);
              Serial.write(',');
              Serial.write('}');
      #endif*/

              
              //testmodeSortSwitches();

      // Hold down the first of the four push buttons on the interface panel to enter calibration.
      if (digitalRead(diagSwitchArray[7]) == LOW)
      {
        HUMANSERIAL.println("low");
        buttonSetupCalibrationActive = 1;
      }


      //Hold down pushbutton 2 to only send calibration data to the sound module
      if (digitalRead(diagSwitchArray[8]) == LOW)
      {
        HUMANSERIAL.println("low8");
        buttonSoundUpdateActive = 1;
        //soundModuleCalibrate();
      }


      

      

      
              

}

void loop() {
      //delay(50);
    
    /***********************************************
    radiomode
    if radio good
      logicandflags
      timingcalibration
      testmode(optional)
      lightswitch
      serialsetup
    
    displaymode
      set timings
      delay
      testmode
      lightswitch
      serialsetup
    
    
    ***********************************************/
    
    static unsigned long timeSinceChannelUpdate = 0;
    static unsigned long failsafeTimeoutStore = 0;
    
    
    SBUSinput();                                               //read from rx. When channelsReady = 1 the channels will be available in SBUS.channels[]
    SBUSfailsafeTimeout(failsafeTimeoutStore, 50);             //set failsafe status to 4 if there's no good read in x ms.
    //Serial.println(SBUS.failsafe);
    
    // If we're reading a good SBUS stream
    
    if (SBUS.channelsReady)
    {
      failsafeTimeoutStore = millis();  //refresh the failsafe time
      
      SBUS.channelsReady = 0;
      if (SBUS.failsafe == 0)
      {
        // First loop: set center values for throttle and steering
        static byte haveReadCenterValues = 0;
        if (!haveReadCenterValues){
          //int throttleCenter;
          throttleCenter = SBUS.channels[0];
          steeringCenter = SBUS.channels[1];
          haveReadCenterValues = 1;
          //set neutral range?
          throttleNeutralLowerLimit = throttleCenter - neutralLowerOffset;
          throttleNeutralUpperLimit = throttleCenter + neutralUpperOffset;
        }
    
        throttleAxis = SBUS.channels[0];
        steeringAxis = SBUS.channels[1];
        gearAxis     = SBUS.channels[2];
    
        //Serial.println(throttleAxis);
    
        // First loop: attach servos
        
        //*** Servos disables PWM on 44-46!!! Someone should probably mention that in the arduino documentation.
     
        if (!haveAttachedServos){
          attachServos();
        }
        haveAttachedServos = 1;
    
        runCount++;
    
        if (buttonSoundUpdateActive && haveReadCenterValues)
        {
          soundModuleCalibrate();
          buttonSoundUpdateActive = 0;
        }
        
        if (buttonSetupCalibrationActive)   //buttonSetupCalibration should only run if radio is active, so don't move this to donoradioStuffList
        {
          buttonSetupCalibration();
        }
        else
        {
          
          logicandflagsSBUS(); //set flags for lightswitching based on radiovalues
      
          donoradioStuffList();
    
        }
      }
      else //if FS1 we're getting failsafed sbus data from rx.
      {
        
        runCount++;
    
        //do special conditions for tx lost (lights and sound)
        //Serial.println("waitingForRx");
        //flash hazards
        turnSignalLeft = 1;
        turnSignalRight = 1;
    
        donoradioStuffList();
    
        
      }
    }
    
    else if (SBUS.failsafe == 4)// FS 4, no sbus data
    {
      //Serial.print("FS:");
      //Serial.print(sBus.Failsafe());
      //Serial.print('\t');
      //Serial.print("F4-");
      //WARNING:: When entering failsafe, SBUS HANGS UP TO 100MS
      //14.10.2018: This might have another cause. Can't replicate while testing sbus code.
      //issue is with timeSinceChannelUpdate check. checking for channelsready instead gives correct refresh rate
      //frsky x4r sends no data before tx is detected, FS4. it sends failsafe sbus data after tx lost (FS3).
    
      //02.11.2018: does this section including timeupdate even if fs0. can't change to else if fs4 because FS isn't updated without radio signal.
      //temporary fix by resetting timesincechannelupdate in (channelsready). Currently works if check (10) is longer than normal looptime (9) Could check vs normal looptime.
      //03.11.2018: updated SBUS library to revert to FS4 if no signal for 200ms
      //30.10.2019: Made my own SBUS functions to replace the sbus library
      
      /*
       * Failsafe:
       * starting without radio: don't need servos, need lights. Special lighting conditions? Sound conditions?
       * FS4, no sbus data would look the same as starting on usb power?
       * losing transmitter: idle sound, parking mode lights with hazards flashing.
       */
       digitalWrite(diagRadioLedArray[1],flashingFunctionStartActiveSimple(1,specialTimingArray[1]));
      
    
      if ( millis() - timeSinceChannelUpdate > 9 ) // loop interval approximately as long as refresh interval of radio.
      {
        
        detachServos();
        haveAttachedServos = 0;
        //haveAttachedServos = 0;
    
        runCount++;
    
        //Serial.print("F4+");
    
        //Serial.print("FS:");
        //Serial.print(sBus.Failsafe());
        //Serial.print('\t');
        
    
        
        donoradioStuffList();
    
        
    
    
    
        timeSinceChannelUpdate = millis();
    
      }
    
    }
    
    
    
        //timingCalibration();
    
        
    
        //Run auxSwitch function, passing auxLevel flag.
        //auxSwitch(auxLevel);
    
        //buttonSetupMain();
    
      
      
      
    
    
      
    
     
} //END OF VOID LOOP

void donoradioStuffList(void)
//NO RADIO STUFF HERE
{
  //HUMANSERIAL.print(digitalRead(9));
    SerialInput2();
    //HUMANSERIAL.print(digitalRead(9));
    
    testmodeSwitches2();

    //frontNear = 1;
    //brakeLight = 1;
    
    testmodeDisplayLED2();

    


    
    //testmodeSerial();
    //testmodeSortSwitches();

    

     timingCalibration();

    /*if (drivingState == StateParked)
      lightSwitchingParked();
    else if (drivingState == StateOff)
      lightswitchingOff();
    else*/
    //lightSwitchingMain();
    //lightSwitchingReadable();
    lightSwitchingReadablefadetest();


    //Switch active timing array after x runcounts
    static byte switchTiming = 0;
    if (switchTiming == 100){
      togglebit(activeTimingArray);
        
    }
    switchTiming++;
    if (switchTiming > 200)
      switchTiming = 0;
    //Serial.println(switchTiming);

    resetSoundSelector();
   // if (millis() - soundControlServoTimestamp > 70)
   //     soundControlServo.writeMicroseconds( soundControlServoDefault );

 //HUMANSERIAL.println(digitalRead(9));
}

void attachServos(void)
{
      soundThrottleServo.attach(gaspin);
      soundControlServo.attach(controlpin);

      soundThrottleServo.writeMicroseconds(throttleCenter);
      soundControlServo.writeMicroseconds(soundControlServoDefault);
      //HUMANSERIAL.println("attaching");
      //testServo.attach(testServoPin);
      mastServo.attach( mastServoPin );
      roofMonitorXServo.attach(roofMonitorXServoPin);
      roofMonitorYServo.attach(roofMonitorYServoPin);
      pumpServo.attach(pumpServoPin);

}

//Servo library sets pinmode to output when attaching servo. does NOT set it back to input when detaching
void detachServos(void)
{
    if (mastServo.attached())
    { mastServo.detach(); pinMode(mastServoPin,INPUT);}
    if (soundControlServo.attached())
    { soundControlServo.detach(); pinMode(controlpin,INPUT);}
    if (soundThrottleServo.attached())
    { soundThrottleServo.detach(); pinMode(gaspin,INPUT);}
    if (roofMonitorXServo.attached())
    { roofMonitorXServo.detach(); pinMode(roofMonitorXServoPin,INPUT);}
    if (roofMonitorYServo.attached())
    { roofMonitorYServo.detach(); pinMode(roofMonitorYServoPin,INPUT);}
    if (pumpServo.attached())
    { pumpServo.detach(); pinMode(pumpServoPin,INPUT);}

    return;
}
