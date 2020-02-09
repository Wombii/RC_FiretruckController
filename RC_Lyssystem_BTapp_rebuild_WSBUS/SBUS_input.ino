/*
 * SBUS reading by Wombii
 * Simple SBUS data read and process using a union+struct to process raw serial data into channel values.
 * 
 * This is tested to work on arduino mega 2560 and arduino nano 328.
 * The union+struct method does NOT work on ESP32.
 */

 
/*
#define sbusdebug 1

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
  byte failsafe;
  byte channelsReady = 0;
} SBUS;

*/

/*
0000 0000
&
0000 0001 digital 17
0000 0010 digital 18
0000 0100 if this is 1: signal lost
0000 1000 if this is 1: signal failsafe
*/
//uint8_t inputArray[25] = { 0x0f,0x01,0x04,0x20,0x00,0xff,0x07,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x00,0x00};
//uint8_t inputArray[25] = { 0x01,0x04,0x22,0x00,0x92,0x0f,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x00,0x00,0x0f};
   


/*
void loop() {
  // put your main code here, to run repeatedly:


static unsigned long sendtime = 0;
if (millis() - sendtime > 20)
{
  for(byte i=0; i<25; i++) //send sbus on port 1
    {
      Serial1.print((char)inputArray[i]);
    }
  sendtime = millis();
}
  
#if sbusdebug
Serial.print("A:");
Serial.println(SBUSserial.available());
#endif
SBUSinput();
delay(5);
}
*/

void SBUSfailsafeTimeout(unsigned long failsafeTimeoutTimer, unsigned long timeoutDuration)
{
  //static unsigned long failsafeTimeoutStore = 0;
  //Serial.println((millis() - failsafeTimeoutTimer));
  if (millis() - failsafeTimeoutTimer > timeoutDuration)
  {
    SBUS.failsafe = 4;
  }
}

void SBUSinput(void)
{
  static byte state = 1;
  byte readbyte = 0;
  //Serial.println("s:");
  switch (state)
  {
      
    case 1:
      SBUShelper.tempArray[24] = 1; //break the endframe in tempArray to be able to test it in stage 3.
      //If the serial rx buffer is full, we get useless data.
      if (SBUSserial.available() > 53)
      {
        #if sbusdebug 
        Serial.print("buffer full"); 
        #endif
        while(SBUSserial.available())
        {
          SBUSserial.read();
          
        }
        #if sbusdebug
        Serial.println(".k");
        #endif
      }
      //Read until we find the startframe
      if (SBUSserial.available() )
      {
        while(SBUSserial.available())
        {
          readbyte = SBUSserial.read();
          //Serial.println(readbyte,HEX);
          if (readbyte == 0x0F)
          {
            SBUShelper.tempArray[0] = readbyte;
            state = 2;
            break;
          }
        }
        //else
          //state = 1;
      }
      
      break;

    case 2:
      //Serial.print("s2:");
      static byte i = 1;
      while (SBUSserial.available() )
      {
        readbyte = SBUSserial.read();
        SBUShelper.tempArray[i++] = readbyte;
        if (i >= 25)
        {
          i = 1;
          state = 3;
          break;
        }
      }
      break;

    case 3:
      //If it seems like a good full read
      if (SBUShelper.tempArray[24] == 0)
      {
        SBUS.failsafe = 0;
        SBUSpreparechannels();
        #if sbusdebug
        SBUSprintstuff();
        #endif
      }
      //if the endframe doesn't match a good full read
      else
      {
        #if sbusdebug
        Serial.println("SBUS ENDFRAME FAIL");
        Serial.print("input:");
        for(byte i=0; i<25; i++)
        {
          Serial.print(SBUShelper.tempArray[i],HEX);
          Serial.print(",");
          //Serial1.write(timingArray[0][0][i]); //sends bytevalues to app
        }
        Serial.println("/input");
        #endif
        SBUSserial.read(); //read one byte in attempt to catch up to pattern.
      }
      state = 1;
      break;
  }
  
}
void SBUSpreparechannels()
{
  #if sbusdebug 
  Serial.print("channels: ");
  #endif
  SBUS.channels[0] = SBUShelper.pieces.ch1;
  SBUS.channels[1] = SBUShelper.pieces.ch2;
  SBUS.channels[2] = SBUShelper.pieces.ch3;
  SBUS.channels[3] = SBUShelper.pieces.ch4;
  SBUS.channels[4] = SBUShelper.pieces.ch5;
  SBUS.channels[5] = SBUShelper.pieces.ch6;
  SBUS.channels[6] = SBUShelper.pieces.ch7;
  SBUS.channels[7] = SBUShelper.pieces.ch8;
  SBUS.channels[8] = SBUShelper.pieces.ch9;
  SBUS.channels[9] = SBUShelper.pieces.ch10;
  SBUS.channels[10] = SBUShelper.pieces.ch11;
  SBUS.channels[11] = SBUShelper.pieces.ch12;
  SBUS.channels[12] = SBUShelper.pieces.ch13;
  SBUS.channels[13] = SBUShelper.pieces.ch14;
  SBUS.channels[14] = SBUShelper.pieces.ch15;
  SBUS.channels[15] = SBUShelper.pieces.ch16;
  SBUS.failsafe = SBUShelper.pieces.failsafe;

  SBUS.channelsReady = 1;

  return;
}



void SBUSprintstuff(){
/*
  Serial.println("input");
    for(byte i=0; i<23; i++)
  {
    Serial.print(SBUShelper.tempArray[i],HEX);
    Serial.print(",");
    //Serial1.write(timingArray[0][0][i]); //sends bytevalues to app
  }*/
/*
  Serial.println("struct");
  Serial.print('\n');
  Serial.print(SBUShelper.pieces.ch1);
  Serial.print(",");
  Serial.print(SBUShelper.pieces.ch2);
  Serial.print(",");
  Serial.print(SBUShelper.pieces.ch3);
  Serial.print(",");
  Serial.print(SBUShelper.pieces.ch4);
  Serial.print(",");
  Serial.print(SBUShelper.pieces.ch5);
  Serial.print(",");
  Serial.print(SBUShelper.pieces.ch6);
  Serial.print(",");
  Serial.print(SBUShelper.pieces.failsafe);
  Serial.print(",");
  Serial.print(SBUShelper.pieces.d17);
  Serial.println(",");

  Serial.println("bitshift");
  int16_t test1;
  int16_t test2;
  test1 = ((inputArray[0]|inputArray[1]<< 8) & 0x07FF);
  test2 = ((inputArray[11]|inputArray[12]<< 8) & 0x07FF);
  Serial.print( test1 );
  Serial.print(",");
  Serial.print( test2 );
  Serial.println(",");
*/
  
  

  for(byte i=0; i<15; i++)
  {
    Serial.print(SBUS.channels[i]);
    Serial.print("\t");
    //Serial1.write(timingArray[0][0][i]); //sends bytevalues to app
  }
  Serial.print("DDF:");
  Serial.print(SBUShelper.pieces.d17);
  Serial.print("\t");
  Serial.print(SBUShelper.pieces.d18);
  Serial.print("\t");
  Serial.print(SBUShelper.pieces.test);
  Serial.print("\t");
  Serial.println(SBUS.failsafe);
}
