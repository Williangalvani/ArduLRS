#if defined (RX)

/**************************************************************************************/
/***************                  Motor Pin order                  ********************/
/**************************************************************************************/

uint8_t servo[6] = { 128, 128, 128, 128, 128, 128 }; // servo pulse in usec

uint8_t  ppmCounter = 0;
uint16_t ppmTotal = 0;

#if defined(PPMSUM_OUTPUT)
  boolean PPM_output = true; 
#else
  boolean PPM_output = false; 
#endif

void initOutput() 
{
  TCCR1A = (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
  ICR1 = 40000; // just initial value, will be constantly updated
  OCR1A = 1000;
  if (PPM_output)
  {
    TIMSK1 |= (1 << TOIE1) | (1 << OCIE1A); 
    pinMode(PPMSUM_OUT_PIN, OUTPUT);
  } else {
    TIMSK1 |= (1 << TOIE1);
    for (unsigned char i=0; i<RC_CHANNELS; i++) pinMode(OUT_PIN[i], OUTPUT);
  }
  
}

ISR(TIMER1_OVF_vect)
{
  uint16_t ppmOut;
  if (ppmCounter >= RC_CHANNELS) 
  {
    ICR1 = 40000 - ppmTotal; // 20ms total frame
    ppmCounter = 0;
    ppmTotal = 0;
    if (PPM_output) PPM_Port_LOW;
    else            PWM_Port_LOW; 
  } 
  else 
  {
    ppmOut = 1976 + 8*(uint16_t)servo[ppmCounter];
    ppmTotal += ppmOut;
    ICR1 = ppmOut;
    if (PPM_output)  PPM_Port_LOW;        
    else 
    {
      PWM_Port_LOW;
      switch (ppmCounter)
      {
        case 0: Servo1_OUT_HIGH; break;
        case 1: Servo2_OUT_HIGH; break;
        case 2: Servo3_OUT_HIGH; break;
        case 3: Servo4_OUT_HIGH; break;
        case 4: Servo5_OUT_HIGH; break;
        case 5: Servo6_OUT_HIGH; break;
      }
    }
    ppmCounter++;
  }
}

ISR(TIMER1_COMPA_vect)
{
  PPM_Port_HIGH;
}
#endif
