
#include <Arduino.h>
#include "config.h"
#include "hardware.h"

unsigned char tx_buf[packetlength]; // TX buffer
unsigned char rx_buf[packetlength]; // RX buffer

unsigned char RF_channel = 0;
unsigned char RF_Mode = 0;

unsigned long time;
unsigned long TXT; // Transmit time
unsigned long RXT; // Receive time
unsigned long FST; // Failsave time

byte FSC = 0; // FailSaveCounter
boolean SyncMode = false;

uint8_t hop_list[Max_RF_channel] = {DEFAULT_HOPLIST}; 
extern volatile uint8_t rcValue[RC_CHANNELS];
extern uint8_t rcData[RC_CHANNELS];
extern uint8_t servo[6];

void setup() 
{
  #if defined(LED)
    pinMode(13, OUTPUT);
    pinMode(A3, OUTPUT);
    Red_LED_ON
    Green_LED_OFF 
  #endif

  //RF module pins
  pinMode(nSel_pin, OUTPUT); //nSEL
  RF22B_init();
  
  pinMode(0, INPUT); // Serial Rx
  pinMode(1, OUTPUT);// Serial Tx
  pinMode(IRQ_pin, INPUT); // irq line

  SerialOpen(SERIAL_BAUD_RATE);  
  RF22B_init_parameter();     // Configure the RFM22B's registers

#if defined(TX)
  //pinMode(BTN, INPUT);        //Buton
  pinMode(PPM_IN, INPUT);     //PPM from TX
  digitalWrite(PPM_IN, HIGH); // enable pullup for TX:s with open collector output
  InitPPMin();
#endif

  sei();

  RF_Mode = 0;
  RF_reset();
  
  time = millis();
  TXT = time + 200;
  RXT = time + 200;

  if (rfmGetDevType() == 8) SerialWrite('G'); else SerialWrite('N');
  if (rfmGetDevVers() == 6) Red_LED_OFF else Red_LED_ON
#if defined(RX)
  initOutput();
  to_rx_mode();
#endif

}

//############ MAIN LOOP ##############

void checkhang()
{
  if (spiReadRegister(0x0C)==0) 
  { 
    //Serial.println("Device hang");
    RF22B_init_parameter();
    RF_reset();
  }
}

byte rxcounter = 0;
byte txcounter = 0;
byte misscounter = 0;

boolean getRssi = false;
uint8_t actRssi;

void loop() 
{
  time = millis();
  //checkhang();  
  RF_Mode = spiReadRegister(0x03);
    
#if defined(TX)
  
  if (time >= TXT) // transmit the stuff
  {
    if (getRC())
    {
      serialize8(tx_buf[0]);
      serialize8(tx_buf[1]);
    }  
    TXT += SendMS;
    FST = time; // reuse variable
    RF_Mode = 0;
    to_tx_mode();
  }
  if (RF_Mode & 0x04) // done transmitting
  { 
    RF_Mode = 0;
    #if defined(HOPPING)
      Hopping();
    #endif
    RXT = time - FST;
    serialize8(RXT & 0xff); 
    txcounter++;
    serialize8(txcounter);
    SerialWrite(13);
  }  
  delay(1);
  
#endif 
#if defined(RX) 

  if (RF_Mode & 0x02)  // Received a packet 
  {
    RXT = time;
    FST = time + ReceiveMS;
    
    spiReadPacket();
    #if defined(HOPPING)
      Hopping();
    #endif

    Green_LED_OFF
    FSC = 0;
    SyncMode = true;
    Red_LED_ON

    RF_Mode = 0;
    to_rx_mode(); 
    getRssi = true;
    
    getRCdata();
    
    for (unsigned char n=0; n<RC_CHANNELS; n++)
      servo[n] = rcData[n];
        
    rxcounter++;  
    serialize8(rxcounter);
    serialize8(misscounter);
    SerialWrite(13);    
  }
  else if (time >= FST) // Missed a packet
  { 
    SyncMode = (FSC <= MaxFail);
    if (SyncMode) 
    { 
      FSC++; 
      FST = time + ReceiveMS; 
    } else {
      FST = time + SyncMS;
      for (unsigned char n=0; n<RC_CHANNELS; n++)
        servo[n] = 128;
    }
            
    #if defined(HOPPING)
      Hopping();
    #endif    
    RF_Mode = 0;
    to_rx_mode();
    
    Green_LED_ON
    
    misscounter++;  
    if (!SyncMode) serialize8(255);  
    else serialize8(rxcounter);
    serialize8(misscounter);
    SerialWrite(13);
  }  
  else if ((time - RXT) >= RssiMS)
  {
    if (getRssi)
    {
      getRssi = false;
      actRssi = rfmGetRSSI();
      SerialWrite(actRssi);
    }
  }
  delay(1);
  Red_LED_OFF
#endif
}
