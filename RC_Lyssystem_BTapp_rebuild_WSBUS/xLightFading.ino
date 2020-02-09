
//Experimental. Design scratch pad for light fading functions. NOT IN USE
#if 0

 - if only xenon flash on low beam at startup: -
in ignition state 2:
xenonWarming =1 


if index > sizeof(valuesArray
  xenonWarming = 0;
  index = 0;
  return 255
value = valuesArray[index]
index++
return value

 - if xenon ballast warms up and cools down: - (needs to check per function or per pin)


if function flag == 0 && xenonWarmingArray[frame] > 0
    if runCount % 8 > 0 // ca 25 sec cooldown
      xenonWarmingArray[frame] -= 1
      
 
 - fadeUp: - (per pin)
if fade up in progress for this pin
  maxValueFromFading = fadeUp(frame);

checking if fade up is in progress: if fadeStateArray is 'u'?

check if pin should fade up: fadeTypeArray

activate fadeup for this pin:
check if high
check if should fadeup // if fadetype 'u' set fadestate 'u'
check if fadeup active // dont check, just do?
if high && fadetype 'u'
  fadestate 'f'

if fadestate 'f'
  maxValueFromFading = fadeUp(frame);

//if fadeIndex > sizeof
  fadestate = 0
  index = 0
  return 255

if fadevalue == 255
  fadestate
  return

fadevalue++
//every loop inc index







in ignition:
ignitionState 2 ->
  xenonWarming = 1;

maxValueFromFading = 255;
minValueFromFading = 0;
maxValueFromPWM = 255;
minValueFromPWM = 0;

fadeUp -> if fadeState 'u'

if (fadeDown)
  minValueFromFading = //0-255

if (fadeUp)
  maxValueFromFading = //0-255

if (xenonWarming)
  maxValueFromFading = xenonFlash(frame);

if (pwmValue == 'h')
  maxValueFromPWM = //255 or half

else if (pwmValue == 'l')
  minValueFromPWM = //0 or half



if (workingLightHigh)
{
  outputValue = max( min(maxValueFromFading,maxValueFromPWM) , minValueFromPWM );
}
else
{
  //outputValue = max(0,minValueFromPWM);
  outputValue = max( minValueFromFading , minValueFromPWM );
}



  

if (digital_pin_to_timer_PGM[frame] == NOT_ON_TIMER )
{
  digitalWrite(outputLedArray[frame],outputValue);
}
else
{
  analogWrite(outputLedArray[frame],outputValue);
}

else if (workingLightPWM == 'x') //if inactive = pwm based
            {
                if (workingLightHigh)
                    analogWrite(outputLedArray[frame],xenonFlash);
                else
                {
                    //Serial.println('l');
                    analogWrite(outputLedArray[frame],LOW);
                }
                
            }

            else //if no pwm (can skip maybe?)
            {
                if (workingLightHigh)
                    digitalWrite(outputLedArray[frame],HIGH);
                else
                    digitalWrite(outputLedArray[frame],LOW);
            }

            if (digital_pin_to_timer_PGM[frame] == NOT_ON_TIMER )
              digitalWrite(frame,HIGH);
            



byte xenonFlash (  byte currentPin )
{
  //outputLEDfadeIndexArray[numberOfPins] = { 0,0,0,0,0,0,0,0,0 };
  //outputLEDfadeStateArray[numberOfPins] = { // '0' off, '1' on, 'x' doing xenon, 'u' doing fade up, 'd' doing fade down, 's' doing sine sweep
  //xenonFlashConstants = { 50,100,200,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200,210,220,230,240,250,255 };
  byte returnValue
  
  if (outputLEDfadeStateArray[currentPin] == 0)
    outputLEDfadeIndexArray[currentPin] = 0;
  else
  {

    returnValue = xenonFlashConstants[ outputLEDfadeIndexArray[currentPin];
    outputLEDfadeIndexArray[currentPin] = returnValue++;
    if (outputLEDfadeIndexArray[currentPin] >= 255)
      outputLEDfadeStateArray[currentPin] = '1';
    return returnValue;

  }
    
}

byte fadeOff ( byte currentPin )
{
  
}

*/









#endif
