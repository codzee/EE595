

// Reads the value of a sensor
unsigned int sensor_read(byte sensorPin) {
  
  unsigned int value = analogRead(sensorPin);
  if (value>999) value = 999;
 
  if (sensorPin == PINS_SENSOR_MIC) {
    if (value > 500) value = (value-500)*2;
    else value = (500-value)*2;
  } 
 
  if (sensorPin == PINS_SENSOR_PHOTO_D) {
    if (value > 500) value = (value-500)*2;
    else value = (500-value)*2;
  } 
  
  return value;
}

// Sets the sensor pin and sensor mode according the sensor type
void sensor_getConfiguration(byte sensorType, unsigned int **limitValue, byte *sensorPin, byte *sensorMode) {
   
   if (sensorType == SENSOR_TYPE_NONE) {  }
   else if (sensorType == SENSOR_TYPE_AUDIO) { *sensorPin = PINS_SENSOR_MIC; *sensorMode = SENSOR_MODE_HIGHER; *limitValue = &sensorTriggerMode_sensorAudioLimit; }
   else if (sensorType == SENSOR_TYPE_LIGHT) { *sensorPin = PINS_SENSOR_PHOTO_D; *sensorMode = SENSOR_MODE_HIGHER; *limitValue = &sensorTriggerMode_sensorLightLimit;}
 
}

// Waits for sensor cross the limit
boolean sensor_waitFor(byte sensorType, unsigned int limitTime){
  
  boolean result = false;
  
  if(sensorType != SENSOR_TYPE_NONE){
    
    byte sensorPin;
    byte sensorMode;
    unsigned int *limitValue;
    sensor_getConfiguration(sensorType, &limitValue, &sensorPin, &sensorMode);
    
    unsigned long time = millis();
    unsigned int sensorValue = sensor_read(sensorPin);
  
    for(; !result && !cancelFlag && (millis()<time+limitTime || limitTime==0); sensorValue = sensor_read(sensorPin)) {
      if (sensorMode==SENSOR_MODE_HIGHER && sensorValue>=*limitValue) result = true;
      if (sensorMode==SENSOR_MODE_LOWER && sensorValue<=*limitValue) result = true;
    }
    if(cancelFlag) result = true;
  
  } else {
  
    result = true;
  
  }
  
  return result;
}

// Called in interrupt mode when any button is pressed
void keyboard_interrupts(){
  if (!digitalRead(PINS_BTN_A)&&!digitalRead(PINS_BTN_B)) cancelFlag = false; 
  else cancelFlag = true;
}

// Scans keyboard in normal mode
void keyboard_scan() { 
   keyboard_scan(false);
}

// Scans keyboard buttons
void keyboard_scan(boolean quickmode) { 

  unsigned long time = millis();  
  
  if (flagHoldKey && !quickmode) { 
     
      while(digitalRead(PINS_BTN_A) || digitalRead(PINS_BTN_B)) {}
      flagHoldKey = false;
      lastKey = KEY_NONE; 
      
  } else if (digitalRead(PINS_BTN_A)) {
     while(digitalRead(PINS_BTN_A) && (millis()-time) <= KEY_HOLD_TIME+KEY_HOLD_TIME_WAIT){  
        if (millis()-time >= KEY_DEBOUNCE_TIME) lastKey = KEY_UP;  
        if (millis()-time >= KEY_HOLD_TIME) { if(digitalRead(PINS_BTN_B)) lastKey = KEY_SBH; else lastKey = KEY_BACK; flagHoldKey = true; }
     }
     
  } else if (digitalRead(PINS_BTN_B)) {
     while(digitalRead(PINS_BTN_B) && (millis()-time) <= KEY_HOLD_TIME+KEY_HOLD_TIME_WAIT){  
        if (millis()-time >= KEY_DEBOUNCE_TIME)  lastKey = KEY_BACK;  
        if (millis()-time >= KEY_HOLD_TIME) { if(digitalRead(PINS_BTN_A)) lastKey = KEY_SBH; else lastKey = KEY_SELECT; flagHoldKey = true; }
     }    
  } else {
    flagHoldKey = false;
    lastKey = KEY_NONE;
  }
  
}

