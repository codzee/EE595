#include <usbhub.h>

#include <ptp.h>
#include <canoneos.h>

class CamStateHandlers : public PTPStateHandlers
{
      bool stateConnected;

public:
      CamStateHandlers() : stateConnected(false) {};

      virtual void OnDeviceDisconnectedState(PTP *ptp);
      virtual void OnDeviceInitializedState(PTP *ptp);
} CamStates;

USB         Usb;
USBHub      Hub1(&Usb);
CanonEOS    Eos(&Usb, &CamStates);

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
    if ( stateConnected )
    {
        stateConnected = false;
        
        Serial.println("Camera disconnected");
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
    if ( !stateConnected ) {
      
        stateConnected = true;
    }
    
    Serial.println("Camera connected");
    
    while( stateConnected == true ){
      
        readSerialCommand();
    }
}

void setup()
{
    Serial.begin( 115200 );
    
    Serial.println("Start");

    if (Usb.Init() == -1) {
      
        Serial.println("OSC did not start.");
    }
    
    delay( 200 );
}

void loop()
{
    Usb.Task();
}

void readSerialCommand() {
    
    if ( Serial.available() ) {
      
      if( Serial.read() == 'C' )
   
        Serial.println("Capture!");
        
        Eos.Capture();
        
        delay(500);
    }
}
