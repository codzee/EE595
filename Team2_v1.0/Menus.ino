
// Runs the controller
void controller_run(){
  
   controller_showWelcome();
   
   while(true) {
     
       keyboard_scan();
     
     if(lastKey != KEY_NONE) {
        controller_showMainMenu();
        controller_showWelcome();
     }
   }
}

// Show welcome
void controller_showWelcome(){
   lcd.clear();
   display_printMessage(MSG_NAME);
   lcd.print(CODE_MAJOR_VERSION,10);
   lcd.print(".");
   lcd.print(CODE_MINOR_VERSION,10);
   lcd.setCursor(0,1);
   display_printMessage(MSG_READY);
}

// Main Menu View
void controller_showMainMenu(){
  
  byte currentOption = 0;
 
  for(boolean exit = false;!exit;){
      
    display_printTitle(MSG_MAIN_MENU);

    if(currentOption==0) display_printEnumeration(1, MSG_SENSOR_TRIGGER);
    if(currentOption==1) display_printEnumeration(2, MSG_SETTINGS); 
    
    keyboard_waitForAnyKey();
    
    if(lastKey==KEY_UP) circularList_incrementBy(&currentOption, 0, 1, -1);   
    if(lastKey==KEY_DOWN) circularList_incrementBy(&currentOption, 0, 1, 1);        
    if(lastKey==KEY_LEFT) exit = true;
    if(lastKey==KEY_RIGHT) {
                        
        if(currentOption==0) controller_showSensorTriggerMenu();
        if(currentOption==1) controller_showSystemConfig();    
    }
  }
}


// Sensor trigger menu view
void controller_showSensorTriggerMenu(){
  
  byte currentOption = 0;
  boolean skipOption = false;
  
  for(boolean exit = false;!exit;){
      
    display_printTitle(MSG_SENSOR_TRIGGER);

    // Show menu option
   
    if(currentOption==0) display_printEnumeration(1, MSG_RUN);
    if(currentOption==1) { display_printEnumeration(2, MSG_SENSOR); lcd.print(" "); display_printSensorType(sensorTriggerMode_sensorType); }  
    if(currentOption==2) { display_printEnumeration(3, MSG_SET_LIMIT); }
    if(currentOption==3) display_printEnumeration(4, MSG_RESET_CONFIG);
    
    
    if (!skipOption) keyboard_waitForAnyKey();
    else skipOption = false;
      
    if(lastKey==KEY_UP) { circularList_incrementBy(&currentOption, 0, 3, -1); }
    if(lastKey==KEY_DOWN) { circularList_incrementBy(&currentOption, 0, 3, 1); }
    if(lastKey==KEY_LEFT) exit = true;
    if(lastKey==KEY_RIGHT) {
                    
           if(currentOption==0)  controller_setSensorType(&sensorTriggerMode_sensorType);
           if(currentOption==1) { runAs_sensorTriggerMode(); }
           if(currentOption==2)  controller_setSensorLimit(sensorTriggerMode_sensorType); 
           if(currentOption==3) { if(controller_confirmAction(MSG_RESET_CONFIG)) {config_loadDefaults_sensorTriggerMode(); config_saveBackup_sensorTriggerMode(); display_printResetting();} }
      }
    }
  } 


// System config View
void controller_showSystemConfig(){
  
  byte currentOption = 0;
 
  for(boolean exit = false;!exit;){
      
    display_printTitle(MSG_SETTINGS);

    //////// Show menu option
    if(currentOption==0) { display_printMessage(MSG_SENSOR_TUNING); }
    if(currentOption==1) { display_printMessage(MSG_BLUETOOTH_PAIR); }
    if(currentOption==2) { display_printMessage(MSG_CAPTURE_TEST); }
    if(currentOption==3) { display_printMessage(MSG_MFG_TEST); }
    
    keyboard_waitForAnyKey();
    
    if(lastKey==KEY_UP) circularList_incrementBy(&currentOption, 0, 3, -1);
    if(lastKey==KEY_DOWN) circularList_incrementBy(&currentOption, 0, 3, 1);
    if(lastKey==KEY_LEFT) { config_saveBackup_system(); exit = true; } 
    if(lastKey==KEY_RIGHT) {
          
      if(currentOption==0) controller_setSensorTuningMode(&system_sensorTuningMode);
      if(currentOption==1) controller_setSensorTuningMode(&system_sensorTuningMode);
      if(currentOption==2) controller_setSensorTuningMode(&system_sensorTuningMode);
      if(currentOption==3) controller_setSensorTuningMode(&system_sensorTuningMode);
    }
  }
}

