

// Import libraries 
#include <avr/pgmspace.h>  // For messages in flash memory
#include <LiquidCrystal.h> // For LCD Display use 
#include <EEPROM.h>        // For persistent config 



// Valid values:
// - SENSOR_TYPE_AUDIO
// - SENSOR_TYPE_LIGHT

#define DEFAULT_SENSOR_TYPE                     SENSOR_TYPE_NONE

// Default sensor tunning mode:
// Valid values:
// - SENSOR_TUNNING_VISUAL
// - SENSOR_TUNNING_NUMERIC

#define DEFAULT_SENSOR_TUNING_MODE              SENSOR_TUNING_VISUAL

// Version Codes 
#define CODE_MAJOR_VERSION          1  // major version
#define CODE_MINOR_VERSION          0  // minor version

// Default system config
#define DEFAULT_system_sensorTuningMode               DEFAULT_SENSOR_TUNING_MODE

// Audio trigger mode default config
#define DEFAULT_sensorTriggerMode_sensorType          DEFAULT_SENSOR_TYPE
#define DEFAULT_sensorTriggerMode_sensorAudioLimit    500
#define DEFAULT_sensorTriggerMode_sensorLightLimit    500

// LCD settings
#define LCD_COLS             16 // columns
#define LCD_ROWS             2  // rows

// Pinout settings
#define PINS_LCD_LED         6 //(digital pin)
#define PINS_LCD_RS          5 //(digital pin)
#define PINS_LCD_ENABLE      4 //(digital pin)
#define PINS_LCD_DB4         3 //(digital pin)
#define PINS_LCD_DB5         9  //(digital pin)
#define PINS_LCD_DB6         8  //(digital pin)
#define PINS_LCD_DB7         7  //(digital pin)
#define PINS_BTN_A           3  //(digital pin)
#define PINS_BTN_B           2  //(digital pin)
#define PINS_UP              
#define PINS_DOWN

#define PINS_SENSOR_MIC      3  //(analog pin)
#define PINS_SENSOR_PHOTO_D  1  //(analog pin)

// Sensor types
#define SENSOR_TYPE_NONE                 0 // None sensor
#define SENSOR_TYPE_AUDIO                1 // Audio sensor
#define SENSOR_TYPE_LIGHT                2 // Light sensor
#define SENSOR_TYPE_LIGHTNING            3 // Lightning Sensor

// Sensor tunning modes
#define SENSOR_TUNING_VISUAL             0 // Visual 
#define SENSOR_TUNING_NUMERIC            1 // Numeric 

// Sensor limit modes
#define SENSOR_MODE_LOWER                0 // for triggering by lower values
#define SENSOR_MODE_HIGHER               1 // for triggering by higher values

// Device type
#define DEVICE_PORT_TYPE_NONE            0 // No device connected

// Measurement units
#define UNITS_MS             0  // miliseconds
#define UNITS_SECS           1  // seconds
#define UNITS_MINS           2  // minutes
#define UNITS_HOURS          3  // hours
#define UNITS_DAYS           4  // days
#define UNITS_CYCLES         10 // cycles
#define UNITS_US             11 // microseconds

// Keycodes
#define KEY_NONE             0 // No keys pressed
#define KEY_UP               1 // Button A was pressed
#define KEY_DOWN             2 // Button B was pressed
#define KEY_BACK             3 // Button A was pressed and held (KEY_HOLD_TIME) milisecons
#define KEY_SELECT           4 // Button B was pressed and held (KEY_HOLD_TIME) milisecons
#define KEY_SBH              5 // Buttons A+B was pressed and holded (KEY_HOLD_TIME) milisecons


// Keyboard times
#define KEY_DEBOUNCE_TIME    30 // debounce time (ms) to prevent flickering when pressing or releasing the button
#define KEY_HOLD_TIME       400 // holding period (ms) how long to wait for press+hold event
#define KEY_HOLD_TIME_WAIT  100 // Used for double key holding

// Characters and symbols addresses on lcd eeprom
#define SYMBOL_DOWN         0x00 // Down arrow
#define SYMBOL_UP           0x01 // Up arrow
#define SYMBOL_BOX          0xff // Black box

// EEPROM size in bytes
#define EEPROM_SIZE         0x200  

// EEPROM Addresses for signature code and version of firmware
#define EE_ADDR_CODE_MAJOR_VERSION                   0x00 // BYTE
#define EE_ADDR_CODE_MINOR_VERSION                   0x01 // BYTE

// EEPROM Addresses for system config
#define EE_ADDR_system_sensorTuningMode              0x10 // BYTE
#define EE_ADDR_system_devicePortType                0x12 // BYTE

// EEPROM Addresses for sensor trigger mode config
#define EE_ADDR_sensorTriggerMode_sensorType         0x30 // BYTE
#define EE_ADDR_sensorTriggerMode_sensorAudioLimit   0x32 // WORD
#define EE_ADDR_sensorTriggerMode_sensorLightLimit   0x34 // WORD


// LiquidCrystal LCD control object instance
LiquidCrystal lcd(PINS_LCD_RS, PINS_LCD_ENABLE, PINS_LCD_DB4, PINS_LCD_DB5, PINS_LCD_DB6, PINS_LCD_DB7);

