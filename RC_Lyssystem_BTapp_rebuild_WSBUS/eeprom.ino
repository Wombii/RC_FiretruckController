

void savesetup(void)
{
  byte toSave;
  //reverseDelay /= 10;

  eeprom_write_byte ((uint8_t*)1,throttleAxisReversed); //byte
  delay(5);
  eeprom_write_byte ((uint8_t*)2,steeringAxisReversed);
  delay(5);
  eeprom_write_byte ((uint8_t*)3,reverseType2);
  delay(5);
  eeprom_write_byte ((uint8_t*)4,neutralUpperOffset);
  delay(5);
  eeprom_write_byte ((uint8_t*)5,neutralLowerOffset);
  delay(5);

  eeprom_write_block((const void*)settingsArray2, (void*)40,sizeof(settingsArray2)); //sourcearray,address,(size 21*2*2) sizeof(settingsArray2)
  delay(20);
  eeprom_write_block((const void*)timingArray, (void*)200,sizeof(timingArray)); //21*8 =168
  delay(20);
  eeprom_write_block((const void*)fadeSettingsArray, (void*)400,sizeof(fadeSettingsArray)); //21*2 = 42
  delay(20);
  /*
  eeprom_write_word ((uint16_t*)0,reverseDelay);
  delay(5);
  signalDelay /= 10;
  eeprom_write_byte ((uint8_t*)2,signalDelay);
  signalDelay *= 10;
  delay(5);
  //auxMaxTXDelay /= 10;
  //eeprom_write_byte ((uint8_t*)3,auxMaxTXDelay);
  //auxMaxTXDelay *= 10;
  delay(5);
  byte switchByte = 0;
  //Serial.print(throttleAxisReversed);
  //Serial.print("   ");
  switchByte |= (throttleAxisReversed << 0);
  //Serial.print(steeringAxisReversed);
  //Serial.print("   ");
  switchByte |= (steeringAxisReversed << 1);
  //Serial.print(reverseType2);
  //Serial.print("   ");
  switchByte |= (reverseType2 << 2);
  //Serial.println(switchByte);
  
  
  eeprom_write_byte ((uint8_t*)4,switchByte);
  delay(5);
  
  eeprom_write_byte ((uint8_t*)12,gear_first);
  delay(5);
  eeprom_write_byte ((uint8_t*)13,gear_second);
  delay(5);
  eeprom_write_byte ((uint8_t*)14,gear_third);
  delay(5);
//  eeprom_write_byte ((uint8_t*)15,var1);
 // delay(5);
 // eeprom_write_byte ((uint8_t*)16,var2);
 // delay(5);
  //Serial.write('e');
  Serial.write(array,13);
  eeprom_write_block((const void*)array, (void*)40,13);
  delay(20);
  */
}

/*
void savecalibration (void)
{
  eeprom_write_word ((uint16_t*)6,throttleNeutralUpperLimit);
  delay(5);
  eeprom_write_word ((uint16_t*)8,throttleNeutralLowerLimit);
  delay(5);
  eeprom_write_word ((uint16_t*)10,reverseLimit);
  delay(5);
}
*/

void saveVersion (byte number)
{
  eeprom_write_byte ((uint8_t*)0,number);
  delay(5);
}

void saveEepromNumberOfPorts (byte number)
{
  eeprom_write_byte ((uint8_t*)6,number);
  delay(5);
}

byte readVersion (void)
{
  return eeprom_read_byte((uint8_t*)0);
}

byte readEepromNumberOfPorts (void)
{
  return eeprom_read_byte((uint8_t*)6);
}



void readeeprom(void)
{
  
  throttleAxisReversed = eeprom_read_byte((uint8_t*)1); //byte
  steeringAxisReversed = eeprom_read_byte((uint8_t*)2);
  reverseType2 = eeprom_read_byte((uint8_t*)3);
  neutralUpperOffset = eeprom_read_byte((uint8_t*)4);
  neutralLowerOffset = eeprom_read_byte((uint8_t*)5);
  // 6 reserved for numberOfPorts
  
  //eeprom_write_block((const void*)settingsArray2, (void*)40,40); //address,size 10*2*2
  //eeprom_write_block((const void*)timingArray, (void*)100,80);

  eeprom_read_block((void*)settingsArray2, (const void*)40,NumberOfPorts*2*2); //14*2*2
  eeprom_read_block((void*)timingArray, (const void*)200,NumberOfPorts*4*2);
  eeprom_read_block((void*)fadeSettingsArray, (const void*)400,NumberOfPorts*2);
/*
  reverseDelay = eeprom_read_word((uint16_t*)0);
  //reverseDelay *= 10;
  
  signalDelay = eeprom_read_byte((uint8_t*)2);
  signalDelay *= 10;
  
  auxMaxTXDelay = eeprom_read_byte((uint8_t*)3);
  auxMaxTXDelay *= 10;
  
  byte switchByte = 0;
  switchByte = eeprom_read_byte((uint8_t*)4);
  //Serial.print(switchByte);
  //Serial.print("   ");
  throttleAxisReversed = (switchByte & (1<<0));
  //Serial.print(throttleAxisReversed);
  //Serial.print("   ");
  steeringAxisReversed = (switchByte & (1<<1));
  //Serial.print(steeringAxisReversed);
  //Serial.print("   ");
  reverseType2 = (switchByte & (1<<2));
  //Serial.println(reverseType2);


  throttleNeutralUpperLimit = eeprom_read_word((uint16_t*)6);
  throttleNeutralLowerLimit = eeprom_read_word((uint16_t*)8);
  reverseLimit = eeprom_read_word((uint16_t*)10);
  
  gear_first = eeprom_read_byte((uint8_t*)12);
  gear_second = eeprom_read_byte((uint8_t*)13);
  gear_third = eeprom_read_byte((uint8_t*)14);
  //var1 = eeprom_read_byte((uint8_t*)15);
 // var2 = eeprom_read_byte((uint8_t*)16);
 
  
  eeprom_read_block((void*)array, (const void*)40,13);

  */
}


/*frame = 0;
location = 40;
        while(frame <= 13){
          //showString(PSTR("Please enter type of light on pin ")); //testsetup: 00000000809521
          //Serial.print(frame);
          //Serial.print(": ");
          array[frame] = serialrx();
          frame++;*/
