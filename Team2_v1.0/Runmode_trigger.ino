 
 // Run sensor trigger mode
void runAs_sensorTriggerMode() { 
       
   display_printTitle(MSG_RUNNING);
   
   keyboard_waitForNokey();
    
   attachInterrupt(0, keyboard_interrupts, CHANGE);
   attachInterrupt(1, keyboard_interrupts, CHANGE);
   
//   for(unsigned int cyclesCounter = 0; (cancelFlag==false && !(sensorTriggerMode_numCycles>0 && cyclesCounter >= sensorTriggerMode_numCycles));cyclesCounter++) { 
     
      // Normal shooting mode
//    if (sensorTriggerMode_shootingMode == SHOOTINGMODE_NORMAL) {

//       laser_turnOn();
//       solenoidValve_makeDrops();
       
//       sensor_waitFor(sensorTriggerMode_sensorType, 0);
//       laser_turnOff();
       
//       if(!cancelFlag) {
//       }
//   }

//}
   
   display_printAborting();
   keyboard_waitForNokey();
   
   detachInterrupt(0);
   detachInterrupt(1);

}