// set of sensorlimit
void controller_setSensorLimit(byte sensorType){
  
  if(system_sensorTuningMode==SENSOR_TUNING_VISUAL) controller_setSensorLimitVisual(sensorType);
  if(system_sensorTuningMode==SENSOR_TUNING_NUMERIC) controller_setSensorLimitNumeric(sensorType);
}


// set of sensorlimit in visual mode
void controller_setSensorLimitVisual(byte sensorType){

     unsigned int sensorValue;
     unsigned int *sensorLimit;
     byte sensorPin;
     byte sensorMode;
     

     sensor_getConfiguration(sensorType, &sensorLimit, &sensorPin, &sensorMode);
     
     display_printTitle(MSG_SENSOR_LIMIT);

     do { 
          sensorValue = sensor_read(sensorPin);
          byte sensorValueEquivalent = map(sensorValue, 0, 999, 0, LCD_COLS-1);
          byte sensorLimitEquivalent = map(*sensorLimit, 0, 999, 0, LCD_COLS-1);
          
          lcd.setCursor(0,1);
          
          for (byte i=0; i < LCD_COLS; i++) {
              if(i==sensorLimitEquivalent){
                 if (sensorMode==SENSOR_MODE_HIGHER){
                     if (sensorValueEquivalent >= sensorLimitEquivalent); //buzzer_beep(100); 
                     lcd.write((uint8_t)SYMBOL_UP);
                 } 
                 else                  
                 if (sensorMode==SENSOR_MODE_LOWER){
                     if (sensorValueEquivalent <= sensorLimitEquivalent); // buzzer_beep(100); 
                     lcd.write((uint8_t)SYMBOL_DOWN);
                 }
              }
              if(i < sensorValueEquivalent) lcd.write((uint8_t)SYMBOL_BOX); else lcd.print(" ");
          }
          
          keyboard_scan();
             
          if (lastKey==KEY_UP) circularList_incrementBy(sensorLimit, 0, 999, 64);
          if (lastKey==KEY_DOWN) circularList_incrementBy(sensorLimit, 0, 999, -64);
     
     } while (lastKey!=KEY_LEFT);
}

// set of sensorlimit in numeric mode
void controller_setSensorLimitNumeric(byte sensorType){

     unsigned int sensorValue = 0;
     unsigned int previousSensorValue = 0;
     unsigned int *limitValue;
     byte currentPosition = 2;
     byte sensorPin;
     byte sensorMode;
       
     sensor_getConfiguration(sensorType, &limitValue, &sensorPin, &sensorMode);    
     
     sensorValue = sensor_read(sensorPin);
     
     lcd.clear();
     display_printMessage(MSG_SENSOR_VALUE);
     lcd.setCursor(0,1);
     display_printMessage(MSG_LIMIT_VALUE);
     
     if (sensorMode==SENSOR_MODE_HIGHER) {
             lcd.setCursor(15,1);
             lcd.write((uint8_t)SYMBOL_UP);
     }
     if (sensorMode==SENSOR_MODE_LOWER) {
             lcd.setCursor(15,1);
             lcd.write((uint8_t)SYMBOL_DOWN);
     }       
                    
     lcd.setCursor(9,0);
     display_leadingZeroNumber(sensorValue, 3);
          
     lcd.setCursor(9,1);
     display_leadingZeroNumber(*limitValue, 3);
     
     keyboard_waitForNokey();

     do { 
         
           sensorValue = sensor_read(sensorPin);
                    
           if (sensorValue != previousSensorValue) {
             lcd.noBlink();
             lcd.setCursor(9,0);
             display_leadingZeroNumber(sensorValue, 3);
             lcd.setCursor(9+currentPosition,1);
             lcd.blink();
           }
        
           keyboard_scan();
           
           if (lastKey!=KEY_NONE) controller_setUnsignedIntValue(limitValue, &currentPosition, 3, 9, 1);     
          
           if (((sensorMode==SENSOR_MODE_HIGHER && sensorValue >= *limitValue) || (sensorMode==SENSOR_MODE_LOWER  && sensorValue <= *limitValue)) && lastKey==KEY_NONE);// buzzer_beep(100); 
           else delay(20);
          
           previousSensorValue = sensorValue;
          
     } while (lastKey!=KEY_LEFT);
     
     lcd.noBlink();
}


