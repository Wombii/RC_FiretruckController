/* --  timer variables  -- */

// Time / looptime = ticks
  byte signalInterval = 35;       //calculated live, default value MAKE LOCAL
  byte reverseDelayTicks = 142;   //calculated live, default value MAKE LOCAL

  //These two are not currently used, as the calculation part is disabled.
  int signalDelay = 1000;
  int reverseDelay = 2000; //default value, from setup.
  

  
/* --------  Lightingsystem variables ------- */
/* --  Light functions Arrays  -- */

//IF you change these, you should probably also change it in the switch in newlightswitching()
// ^ after rewriting the lightswitching function, you only need to update the number for the flashing special cases
// TODO: rewrite to flag aliases and light aliases.

//Human readable names for the flags
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
#define cabDomeLight flagsArray[12]
#define roofMonitorLight flagsArray[13]

//Array of flags for light states. This is where the program keeps track of light functions being on or off.
char flagsArray[] = //signed because it was previously necessary for turn signal logic.
{
  0,  //defaultOff
  0,  //parkLights
  0,  //frontNear
  0,  //fullBeam
  0,  //fullBeam2
  0,  //warningFlash
  0,  //beaconAmber
  0,  //floodLight
  0,  //turnSignalLeft
  0,  //turnSignalRight
  0,  //brakeLight
  0,  //reverseLight
  0,  //cabDomeLight
  0,  //roofMonitorLight
};


  const byte NumberOfPorts = 21;      //Number of LED output pins

  const byte pwmHalfValue = 30;       //value for half light intensity when doing analogWrite

  //'type', number of multiflashes, phase, frames active, frames total.
  //function 1, function 2.

  //settingsArray2 contains function assignments for each pin. Refer to definitions above for function numbers. First function is processed first, then second function.
  //Second function will override the first. pwm value 0 or '0' will switch between off and on. 'h' between full and half. 'l' between half and off.
  //Examples:
  //Brake + rear pwm, and turn signal on same lamp: {'1','l'},{'4','h'}. brake will have priority over turn signal, rear light will be dim or blinking to off.
  //Full beam headlights + emergency flashers: {'3',0},{'6',0}. emergency flashers enabled while full beam is off, but full beam will have priority.
  
  byte settingsArray2[NumberOfPorts][2][2] ; // 'h' = high is pwm 'l' = low is pwm


                                  /*
                                   * S:
                                  {{'8','0'},{'0','0'}},
                                  {{'2','l'},{'3','h'}},
                                  {{'9','0'},{'0','0'}},
                                  {{'1','0'},{'0','0'}},
                                  {{'2','0'},{'0','0'}},
                                  {{';','0'},{'0','0'}},
                                  {{':','0'},{'0','0'}},
                                  {{'7','0'},{'0','0'}},
                                  {{'1','0'},{'0','0'}},
                                  {{'5','0'},{'0','0'}},
                                  {{'5','0'},{'0','0'}},
                                  {{';','0'},{'7','0'}},
                                  {{'4','0'},{'0','0'}},
                                  {{'0','0'},{'0','0'}},
                                  {{'0','0'},{'0','0'}},
                                  {{'6','0'},{'0','0'}},
                                  {{'6','0'},{'0','0'}},
                                  {{'6','0'},{'0','0'}},
                                  {{'5','0'},{'0','0'}},
                                  {{'5','0'},{'0','0'}},
                                  {{'2','l'},{':','h'}},
                                  */


  //Fade settings.
  byte fadeSettingsArray[NumberOfPorts][2]; //{'q','x','q',255,255,0,'q',255,255,255,255,255,255,255,255,255,255,255,255,255,'q'};
                                   

  //timing Array contains timings for flashing. totalframes can be moved to settings to cut 26 bytes if necessary? Will slightly limit alternate patterns                                
  byte timingArray[NumberOfPorts][2][4] ;//=  { //multi,phase,total,active
                                  //{{6,0,20,1}, {1,0,40,1}},
                                  //{{6,0,20,1}, {1,10,40,1}},
                                  //{{3,10,40,2}, {1,20,40,20}},
                                  //{{6,10,20,1}, {1,30,40,1}},
                                  //{{6,10,20,1}, {1,30,10,1}},
                                  //{{6,10,20,1}, {1,3,10,1}},
                                  //{{6,10,20,8}, {1,3,20,8}},
                                  //{{6,10,20,8}, {1,3,20,8}},
                                  //{{6,10,20,8}, {1,3,20,8}},
                                  //{{6,10,20,8}, {1,3,20,8}},
                                  //{{6,10,20,8}, {1,3,20,8}},
                                  //{{6,10,20,8}, {1,3,20,8}},
                                  //{{6,10,20,8}, {1,3,20,8}},
                                  //{{3,10,14,1}, {3,0,10,1}}
                               // };

  //Used for stuff like turn signals.
  byte specialTimingArray[2][4] = {
                                  {1,255,signalInterval+signalInterval,signalInterval}, //Turn signal. NEEDS FIXING: currently not updated in loop.
                                  {1,0,100,50} //faster flashing for buttonsetup
                                  };
  

  byte activeTimingArray = 0; //primary or secondary timing array (to have multiple flash patterns.

  //byte drivingState = 3;

  const byte StateOff = 0;

  const byte StateParked = 1;

  const byte StateRunning = 2;

  byte ignitionState = 1; // 0 off 1 park 2 drive. selected in SBUS logic under headlights main switch.

  //const byte StateRunning = 3;

  //Arrays to assign I/O pins

  //place:                                      0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10,11,12,13,14

  const byte outputLedArray[NumberOfPorts] =  {13,12,11,10,9 ,8 ,7 ,5 ,4 ,3 ,2, 6, 23, 22, 32, 34, 36, 38, 40, 42, 44}; //Light system output pins (to transistors)
  
  const byte diagLedArray[11] =               {49,50,51,52,53,A14,A15,A13,A12,A11,A10}; //Pins for diagboard LEDs
  const byte diagRadioLedArray[6] =           {25,24,26,27,28,29};                      //Pins for diagboard Radio cross LEDs
  byte diagRadioLedArrayState[] =             {0 ,0 ,0 ,0 ,0 ,0};                       //Keeps track of lights on or off.
  const byte diagSwitchArray[11] =            {30,31,33,35,37,39,41,43,45,47,48};       //Pins for diagboard switches and buttons
  

  //Servo
  const byte roofMonitorXServoPin  = A0;
  const byte roofMonitorYServoPin  = A1;
  
  //#define soundControlServoPin   A0
  //#define soundThrottleServoPin  A1
  //#define soundLearnPin          A2

  const byte mastServoPin        =   A7;

  const byte pumpServoPin        =   A2;
  const byte pumpEnablePin       =   A3;

  int mastServoCenter       = 1325;
  const int mastServoUpUs   = 70;   //Microseconds deflection from center to raise light mast
  const int mastServoDownUs = 50;   //Microseconds deflection from center to lower light mast
  byte mastPosition         = 0;

