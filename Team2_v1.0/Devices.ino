

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
  if (!digitalRead(PINS_RIGHT)&&!digitalRead(PINS_LEFT) &&
      !digitalRead(PINS_UP)&&!digitalRead(PINS_DOWN)) cancelFlag = false; 
  else cancelFlag = true;
}

// Scans keyboard in normal mode
void keyboard_scan() { 
   keyboard_scan(false);
}

void keyboard_scan(boolean quickmode) {
  
  int reading;
  int buttonUpState=LOW;             // the current reading from the Enter input pin
  int buttonDownState=LOW;             // the current reading from the input pin
  int buttonLeftState=LOW;             // the current reading from the input pin
  int buttonRightState=LOW;             // the current reading from the input pin

  reading = digitalRead(PINS_UP);
 
  if (reading != lastButtonUpState) {
    // reset the debouncing timer
    lastUpDebounceTime = millis();
  } 
  
  if ((millis() - lastUpDebounceTime) > KEY_DEBOUNCE_TIME) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonUpState=reading;
    lastUpDebounceTime=millis();
  }
  
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonUpState = reading;
  
  
  //Down button               
  // read the state of the switch into a local variable:
  reading = digitalRead(PINS_DOWN);
  
  // check to see if you just pressed the Down button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  
  
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonDownState) {
    // reset the debouncing timer
    lastDownDebounceTime = millis();
  } 
  
  if ((millis() - lastDownDebounceTime) > KEY_DEBOUNCE_TIME) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonDownState = reading;
    lastDownDebounceTime=millis();
  }
  
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonDownState = reading; 
  
     
   //Down button               
  // read the state of the switch into a local variable:
  reading = digitalRead(PINS_RIGHT);
  
  // check to see if you just pressed the Down button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  
  
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonRightState) {
    // reset the debouncing timer
    lastRightDebounceTime = millis();
  } 
  
  if ((millis() - lastRightDebounceTime) > KEY_DEBOUNCE_TIME) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonRightState = reading;
   lastRightDebounceTime =millis();
  }
  
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonRightState = reading;                  
  
  
  //Up button               
  // read the state of the switch into a local variable:
  reading = digitalRead(PINS_LEFT);
  
  // check to see if you just pressed the Down button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  
  
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonLeftState) {
    // reset the debouncing timer
    lastLeftDebounceTime = millis();
  } 
  
  if ((millis() - lastLeftDebounceTime) > KEY_DEBOUNCE_TIME) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonLeftState = reading;
    lastLeftDebounceTime=millis();;
  }
  
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonLeftState = reading;  
  
  //records which button has been pressed
  if (buttonUpState==HIGH){
    lastKey = KEY_UP;
  
  }else if(buttonDownState==HIGH){
    lastKey = KEY_DOWN;
  
  }else if(buttonRightState==HIGH){
    lastKey = KEY_RIGHT;
  
  }else if(buttonLeftState==HIGH){
    lastKey = KEY_LEFT;
  
  }else{
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