// Variables used on interrupt mode
volatile boolean cancelFlag = false;    // Flag used to abort interrupt mode


// Global variables
byte         lastKey = KEY_NONE;        // Last key pressed
boolean      flagHoldKey = false;       // Flag to ignore keys after a hold key
byte         lastProgressBarValue = 0;  // Last value of progress bar

// System config in ram   
byte         system_sensorTuningMode;
byte         system_devicePortType;


// Sensor trigger mode config in ram
byte         sensorTriggerMode_sensorType;
unsigned int sensorTriggerMode_sensorAudioLimit;
unsigned int sensorTriggerMode_sensorLightLimit;


const prog_char PROGMEM MSG_BLUETOOTH_PAIR[] =                    "Pair Bluetooth";
const prog_char PROGMEM MSG_CAPTURE_TEST[] =                      "Capture Test";
const prog_char PROGMEM MSG_NAME[] =                            "   Team2 v";
const prog_char PROGMEM MSG_READY[] =                             "Push Any Button";
const prog_char PROGMEM MSG_MAIN_MENU[] =                         "Main Menu";
const prog_char PROGMEM MSG_SETTINGS[] =                          "Settings";
const prog_char PROGMEM MSG_SENSOR_TRIGGER[] =                    "Sensor Trigger";
const prog_char PROGMEM MSG_CONFIG[] =                            "Config";
const prog_char PROGMEM MSG_CONFIG_TRIGGER[] =                    "Config Trigger";
const prog_char PROGMEM MSG_RUN[] =                               "Run";
const prog_char PROGMEM MSG_RUNNING[] =                           "Running...";
const prog_char PROGMEM MSG_RESET_CONFIG[] =                      "Reset Config";
const prog_char PROGMEM MSG_SENSOR[] =                            "Sensor";
const prog_char PROGMEM MSG_SENSOR_LIMIT[] =                      "Sensor Limit";
const prog_char PROGMEM MSG_SET_LIMIT[] =                          "Set Limit";
const prog_char PROGMEM MSG_SENSOR_VALUE[] =                      ">Sensor :";
const prog_char PROGMEM MSG_LIMIT_VALUE[] =                       ">Limit  :";
const prog_char PROGMEM MSG_RESETTING[] =                         "Resetting...";
const prog_char PROGMEM MSG_ABORTING[] =                          "Aborting...";
const prog_char PROGMEM MSG_YES[] =                               "YES";
const prog_char PROGMEM MSG_NO[] =                                "NO";
const prog_char PROGMEM MSG_NONE[] =                              "None";
const prog_char PROGMEM MSG_UNITS_US[] =                          "us";
const prog_char PROGMEM MSG_UNITS_MS[] =                          "ms";
const prog_char PROGMEM MSG_UNITS_SECS[] =                        "Seconds";
const prog_char PROGMEM MSG_UNITS_MINS[] =                        "Minutes";
const prog_char PROGMEM MSG_UNITS_HOURS[] =                       "Hours";
const prog_char PROGMEM MSG_UNITS_DAYS[] =                        "Days";
const prog_char PROGMEM MSG_SENSOR_TUNING[] =                     "Tuning Style";
const prog_char PROGMEM MSG_SENSOR_TUNING_VISUAL[] =              "Visual";
const prog_char PROGMEM MSG_SENSOR_TUNING_NUMERIC[] =             "Numeric";
const prog_char PROGMEM MSG_SENSOR_TYPE[] =                       "Sensor Type";
const prog_char PROGMEM MSG_SENSOR_TYPE_AUDIO[] =                 "Sound";
const prog_char PROGMEM MSG_SENSOR_TYPE_LIGHT[] =                 "Light";
const prog_char PROGMEM MSG_SENSOR_TYPE_LIGHTNING[] =             "Lightning";


// Setups at startup
void setup() {
  
  // Create custom LCD symbols
  byte char_arrow_down[8] = {B00000, B00100, B00100, B00100, B00100, B10101, B01110, B00100};
  byte char_arrow_up[8]   = {B00100, B01110, B10101, B00100, B00100, B00100, B00100, B00000};

  // LCD initialize
  lcd.begin( LCD_COLS,  LCD_ROWS);
  lcd.createChar(SYMBOL_DOWN, char_arrow_down);
  lcd.createChar(SYMBOL_UP, char_arrow_up);

  // Pinmode inputs
  pinMode(PINS_BTN_A,          INPUT);     
  pinMode(PINS_BTN_B,          INPUT);    
  pinMode(PINS_SENSOR_MIC,     INPUT);
  pinMode(PINS_SENSOR_PHOTO_D,  INPUT);

  // Pinmode outputs
  pinMode(PINS_LCD_RS,         OUTPUT);
  pinMode(PINS_LCD_ENABLE,     OUTPUT);
  pinMode(PINS_LCD_DB4,        OUTPUT);
  pinMode(PINS_LCD_DB5,        OUTPUT);
  pinMode(PINS_LCD_DB6,        OUTPUT);
  pinMode(PINS_LCD_DB7,        OUTPUT);

}




// Run controller
void loop(){ 
  
  // Initialize config
  config_init(); 
  
  // Run main controller
  controller_run();

}






