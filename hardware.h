

//**********************************************************************//

//####### Board Pinouts My own RX/TX board #########

#if defined(Board_HWSPI) 
  //## RFM22B Pinouts 

  const uint8_t OUT_PIN[RC_CHANNELS] = { 2, 3, 4, 5, 6, 7 };
  const uint8_t PPMSUM_OUT_PIN =  { 2 };
  
  #define PPM_Port_HIGH PORTD |=  _BV(PPMSUM_OUT_PIN) //PPMSUM pull up  
  #define PPM_Port_LOW  PORTD &= ~_BV(PPMSUM_OUT_PIN) //PPMSUM pull down 

  #define Servo1_OUT_HIGH PORTD |= _BV(2) //Servo1 pin2 PD2
  #define Servo2_OUT_HIGH PORTD |= _BV(3) //Servo2 pin3 PD3
  #define Servo3_OUT_HIGH PORTD |= _BV(4) //Servo3 pin4 PD4
  #define Servo4_OUT_HIGH PORTD |= _BV(5) //Servo4 pin5 PD5
  #define Servo5_OUT_HIGH PORTD |= _BV(6) //Servo5 pin6 PD6
  #define Servo6_OUT_HIGH PORTD |= _BV(7) //Servo6 pin7 PD7
  #define PWM_Port_LOW PORTD &= 0x03 // 6 chan PWM pull down pin2 to pin7 
 
  #define  nSEL_on PORTC |= (1<<0) //A0
  #define  nSEL_off PORTC &= 0xFE  //A0

  // Hardware SPI, D13 SCK PB5, D12 SO PB4, D11 SI PB3
  #define  SCK_on  PORTB |= (1<<5) //D13 / PB5
  #define  SCK_off PORTB &= 0xDF

  #define  SDI_on  PORTB |= (1<<3) //D11 / PB3
  #define  SDI_off PORTB &= 0xF7 

  #define  SDO_1 (PINB & 0x10) == 0x10 //D12 / PB4
  #define  SDO_0 (PINB & 0x10) == 0x00 

  #define SDO_pin  12  // PB4
  #define SDI_pin  11  // PB3
  #define SCLK_pin 13  // PB5

  #define IRQ_pin 2
  #define nSel_pin A0 
  #define IRQ_interrupt 0
  
  #define PPM_IN 1
  #define ICP_pin D8 //PB0  
#endif

//**********************************************************************//

//Orangerx RX 
#if defined(Board_RX) 

  #define LED
  #define Red_LED A3
  #define Green_LED 13
  #define Red_LED_ON  PORTC|=_BV(3);
  #define Red_LED_OFF  PORTC&=~_BV(3);    
  #define Green_LED_ON  PORTB|=_BV(5);
  #define Green_LED_OFF  PORTB&=~_BV(5);

  //Servos 3, 5, 6, 7, 8, 9, 10, 
  const uint8_t OUT_PIN[RC_CHANNELS] = { 3, 5, 6, 7, 8, 9};
  const uint8_t PPMSUM_OUT_PIN =  { 3 };

  #define PPM_Port_HIGH PORTD |=  _BV(PPMSUM_OUT_PIN) //PPMSUM pull up  
  #define PPM_Port_LOW  PORTD &= ~_BV(PPMSUM_OUT_PIN) //PPMSUM pull down 

  #define Servo1_OUT_HIGH PORTD |= _BV(3) //Servo1 pin3 PD3
  #define Servo2_OUT_HIGH PORTD |= _BV(5) //Servo2 pin5 PD5
  #define Servo3_OUT_HIGH PORTD |= _BV(6) //Servo3 pin6 PD6
  #define Servo4_OUT_HIGH PORTD |= _BV(7) //Servo4 pin7 PD7
  #define Servo5_OUT_HIGH PORTB |= _BV(0) //Servo5 pin8 PB0
  #define Servo6_OUT_HIGH PORTB |= _BV(1) //Servo6 pin9 PB1
  #define PWM_Port_LOW PORTD &= 0x17; PORTB &= 0xFC 

  #define  nSEL_on PORTD |= (1<<4) //D4
  #define  nSEL_off PORTD &= 0xEF //D4

  #define  SCK_on PORTC |= (1<<2) //A2
  #define  SCK_off PORTC &= 0xFB //A2

  #define  SDI_on PORTC |= (1<<1) //A1
  #define  SDI_off PORTC &= 0xFD //A1

  #define  SDO_1 (PINC & 0x01) == 0x01 //A0
  #define  SDO_0 (PINC & 0x01) == 0x00 //A0

  #define SDO_pin A0
  #define SDI_pin A1
  #define SCLK_pin A2
  #define nSel_pin 4
  #define IRQ_pin 2

  #define IRQ_interrupt 0  
  #define PPM_IN 1
  #define ICP_pin D8 //PB0  
#endif

//**********************************************************************//
//####### Board Pinouts TX Board #########

// Orangerx transmitter module TX_BOARD_TYPE 2
#if defined(Board_TX) 

  #define LED
  #define PPM_IN 3
  #define RF_OUT_INDICATOR A0
  #define BUZZER 10
  #define BTN 11
  #define Red_LED 13
  #define Green_LED 12

  #define Red_LED_ON  PORTB |= _BV(5);
  #define Red_LED_OFF  PORTB &= ~_BV(5);

  #define Green_LED_ON   PORTB |= _BV(4);
  #define Green_LED_OFF  PORTB &= ~_BV(4);

  #define PPM_IN 1
  #define ICP_pin D8 //PB0

  //## RFM22B Pinouts for Public Edition (M2)
  #define  nSEL_on PORTD |= (1<<4) //D4
  #define  nSEL_off PORTD &= 0xEF //D4

  #define  SCK_on PORTD |= (1<<7) //D7
  #define  SCK_off PORTD &= 0x7F //D7

  #define  SDI_on PORTB |= (1<<0) //B0
  #define  SDI_off PORTB &= 0xFE //B0

  #define  SDO_1 (PINB & 0x02) == 0x02 //B1
  #define  SDO_0 (PINB & 0x02) == 0x00 //B1

  #define SDO_pin 9
  #define SDI_pin 8
  #define SCLK_pin 7
  #define IRQ_pin 2
  #define nSel_pin 4

  #define IRQ_interrupt 0
#endif

#if !defined(LED)
  #define Red_LED_ON 
  #define Red_LED_OFF 
  #define Green_LED_ON
  #define Green_LED_OFF 
#endif



