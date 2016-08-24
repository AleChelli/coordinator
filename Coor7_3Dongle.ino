//Library for the watchdog
#include <avr/wdt.h>


//Library for the Lwm
#include "lwm.h"
#include "atmegarfr2.h"
#include "hal.h"
#include "halTimer.h"
#include "nwk.h"
#include "nwkCommand.h"
#include "nwkDataReq.h"
#include "nwkFrame.h"
#include "nwkGroup.h"
#include "nwkRoute.h"
#include "nwkRouteDiscovery.h"
#include "nwkRx.h"
#include "nwkSecurity.h"
#include "nwkTx.h"
#include "phy.h"
#include "sys.h"
#include "sysConfig.h"
#include "sysEncrypt.h"
#include "sysTimer.h"
#include "sysTypes.h"

//Library for the Xbee
#include "XBee.h"


#include "coordinatore.h"
int contatore=0;
const long interval = 500; 
void setup() {
  wdt_disable();
 /*--------------------------------------------------------------------*/ 
 ApioCoordinatorSetup(); //inizializzazione coordinatore
  pinMode(20,OUTPUT);
 /*--------------------------------------------------------------------*/  
  wdt_enable(WDTO_8S);
}
int ledState=LOW;
unsigned long previousMillis = 0;

// the loop routine runs over and over again forever:
void loop() {
  SYS_TaskHandler(); //task management LWM
  /*unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(20, ledState);
  } */
  if(Serial.available()>0)
  {
      comunication_protocol = ReadFromWebServer();
              
        switch(comunication_protocol)
        {
          case('z'):
            XBeeOutput_62(deviceAddr, content);
          break;
        
          case('l'):
            LwmOutput_109(deviceAddr,content);
          break;
          
          case('x'):
            if (property=="panId")
            {
              NWK_SetPanId(value.toInt());
            }
            else if(property=="channel")
            {
              PHY_SetChannel(value.toInt()); 
            }
            break;
            
        }
        
    content="";
    deviceAddr="";
    comunication_protocol=NULL;
    contatore++;
   } 
   if(contatore>500)
   {
     wdt_enable(WDTO_30MS);
     while(true)
     {
     }
     //Serial.println("CIAOOOOOO");
     contatore=0;
   }
     
 /*--------------------------------------------------------------------*/ 
  wdt_reset(); //watchdog reset
  
}