//Sound

  int soundControlServoSound1 = 1250;
  int soundControlServoDefault = 1000;
  byte soundStepsize = 50;
  unsigned long soundControlServoTimestamp = 0;
  
  const byte soundLearnpin = A4;
  const byte gaspin = A5;
  const byte controlpin = A6; //control and gaspin might be reversed? If it doesn't work, try swapping them. //TODO: check and fix?

  #define HORNSOUND 3
  #define SIRENSOUND 5
  #define SIRENSOUND2 6
  #define REVERSESOUND 2
  #define BRAKESOUND 4
  #define IGNITIONSOUND 1
  #define VOLUPSOUND 7
  #define VOLDNSOUND 8
  

//SBUS channels

 #define channelTurnSignals       4
 #define channelEMFLashers        10 //9
 #define channelHeadlights        5 //6
 #define channelHeadlightstoggle  6 //3
 #define channelFullBeam2         7
 #define channelFloodLights       8 //11
 #define channelIgnition          9 //8
 #define channelAmberflash        11
 #define channelAudioSwitch       12 //14
 
 //#define testServoChannel         12 //??? remove
 #define pumpWinchEnableChannel   13
 #define pumpWinchServoChannel    14
 #define monitorXChannel          15
 #define monitorYChannel          16
 


 
