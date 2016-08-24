#include <avr/wdt.h>

#include "Apio.h"
#include "property.h"

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

int pin20=14;
int pin21=15;

void setup() {
  wdt_disable();
  pinMode(14,OUTPUT);
  pinMode(15,OUTPUT);
  Apio.setup("Miiiio", "1,0", 0, 0x01);
  wdt_enable(WDTO_4S);
}
void loop(){
  Apio.loop();
  wdt_reset();	
}
