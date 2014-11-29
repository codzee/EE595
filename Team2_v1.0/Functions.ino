
// Write a unsigned int (two bytes) value to eeprom
void eeprom_writeInt(unsigned int address, unsigned int value){

  EEPROM.write(address, value % 256); // LSB
  EEPROM.write(address+1, value/256); // MSB

}

// read a unsigned int (two bytes) value from eeprom
unsigned int eeprom_readInt(unsigned int address){

  return EEPROM.read(address)+EEPROM.read(address+1)*256;

}


// increment the value of a variable using minimun and maximun limits (byte version)
void circularList_incrementBy(byte *value, byte minimun, byte maximun, int incrementBy) {

  if (incrementBy>0) {

    if (*value+incrementBy > maximun) *value = minimun; 
    else *value = *value+incrementBy;

  } 
  else {

    if ( (*value < minimun) || (*value < (incrementBy*(-1)) )) *value = maximun;
    else *value = *value+incrementBy;

  }

}

// increment the value of a variable using minimun and maximun limits (unsigned int version)
void circularList_incrementBy(unsigned int *value, unsigned int minimun, unsigned int maximun, int incrementBy) {

  if (incrementBy>0) {

    if (*value+incrementBy > maximun) *value = minimun; 
    else *value = *value+incrementBy;

  } 
  else {

    if ( (*value < minimun) || (*value < (incrementBy*(-1)) )) *value = maximun;
    else *value = *value+incrementBy;

  }
}

// increment the value of a variable using minimun and maximun limits (unsigned int version)
void circularList_matchBy(unsigned int *value, unsigned int targetValue, unsigned int minimun, unsigned int maximun, int offset) {

  if (offset>0) {

    if (targetValue+offset > maximun) *value = maximun; 
    else *value = targetValue+offset;

  } 
  else {

    if ( targetValue < (offset*(-1)) ) *value = minimun;
    else *value = targetValue+offset;

  }  
}






