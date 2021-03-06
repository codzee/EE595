#include <SPI.h>
#include <AS3935.h>

#define LIGHTNING_DETECTED 0b1000 

byte SPItransfer(byte sendByte);

AS3935 AS3935(SPItransfer,SS,2);

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
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Storm Overhead  ");
      lcd.setCursor(0,1);
      lcd.print("Watch Out!      ");   
    }
    
    if (strokeDistance < 63 && strokeDistance > 1)
    {
      Serial.print("Lightning detected ");
      Serial.print(strokeDistance,DEC);
      Serial.println(" kilometers away.");
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Lightning Found ");
      lcd.setCursor(0,1);
      lcd.print(strokeDistance,DEC);
      lcd.print(" kilometers   "); 
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