// set of sensor tunning mode
void controller_setSensorTuningMode(byte *value){
  
     do { 
       
      display_printTitle(MSG_SENSOR_TUNING);
      display_printInterfaceSensorMode(*value);
      
      keyboard_waitForAnyKey();
      
      if (lastKey==KEY_UP) circularList_incrementBy(value, 0, 1, -1);
      if (lastKey==KEY_DOWN) circularList_incrementBy(value, 0, 1, 1);
   
     } while (lastKey!=KEY_LEFT);     
}


// set of sensor type
void controller_setSensorType(byte *value){
  
  do { 
       
      display_printTitle(MSG_SENSOR_TYPE);
      display_printSensorType(*value);
      
      keyboard_waitForAnyKey();
      
      if (lastKey==KEY_UP) circularList_incrementBy(value, 0, 4, -1);
      if (lastKey==KEY_DOWN) circularList_incrementBy(value, 0, 4, 1);
   
     } while (lastKey!=KEY_LEFT); 
}

// set of numeric parameter value
void controller_setNumericParameterValue(unsigned int *value, const prog_char* msg, byte units){
     
     lcd.clear();
     lcd.print(">");
     display_printMessage(msg);
     
     lcd.setCursor(6,1);
     display_printUnits(units);
     
     lcd.setCursor(0,1);     
     display_leadingZeroNumber(*value, 5);
     
     byte currentPosition = 4;
     lcd.setCursor(currentPosition,1);
     
     
     do{
       keyboard_scan();
       if (lastKey!=KEY_NONE) controller_setUnsignedIntValue(value, &currentPosition, 5, 0, 1);     
     
     } while(lastKey!=KEY_LEFT);
     
}

// set of unsigned integer value assisted by other function.
void controller_setUnsignedIntValue(unsigned int *value, byte *currentPos, byte positions, byte col, byte row){
     
     if (positions>5) positions = 5;
     byte values[positions];

     byte counter = 0;
       
     if (positions>4) { values[counter] = *value/10000; counter++; }
     if (positions>3) { values[counter] = (*value%10000)/1000; counter++; }
     if (positions>2) { values[counter] = (*value%1000)/100; counter++; }
     if (positions>1) { values[counter] = (*value%100)/10; counter++; }
     if (positions>0) { values[counter] = *value%10; counter++; }
    
      if (lastKey==KEY_UP) {
           
             if (*currentPos == 0 && positions == 5) circularList_incrementBy(&values[*currentPos],0,5, 1);
             else circularList_incrementBy(&values[*currentPos],0,9, 1); 
      }
      if (lastKey==KEY_DOWN) {
        
             if (*currentPos == 0 && positions == 5) circularList_incrementBy(&values[*currentPos],0,5, -1);
             else circularList_incrementBy(&values[*currentPos],0,9, -1);
      }
      if (lastKey==KEY_DOWN) circularList_incrementBy(currentPos,0, positions-1, -1);             
     
     counter = positions-1;
     *value = 0;
     
     if (positions>0) { *value = *value + values[counter]; counter--; }
     if (positions>1) { *value = *value + values[counter]*10; counter--; }
     if (positions>2) { *value = *value + values[counter]*100; counter--; }
     if (positions>3) { *value = *value + values[counter]*1000; counter--; }
     if (positions>4) { *value = *value + values[counter]*10000; }     
     
   
     lcd.setCursor(col+*currentPos,row);
     lcd.print(values[*currentPos],10);
     lcd.setCursor(col+*currentPos,row);
}

// Alert for confirm action
boolean controller_confirmAction(const prog_char* str){
  
     boolean confirm = false;
     
     do { 
       
      display_printQuestion(str);
    
      lcd.setCursor(2,1);
     
      if(confirm) lcd.print(" "); else lcd.print("<");
      display_printMessage(MSG_NO);
      if(confirm) lcd.print(" "); else lcd.print(">");
     
      lcd.setCursor(9,1);
     
      if(confirm) lcd.print("<"); else lcd.print(" ");
      display_printMessage(MSG_YES);
      if(confirm) lcd.print(">"); else lcd.print(" ");      
       
      keyboard_waitForAnyKey();
      
      if (lastKey==KEY_RIGHT || lastKey==KEY_LEFT) { confirm = !confirm;  };
   
     } while (lastKey!=KEY_RIGHT);   
     
     return confirm;  
}


