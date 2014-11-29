

void config_init(){

  // Check if the firmware version is the same of eeprom config
  if (

    EEPROM.read(EE_ADDR_CODE_MAJOR_VERSION) == CODE_MAJOR_VERSION &&
    EEPROM.read(EE_ADDR_CODE_MINOR_VERSION) == CODE_MINOR_VERSION 
    ) {

    // loads in ram the eeprom config
    config_loadBackup_all();

  } 
  else {

    display_printResetting();

    // clear the eeprom
    for (unsigned int i = 0; i < EEPROM_SIZE; i++) {
      display_printProgressBar(i, EEPROM_SIZE);
      EEPROM.write(i, 0x00);
    }

    // writes sign codes
    EEPROM.write(EE_ADDR_CODE_MAJOR_VERSION,CODE_MAJOR_VERSION);
    EEPROM.write(EE_ADDR_CODE_MINOR_VERSION,CODE_MINOR_VERSION);

    // load defaults to ram and save on eeprom
    config_loadDefaults_all();
    config_saveBackup_all();
  } 
}

void config_loadDefaults_all(){
  
  config_loadDefaults_system();
  config_loadDefaults_sensorTriggerMode();
}

void config_saveBackup_all(){
  
  config_saveBackup_system();
  config_saveBackup_sensorTriggerMode();
}

void config_loadBackup_all(){
  config_loadBackup_system();
  config_loadBackup_sensorTriggerMode();
} 


// Load the system config from eeprom to ram
void config_loadBackup_system(){   
  
  system_sensorTuningMode = EEPROM.read(EE_ADDR_system_sensorTuningMode);
}


// Load the default system config to ram
void config_loadDefaults_system() {
  
  system_sensorTuningMode = DEFAULT_system_sensorTuningMode;
}

// Save the system config from ram to eeprom
void config_saveBackup_system(){ 
  
  EEPROM.write(EE_ADDR_system_sensorTuningMode, system_sensorTuningMode);
}


// Load the sensorTriggerMode from eeprom to ram
void config_loadBackup_sensorTriggerMode(){  
  
  sensorTriggerMode_sensorType = EEPROM.read(EE_ADDR_sensorTriggerMode_sensorType);
  sensorTriggerMode_sensorAudioLimit = eeprom_readInt(EE_ADDR_sensorTriggerMode_sensorAudioLimit);
  sensorTriggerMode_sensorLightLimit = eeprom_readInt(EE_ADDR_sensorTriggerMode_sensorLightLimit);
}

// Load the default sensorTriggerMode config to ram
void config_loadDefaults_sensorTriggerMode() {
  
  sensorTriggerMode_sensorType =         DEFAULT_sensorTriggerMode_sensorType;
  sensorTriggerMode_sensorAudioLimit =   DEFAULT_sensorTriggerMode_sensorAudioLimit;
  sensorTriggerMode_sensorLightLimit =   DEFAULT_sensorTriggerMode_sensorLightLimit;
}

// Save the sensorTriggerMode from ram to eeprom
void config_saveBackup_sensorTriggerMode(){ 

  EEPROM.write(EE_ADDR_sensorTriggerMode_sensorType, sensorTriggerMode_sensorType);
  eeprom_writeInt(EE_ADDR_sensorTriggerMode_sensorAudioLimit, sensorTriggerMode_sensorAudioLimit);
  eeprom_writeInt(EE_ADDR_sensorTriggerMode_sensorLightLimit, sensorTriggerMode_sensorLightLimit);

}
