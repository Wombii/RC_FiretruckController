
//Experimental. Could do a setup page like this for non-serial light function setup. NOT IN USE
#if 0

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

const byte DEFAULTOFF        '0'
const byte PARKINGLIGHT      '1'
const byte HEADLIGHTSLOW     '2'
const byte HEADLIGHTSFULL    '3'
const byte HEADLIGHTSFULL2   '4'
const byte BLUEFLASH         '5'
const byte AMBERFLASH        '6'
const byte FLOODLIGHT        '7'
const byte TURNSIGNALLEFT    '8'
const byte TURNSIGNALRIGHT   '9'
const byte BRAKELIGHT        ':'
const byte REVERSELIGHT      ';'
const byte CABDOMELIGHT      '<'
const byte ROOFMONITORLIGHT  '='

const byte PWMOFF-LO         'l'
const byte PWMHI-FULL        'h'

byte settingsArray2[NumberOfPorts][2][2] ; // 'h' = high is pwm 'l' = low is pwm

{

                                  {{TURNSIGNALLEFT    ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{HEADLIGHTSLOW     ,'l'},{HEADLIGHTSFULL   ,'h'}},
                                  {{TURNSIGNALRIGHT   ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{PARKINGLIGHT      ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{BLUEFLASH         ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{BLUEFLASH         ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{REVERSELIGHT      ,'0'},{FLOODLIGHT       ,'0'}},
                                  {{HEADLIGHTSFULL2   ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{CABDOMELIGHT      ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{FLOODLIGHT        ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{PARKINGLIGHT      ,'l'},{BRAKELIGHT       ,'h'}},
                                  {{BLUEFLASH         ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{BLUEFLASH         ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{PARKINGLIGHT      ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{AMBERFLASH        ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{AMBERFLASH        ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{BLUEFLASH         ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{BRAKELIGHT        ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{REVERSELIGHT      ,'0'},{DEFAULTOFF       ,'0'}},
                                  {{PARKINGLIGHT      ,'0'},{DEFAULTOFF       ,'0'}}



}

//timing Array contains timings for flashing. totalframes can be moved to settings to cut 26 bytes if necessary? Will slightly limit alternate patterns                                
  byte timingArray[NumberOfPorts][2][4] ;//=  { //multi,phase,total,active
                                  
                                  {{6,0,20,1},    {1,0,40,1}},
                                  {{6,0,20,1},    {1,10,40,1}},
                                  {{3,10,40,2},   {1,20,40,20}},
                                  {{6,10,20,1},   {1,30,40,1}},
                                  {{6,10,20,1},   {1,30,10,1}},
                                  {{6,10,20,1},   {1,3,10,1}},
                                  {{6,10,20,8},   {1,3,20,8}},
                                  {{6,10,20,8},   {1,3,20,8}},
                                  {{6,10,20,8},   {1,3,20,8}},
                                  {{6,10,20,8},   {1,3,20,8}},
                                  {{6,10,20,8},   {1,3,20,8}},
                                  {{6,10,20,8},   {1,3,20,8}},
                                  {{6,10,20,8},   {1,3,20,8}},
                                  {{3,10,14,1},   {3,0,10,1}}
                                };


#endif