// Waits until any key is pressed
void keyboard_waitForAnyKey(){
   do{ keyboard_scan(); } while (lastKey==KEY_NONE);
}

// Waits until no key is pressed
void keyboard_waitForNokey(){
   do{ keyboard_scan(); } while (lastKey!=KEY_NONE);
}

// Print on lcd a progmem string
void display_printMessage(const prog_char* str) {
  char c;
  if (!str) return;
  while ((c = pgm_read_byte(str))) {
    lcd.print(c);
    str++;
  }
}

// prints screen title
void display_printTitle(const prog_char* str){
    lcd.clear();
    lcd.print(">");
    display_printMessage(str);
    lcd.setCursor(0,1);
}

// prints screen title
void display_printQuestion(const prog_char* str){
    lcd.clear();
    lcd.print(">");
    display_printMessage(str);
    lcd.print("?");
    lcd.setCursor(0,1);
}

void display_printEnumeration(byte num, const prog_char* str){
    lcd.print(num, 10);
    lcd.print(".");
    display_printMessage(str);
}

// prints aborting message
void display_printResetting(){
  lcd.clear();
  display_printMessage(MSG_RESETTING);
}

// prints aborting message
void display_printAborting(){
  lcd.clear();
  display_printMessage(MSG_ABORTING);
}

// prints a boolean value
void display_printBoolean(boolean value) {
  lcd.print(" (");
  if (value) display_printMessage(MSG_YES); else display_printMessage(MSG_NO);
  lcd.print(")");
}

// Print units value
void display_printUnits(byte units){
  lcd.print("(");
  if(units==UNITS_US) display_printMessage(MSG_UNITS_US);
  if(units==UNITS_MS) display_printMessage(MSG_UNITS_MS);     
  if(units==UNITS_SECS) display_printMessage(MSG_UNITS_SECS);
  if(units==UNITS_MINS) display_printMessage(MSG_UNITS_MINS);
  if(units==UNITS_HOURS) display_printMessage(MSG_UNITS_HOURS);
  if(units==UNITS_DAYS) display_printMessage(MSG_UNITS_DAYS);

  lcd.print(")");
}

// Print sesnor type value
void display_printSensorType(byte type){
  lcd.print("(");
  if(type==SENSOR_TYPE_NONE) display_printMessage(MSG_NONE);
  if(type==SENSOR_TYPE_AUDIO) display_printMessage(MSG_SENSOR_TYPE_AUDIO);
  if(type==SENSOR_TYPE_LIGHT) display_printMessage(MSG_SENSOR_TYPE_LIGHT);
  if(type==SENSOR_TYPE_LIGHTNING) display_printMessage(MSG_SENSOR_TYPE_LIGHTNING);
  lcd.print(")");
}

// Print sensor tunning modes
void display_printInterfaceSensorMode(byte mode){
  
  lcd.print("(");
  if(mode==SENSOR_TUNING_VISUAL) display_printMessage(MSG_SENSOR_TUNING_VISUAL);
  if(mode==SENSOR_TUNING_NUMERIC) display_printMessage(MSG_SENSOR_TUNING_NUMERIC);     
  lcd.print(")");
}

// Print a progressbar
void display_printProgressBar(int howMany, int total){
  
    byte positions = map(howMany, 0, total, 0, LCD_COLS);
    if(positions!=lastProgressBarValue){
      lcd.setCursor(0,1);
      for(byte i=0;i<=positions && positions>0;i++){
        lcd.write(SYMBOL_BOX);
      }  
    }
    lastProgressBarValue = positions;
 
}

// Print units value
void display_printDevicePortType(byte type){
  lcd.print("(");
  if(type==DEVICE_PORT_TYPE_NONE) display_printMessage(MSG_NONE);
//  if(type==DEVICE_PORT_TYPE_LASER) display_printMessage(MSG_DEVICE_PORT_TYPE_LASER);
  lcd.print(")");
}

// Print a number of any positions with leading zeros.
void display_leadingZeroNumber(unsigned int number, byte positions){
  
     if(positions>4) lcd.print(number/10000,10);
     if(positions>3) lcd.print((number%10000)/1000, 10);
     if(positions>2) lcd.print((number%1000)/100, 10);
     if(positions>1) lcd.print((number%100)/10, 10);
     lcd.print((number%10), 10);
  
}

