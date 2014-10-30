//####### FUNCTIONS #########

volatile uint8_t rcValue[RC_CHANNELS];
uint8_t rcData[RC_CHANNELS];
uint8_t chanmux = 0;
uint8_t gotir = 0;

#if defined(TX)

  volatile uint16_t last = 0;
  volatile uint8_t chan = 0;

#if defined(ICP) 

  ISR(TIMER1_CAPT_vect)
  {
    uint16_t now,diff;
    now = ICR1;
    diff = now - last;
    last = now;
    if      (diff>6000) chan = 0; // Sync gap
    else if (chan < RC_CHANNELS)
    {
      if (1800<diff && diff<4200)
      {
        if      (diff <= 2000) rcValue[chan] = 0;
        else if (diff >= 4000) rcValue[chan] = 255;
        else rcValue[chan] = (diff - 1976) >> 3;
        chan++;
      }
      else chan = 20;
    }
    else if (chan == RC_CHANNELS) 
    {
      gotir = 1; 
      chan++;
    }
  }

  void InitPPMin()
  {
    // Setup timer1 for input capture (PSC=8 -> 0.5ms precision, top at 20ms)
    TCCR1A = 0;
    TCCR1B = (1 << CS11) | (1 << ICNC1) | (1 << ICES1);
    TIMSK1 |= (1 << ICIE1);   // Enable timer1 input capture interrupt
  }

#else // sample PPM using pinchange interrupt

  void rxInt() 
  {
    uint16_t now,diff; 
    now = TCNT1;
    diff = now - last;
    last = now;
    if      (diff>6000) chan = 0; // Sync gap
    else if (chan < RC_CHANNELS)
    {
      if (1800<diff && diff<4200)
      {
        if      (diff <= 2000) rcValue[chan] = 0;
        else if (diff >= 4000) rcValue[chan] = 255;
        else rcValue[chan] = (diff - 1976) >> 3;
        chan++;
      }
      else chan = 20;
    }
    else if (chan == RC_CHANNELS) 
    {
      gotir = 1; 
      chan++;
    }
  }

  void InitPPMin() 
  {
    byte chan;
    // Init PPM SUM RX
    attachInterrupt(PPM_IN, rxInt, RISING); 
    TCCR1A = 0;
    TCCR1B = (1 << CS11);
    for (chan = 0; chan < RC_CHANNELS; chan++) rcValue[chan] = 128;
  }
#endif // ICP

boolean getRC()
{
  if (gotir > 0)
  {
    gotir = 0;
    Green_LED_ON
    tx_buf[0] = rcValue[0];
    tx_buf[1] = rcValue[1];
    tx_buf[2] = rcValue[2];
    tx_buf[3] = rcValue[3];
    tx_buf[4] = chanmux | (rcValue[4+chanmux] & 0xFE);
    chanmux++;  chanmux &= 0x01;
    return true;
  }
  else 
  {
    Green_LED_OFF
    return false;
  }
}

#endif // TX

#if defined(RX) 

void getRCdata()
{
  rcData[0] = rx_buf[0];
  rcData[1] = rx_buf[1];
  rcData[2] = rx_buf[2];
  rcData[3] = rx_buf[3];
  chanmux = rx_buf[4] & 0x01;
  rcData[4+chanmux] = rx_buf[4] & 0xFE;
}

#endif // RX

