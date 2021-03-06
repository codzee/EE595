#include <SPI.h>
#include <AS3935.h>

#define LIGHTNING_DETECTED 0b1000 

byte SPItransfer(byte sendByte);

AS3935 AS3935(SPItransfer,SS,2);

// Pinout settings
#define PINS_LCD_LED         6 //(digital pin)
#define PINS_LCD_RS          3 //(digital pin)
#define PINS_LCD_ENABLE      4 //(digital pin)
#define PINS_LCD_DB4         5 //(digital pin)
#define PINS_LCD_DB5         6  //(digital pin)
#define PINS_LCD_DB6         7  //(digital pin)
#define PINS_LCD_DB7         8  //(digital pin)
#define PINS_RIGHT           A5  //(digital pin)
#define PINS_LEFT            A2  //(digital pin)
#define PINS_UP              A3
#define PINS_DOWN            A4

LiquidCrystal lcd(PINS_LCD_RS, PINS_LCD_ENABLE, PINS_LCD_DB4, PINS_LCD_DB5, PINS_LCD_DB6, PINS_LCD_DB7);

void setup()
{
  Serial.begin(9600);
  
  setupSPI();
  
  setupAS3935(); 
}

void loop() {}


byte SPItransfer(byte sendByte)
{
  return SPI.transfer(sendByte);
}

void AS3935Irq()
{
  int irqSource = AS3935.interruptSource();
  
  if (irqSource & LIGHTNING_DETECTED) 
  {
    int strokeDistance = AS3935.lightningDistanceKm();
    
    if (strokeDistance == 1) 
    {  
      Serial.println("Storm overhead, watch out!");
    }
    
    if (strokeDistance < 63 && strokeDistance > 1)
    {
      Serial.print("Lightning detected ");
      Serial.print(strokeDistance,DEC);
      Serial.println(" kilometers away.");
    }
  }
}

void setupSPI() 
{
  SPI.begin();
  
  SPI.setDataMode(SPI_MODE1);
  
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  
  SPI.setBitOrder(MSBFIRST); 
}

void setupAS3935() 
{
  AS3935.reset();
  
  if(!AS3935.calibrate())
    Serial.println("Tuning out of range, check your wiring, your sensor and make sure physics laws have not changed!");
    
  AS3935.setOutdoors();
  
  AS3935.disableDisturbers();
  
  attachInterrupt(0,AS3935Irq,RISING);  
}
