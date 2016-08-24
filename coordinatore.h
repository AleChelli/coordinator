/*-----------------------constants definition -------------------------------*/
#define ARRAY_LENGTH 50
#define COORDINATOR_ADDRESS_LWM 0
#define BUFFER_SIZE 100
#define del 100

/*-----------------------variables definition ------------------------------*/

String content; //contains the whole string: deviceAddr+property+value+...+property+value
char comunication_protocol; //which communications interface to use (and other various control commands)

String deviceAddr; //destination address
String property; 
String value;
int numberkey=0; //number of pairs property:value
char sendThis[BUFFER_SIZE];
//declaration concerning LWM  
static bool nwkDataReqBusy = false;

//declaration concerning XBEE 
XBee xbee = XBee();
ZBRxResponse zbRx = ZBRxResponse(); //for the package in reception
ZBTxRequest zbTx = ZBTxRequest(); 
ZBTxStatusResponse txStatus = ZBTxStatusResponse(); // for the response of the xbee


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//does the power of two integers:I need because the 'pow' function had trouble rounding
//(pow(10,3)=999,838...) if I make a cast to int truncates the decimal

uint32_t power(int base, int esp)
{
  int i;
  uint32_t result = base;
  if(esp == 0)
  {
    return 1;
  }
  for(i=1; i<esp; i++)
  {
    result *= base;
  }
  return result;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//convert a string made up of only numbers (representing a number in hexadecimal) in a data type uint32_t-----------
uint32_t stringHEX_To_uint32_t(String string)
{
  uint32_t num = 0;
  char buf[12];
  int len = string.length();
  string.toCharArray(buf, 12);
  int i;
  
  for(i=len-1; i>=0; i--)
  {
    if(buf[i]>='0'  && buf[i]<='9') //if the character is a number(ascii value between 48('0') and 57('9') ) then subtract '0'
    {
    num += ( (buf[i] - '0')*power(16,len-i-1) ); 
    
    }
    
    
    if(buf[i]>='A'  && buf[i]<='F') //if the character is an uppercase letter invalid (ASCII value between 65('A') and 70('F') - than subtract 'A'
    {                               //then I add 10 because A (in hex)  is equal to 10 (alternatively I can subtract '7' = 'A'-10)
    num += ( (buf[i] - 'A' + 10)*power(16,len-i-1) ); 
    
    }
    
    
    if(buf[i]>='a'  && buf[i]<='f') //if the character is an lowercase letter invalid (ASCII value between 97('a') and 102('f') - than subtract 'a'
    {                               //then I add 10 because a (in hex) is equal to 10 (alternatively I can subtract 'W' = 'a'-10)
    num += ( (buf[i] - 'a' + 10)*power(16,len-i-1) ); 
    
    }
    
  }
  
  return num;
  
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//convert a string made up of only numbers (representing a number in decimal) in a data type uint32_t-----------
uint16_t stringDEC_To_uint16_t(String string) //da cambiare perche per l wm abbiamo valori decimali e non hex
{
  uint16_t num = 0;
  uint16_t numprec = 0; //need to check if the variable num overflows ( num > 65535 )
  char buf[12];
  int len = string.length();
  string.toCharArray(buf, 12);
  int i;
  
  for(i=len-1; i>=0; i--)
  {
    //numprec=num; to control the overflow
    if(buf[i]>='0'  && buf[i]<='9') //if the character is a number (ascii value between 48('0') and 57('9') ) than subtract '0'
    {
      num += ( (buf[i] - '0')*power(10,len-i-1) ); 
    
    }
    return num;
    
    /*if(num<numprec)  //if num overflows starts from 0 so it will be less than its value at the previous cycle
    {
      return NULL; 
    }*/
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void divide_string(String stringToSplit) {
  
  int strlen=stringToSplit.length();
  //Serial1.println(stringToSplit); //debug
  int i; //counter
  deviceAddr=""; 
  //Serial1.println(numberkey);
  //-----------deviceAddr----------------  
  
  for(i=0; stringToSplit.charAt(i)!=':' && i<strlen ;i++)
  {
    deviceAddr += String(stringToSplit.charAt(i));
  }
  for(i++; stringToSplit.charAt(i)!=':' && i<strlen ;i++)
  {
      property+= String(stringToSplit.charAt(i));
  }
  for(i++; stringToSplit.charAt(i)!='-' && i<strlen ;i++)
  {
    value += String(stringToSplit.charAt(i)); 
  }
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char ReadFromWebServer()
{
  //l16:onoff:1-rosso:255-verde:255-blu:255-funzioni:0-
  //Serial1.println("readfrom");
  int flagAddr=0;
  char interface=NULL;
  delayMicroseconds(del); // it is needed, otherwise every now and then you lose the first character to be read!!!
  interface = Serial.read();
  delayMicroseconds(del);
  while (Serial.available())
  {
    char buf = Serial.read();
    delayMicroseconds(del);
    if(buf==':'){
      flagAddr=1;
    }
    if(flagAddr==0)
    {
      deviceAddr+=buf;
    }
    content += buf;
  }
  //Serial.println(content);
  delayMicroseconds(del);
  //divide_string(content);
  return interface;
}
/*-----------------------------------------------------LWM----------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//callback for the management of the confirmation (access the field message->opzioni) and verification of ack
static void appDataConf(NWK_DataReq_t *req)
{
  //Serial.print("ACK: "); //debug
  char x[100];
  switch(req->status)
  {
    case NWK_SUCCESS_STATUS:
      //Serial.print(1,DEC);
      {
      int message_size=req->size;
      Serial.print("ok:");
      for(int j=0; j<message_size; j++)
      {
        x[j] = req->data[j];
        Serial.print(x[j]);
        //delay(10);
        //Serial.write(ind->data[i]);
    
      }
      
      Serial.println();
      
      
      break;
      }
    case NWK_ERROR_STATUS:
      //Serial.print(2,DEC);
      break;
    case NWK_OUT_OF_MEMORY_STATUS:
      //Serial.print(3,DEC);
      break;
    case NWK_NO_ACK_STATUS:
      //Serial.print(4,DEC);
      break;
    case NWK_NO_ROUTE_STATUS:
      //Serial.print(5,DEC);
      break;
    case NWK_PHY_CHANNEL_ACCESS_FAILURE_STATUS:
      //Serial.print(6,DEC);
      break;
    case NWK_PHY_NO_ACK_STATUS:
      //Serial.print(7,DEC);
      break;
//    default:
//      Serial1.print("no correspondence in ack");
//      break;
     

  }
  nwkDataReqBusy = false;

  //Serial.println("");
  
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static void LwmOutput_109(String devAddr,String toSend) 
{

  //Serial1.println("LwmOutput_62");//debug
  int len = toSend.length(); //if i use toSend.toCharArray() the lwm packet do not get good
  //Serial.print("Lunghezza:");
  //Serial.println(len);
  sendThis[len];
  for(int g=0; g<len ;g++) 
  {
      sendThis[g]=toSend.charAt(g);
     // Serial.write(sendThis[g]);
  }
  //Serial.println("");
        
  //int16_t address = stringDEC_To_uint16_t(devAddr);
  
  nwkDataReqBusy = true; 
  NWK_DataReq_t *message = (NWK_DataReq_t*)malloc(sizeof(NWK_DataReq_t));
  message->dstAddr = devAddr.toInt(); //object address
  message->dstEndpoint = 1; 
  message->srcEndpoint = 1;
  message->options = NWK_OPT_ACK_REQUEST; //request an ack
  message->size = len;
  message->data = (uint8_t*)(sendThis);

  message->confirm = appDataConf; //callback for the management of the confirmation (option field)
                                  //and verification of ack required above
  NWK_DataReq(message); //send message
  //delay(100);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//to receive a packet with LWM
static bool LwmInput(NWK_DataInd_t *ind) 
{ 
  
  int message_size=ind->size;
  char bufferL[62];
  for(int i=0; i<message_size; i++)
  {
    //Buffer[i] = ind->data[i];
    bufferL[i] = ind->data[i];
  }
  Serial.println(String(bufferL)); 
  return true;
}


/***********************************************************************/
/*+++++++++++++++++++++++++++++++++++XBEE++++++++++++++++++++++++++++++*/
/***********************************************************************/


//-----------------------------------------function that sends data to xbee-----------------

bool XBeeOutput_62(String devAddr, String toSend)
{   
  //Serial.println(toSend);
  char toSend_char[100]; //beacause the ZBTxRequest needs a uint8_t*
  toSend.toCharArray(toSend_char, 100);
  
  uint32_t indirizzo = stringHEX_To_uint32_t(devAddr);
  
  XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, indirizzo);
  
  zbTx = ZBTxRequest(addr64, (uint8_t*)(toSend_char), strlen(toSend_char)); 
  
  xbee.send(zbTx);
//  
//  // after sending a tx request, we expect a status response
//  // wait up to half second for the status response
//  if (xbee.readPacket(500)) 
//  {
//      Serial1.println("Got a tesponse!");
//
//    // got a response!
//
//    // should be a znet tx status             
//    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) 
//    {
//      Serial1.println("ZB_TX_STATUS_RESPONSE");
//
//      xbee.getResponse().getZBTxStatusResponse(txStatus);
//            // get the delivery status, the fifth byte
//      if (txStatus.getDeliveryStatus() == SUCCESS) 
//      {
//        // success.  time to celebrate
//        //flashLed(statusLed, 5, 50);
//        Serial1.println("SUCCESS");
//        return true;
//      } 
//      else 
//      {
//        if(txStatus.getDeliveryStatus() == PAYLOAD_TOO_LARGE)
//        {
//          Serial1.println("PAYLOAD_TOO_LARGE");
//        }
//        // the remote XBee did not receive our packet. is it powered on?
//        //flashLed(errorLed, 3, 500);
//         Serial1.println("ERROR");
//
//        return false;
//
//      }
//    }
//    else if (xbee.getResponse().isError()) 
//       {
//         Serial1.print("Error reading packet.  Error code: ");  
//         Serial1.println(xbee.getResponse().getErrorCode());
//         return false;
//       } 
//       else 
//       {
//          Serial1.print("local XBee did not provide a timely TX Status Response");  
//
//          // local XBee did not provide a timely TX Status Response -- should not happen
//          //flashLed(errorLed, 2, 50);
//          return false;
//       }
//  }
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool XBeeInput()
{
  // 1. This will read any data that is available:
  xbee.readPacket();

  // 2. Now, to check if a packet was received: 
  if (xbee.getResponse().isAvailable()) //se è vero ho ricevuto qualcosa
  {
    char bufferX[62];
    String receivedX;
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) //if it's true i have a zb-rx packet
    {
       xbee.getResponse().getZBRxResponse(zbRx); //fill zb-rx class
       /*flashLed(led, zbRx.getDataLength(), 500);*/
       for (int i = 0; i < zbRx.getDataLength(); i++) 
       {
           /*buffer[i] = char*/
           bufferX[i]=zbRx.getData()[i];
           receivedX+=bufferX[i];
       }
       Serial.println(receivedX);
       receivedX="";
       for (int i = 0; i < zbRx.getDataLength(); i++) 
       {
           /*buffer[i] = char*/
           bufferX[i]=NULL;
       }
       return true;
     }
  }
}


void ApioCoordinatorSetup()
{
  Serial.begin(115200); //for comucicate with the web-server
  //setup Xbee
  Serial1.begin(9600);//for comunicate with  xbee
  xbee.setSerial(Serial1);
  SYS_Init();//in questa funzione (di libreria LW_Mesh, file sys.c) è stata eliminata sys_Uninit_Arduino() altrimenti non funzionava bene la seriale
  NWK_SetAddr(COORDINATOR_ADDRESS_LWM);
  NWK_SetPanId(0x01);
  PHY_SetChannel(0x1a);
  //PHY_SetTxPower(0);
  TRX_CTRL_2_REG_s.oqpskDataRate = 2;
  PHY_SetRxState(true);
  NWK_OpenEndpoint(1, LwmInput);
  
  Serial.println("COORDINATOR ACTIVE");
}
