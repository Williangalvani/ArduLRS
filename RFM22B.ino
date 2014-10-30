
// r_1c, r_1d, r_1e,  clock recovery  r_20, r_21, r_22, r_23, r_24, r_25,     r_2a,         ook stuff            r_6e, r_6f, r_70, r_71, r_72;
//   1c,   afc        1f,               20,   21,   22,   23,   24,   25, afc limiter 2c,  2d,   2e,   58,   69,   6e,   6f,   70,   71,   72

PROGMEM static const unsigned char MODEM_CONFIG_TABLE[5][18] =
{
    // All the following enable FIFO with reg 71
    // 1c,   1f,   20,   21,   22,   23,   24,   25,   2c,   2d,   2e,   58,   69,   6e,   6f,   70,   71,   72
    // GFSK, No Manchester, Max Rb err <1%, Xtal Tol 20ppm
    // These differ from FSK only in register 71, for the modulation type
    { 0x1b, 0x03, 0x41, 0x60, 0x27, 0x52, 0x00, 0x07, 0x40, 0x0a, 0x1e, 0x80, 0x60, 0x13, 0xa9, 0x2c, 0x23, 0x3a }, // 2.4, 36
    { 0x1d, 0x03, 0xa1, 0x20, 0x4e, 0xa5, 0x00, 0x13, 0x40, 0x0a, 0x1e, 0x80, 0x60, 0x27, 0x52, 0x2c, 0x23, 0x48 }, // 4.8, 45
    { 0x1e, 0x03, 0xd0, 0x00, 0x9d, 0x49, 0x00, 0x45, 0x40, 0x0a, 0x20, 0x80, 0x60, 0x4e, 0xa5, 0x2c, 0x23, 0x48 }, // 9.6, 45
    { 0x2b, 0x03, 0x34, 0x02, 0x75, 0x25, 0x07, 0xff, 0x40, 0x0a, 0x1b, 0x80, 0x60, 0x9d, 0x49, 0x2c, 0x23, 0x0f }, // 19.2, 9.6
};

#define NOP() __asm__ __volatile__("nop")

#define RF22B_PWRSTATE_POWERDOWN          0x00
#define RF22B_PWRSTATE_READY              0x03
#define RF22B_PACKET_SENT_INTERRUPT       0x04
#define RF22B_PWRSTATE_RX                 0x07
#define RF22B_PWRSTATE_TX                 0x0B
#define RF22B_PACKET_RECEIVED_INTERRUPT   0x02

unsigned char ItStatus1, ItStatus2;


#if defined(HARDWARE_SPI)
  #include <SPI.h>
  
  #if (SDO_pin != 12) | (SDI_pin != 11) | (SCLK_pin != 13)
    #error "hardware spi config wrong"
  #endif
  
  void spiReadPacket() 
  {
    unsigned char i;
    nSEL_off;
    SPI.transfer(0x7f);
    for (i=0; i<packetlength; i++) rx_buf[i] = SPI.transfer(0);
    nSEL_on;
  }

  void spiWritePacket()
  {
    unsigned char i;
    nSEL_off;
    SPI.transfer(0x7f | 0x80);
    for (i=0; i<packetlength; i++) SPI.transfer(tx_buf[i]);
    nSEL_on;
  }
  
  unsigned char spiReadRegister(unsigned char address) 
  {
    unsigned char result;
    nSEL_off;
    SPI.transfer(address);
    result = SPI.transfer(0);
    nSEL_on;
    return(result);
  }

  void spiWriteRegister(unsigned char address, unsigned char data) 
  {
    nSEL_off;
    SPI.transfer(address | 0x80);
    SPI.transfer(data);
    nSEL_on; // tsw 80 nsec
  }
#endif // HARDWARE_SPI

#if defined(BITBANG_SPI)
  unsigned char spiTransferData(unsigned char i) 
  {
    unsigned char r = 0;
    for (unsigned char n=0; n<8; n++) 
    {
      if (i&0x80) SDI_on; else SDI_off;
      i <<= 1; 
      SCK_on; 
      NOP();
      r <<= 1; 
      if (SDO_1) r |= 1; 
      SCK_off;
    }
    SCK_off;
    return(r);
  }
 
  void spiReadPacket() //*
  {
    nSEL_off;
    spiTransferData(0x7f);
    for (int i=0; i<packetlength; i++) rx_buf[i] = spiTransferData(0);
    nSEL_on;
  }
  
  void spiWritePacket()
  {
    unsigned char i;
    nSEL_off;
    spiTransferData(0x7f | 0x80);
    for (i=0; i<packetlength; i++) spiTransferData(tx_buf[i]);
    nSEL_on;
  }
  
  unsigned char spiReadRegister(unsigned char address) //*
  {
    unsigned char result;
    nSEL_off;
    spiTransferData(address);
    result = spiTransferData(0);
    nSEL_on;
    return(result);
  }

  void spiWriteRegister(unsigned char address, unsigned char data) //*
  {
    nSEL_off;
    spiTransferData(address|0x80);
    spiTransferData(data);
    nSEL_on;
  } 
#endif //BITBANG_SPI

void RF22B_init(void)
{
  pinMode(SDO_pin, INPUT);   //SDO
  pinMode(SDI_pin, OUTPUT);  //SDI
  pinMode(SCLK_pin, OUTPUT); //SCLK
  nSEL_on;
  SCK_off;

  #if defined(HARDWARE_SPI)
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    SPI.setDataMode(SPI_MODE0);
    SPI.begin();
  #endif
}

void RF22B_init_parameter(void) 
{  
  spiWriteRegister(0x06, 0x00);    // no wakeup up, lbd,
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);      // disable lbd, wakeup timer, use internal 32768,xton = 1; in ready mode
  spiWriteRegister(0x09, 0x7f);    // c = 12.5p
  spiWriteRegister(0x0a, 0x05);
#ifdef SWAP_GPIOS
  spiWriteRegister(0x0b, 0x15);    // gpio0 RX State
  spiWriteRegister(0x0c, 0x12);    // gpio1 TX State
#else
  #ifdef NOBOOSTER  
    spiWriteRegister(0x0b, 0x15);    // gpio0 TX State
  #else
    spiWriteRegister(0x0b, 0x12);    // gpio0 TX State
  #endif
  spiWriteRegister(0x0c, 0x15);    // gpio1 RX State
#endif
  spiWriteRegister(0x0d, 0xfd);    // gpio 2 micro-controller clk output
  spiWriteRegister(0x0e, 0x00);    // gpio    0, 1,2 NO OTHER FUNCTION.

  //new
  spiWriteRegister(0x1D, 0x40);    // AFC ON
  spiWriteRegister(0x2A, 0x28);    // AFC Limiter
  //new
  
  spiWriteRegister(0x1C, MODEM_CONFIG_TABLE[filterset][0]);
  spiWriteRegister(0x1F, MODEM_CONFIG_TABLE[filterset][1]);
  spiWriteRegister(0x20, MODEM_CONFIG_TABLE[filterset][2]);
  spiWriteRegister(0x21, MODEM_CONFIG_TABLE[filterset][3]);
  spiWriteRegister(0x22, MODEM_CONFIG_TABLE[filterset][4]);
  spiWriteRegister(0x23, MODEM_CONFIG_TABLE[filterset][5]);
  spiWriteRegister(0x24, MODEM_CONFIG_TABLE[filterset][6]);
  spiWriteRegister(0x25, MODEM_CONFIG_TABLE[filterset][7]);
  spiWriteRegister(0x2C, MODEM_CONFIG_TABLE[filterset][8]);
  spiWriteRegister(0x2D, MODEM_CONFIG_TABLE[filterset][9]);
  spiWriteRegister(0x2E, MODEM_CONFIG_TABLE[filterset][10]);
  spiWriteRegister(0x58, MODEM_CONFIG_TABLE[filterset][11]);
  spiWriteRegister(0x69, MODEM_CONFIG_TABLE[filterset][12]);
  spiWriteRegister(0x6E, MODEM_CONFIG_TABLE[filterset][13]); // TX Data Rate 1
  spiWriteRegister(0x6F, MODEM_CONFIG_TABLE[filterset][14]); // TX Data Rate 0
  spiWriteRegister(0x70, MODEM_CONFIG_TABLE[filterset][15]);
  spiWriteRegister(0x71, MODEM_CONFIG_TABLE[filterset][16]); // Modulation Mode Control 2
  spiWriteRegister(0x72, MODEM_CONFIG_TABLE[filterset][17]); // Frequency Deviation

  spiWriteRegister(0x30, 0x8c);    // enable packet handler, msb first, enable crc,
  spiWriteRegister(0x32, 0x01);    // bcen4, hdch4, receive header check for byte 0, 1
  spiWriteRegister(0x33, 0x18);    // skipsyn1, hdlen3, fixpklen1, synclen2 prealen1 -> 1a
  spiWriteRegister(0x34, 0x28);    // prealen8, 32 byte preamble 
  spiWriteRegister(0x36, 0x2d);    // synchronize word 1
  spiWriteRegister(0x37, 0x00);    // synchronize word 2
  spiWriteRegister(0x38, 0x00);    // synchronize word 3
  spiWriteRegister(0x39, 0x00);    // synchronize word 4
  spiWriteRegister(0x3a, RF_Header); // tx header
  spiWriteRegister(0x3b, 0x00);
  spiWriteRegister(0x3c, 0x00);
  spiWriteRegister(0x3d, 0x00);
  spiWriteRegister(0x3e, packetlength);           // 6 byte normal packet

  //RX HEADER
  spiWriteRegister(0x3f, RF_Header);   // verify header
  spiWriteRegister(0x40, 0x00);
  spiWriteRegister(0x41, 0x00);
  spiWriteRegister(0x42, 0x00);
  spiWriteRegister(0x43, 0xff);    // all the bit to be checked
  spiWriteRegister(0x44, 0x00);    // all the bit to be checked
  spiWriteRegister(0x45, 0x00);    // zero bit to be checked
  spiWriteRegister(0x46, 0x00);    // zero bit to be checked

  spiWriteRegister(0x6d, RF_POWER); // 7 set power max power
 
  spiWriteRegister(0x79, hop_list[0]);    // start channel

  spiWriteRegister(0x7a, 0x06);    // 60khz step size (10khz x value) 

  spiWriteRegister(0x73, 0x00);    // Frequency Offset
  spiWriteRegister(0x74, 0x00);    // no offset

  unsigned short fb = CARRIER_FREQUENCY / 10000000 - 24;
  unsigned short fc = (CARRIER_FREQUENCY - (fb + 24) * 10000000) * 4 / 625;

  spiWriteRegister(0x75, 0x40 + (fb & 0x1f)); // sbsel=1 lower 5 bits is band
  spiWriteRegister(0x76, (fc >> 8));
  spiWriteRegister(0x77, (fc & 0xff));

  ItStatus1 = spiReadRegister(0x03); // read status, clear interrupt
  ItStatus2 = spiReadRegister(0x04);
}

uint8_t rfmGetDevType(void) { return spiReadRegister(0x00); }
uint8_t rfmGetDevVers(void) { return spiReadRegister(0x01); }
uint8_t rfmGetRSSI(void) { return spiReadRegister(0x26); }

void to_rx_mode(void)
{
  spiWriteRegister(0x08, 0x03);    // clear rx & tx fifo, disable multi packet
  spiWriteRegister(0x08, 0x00);    // clear fifo, disable multi packet
  spiWriteRegister(0x07, RF22B_PWRSTATE_RX );  // to rx mode
}

void RF_reset(void) {

  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
  spiWriteRegister(0x7e, 36);    // threshold for rx almost full, interrupt when 1 byte received
  spiWriteRegister(0x08, 0x03);    //clear fifo disable multi packet
  spiWriteRegister(0x08, 0x00);    // clear fifo, disable multi packet
  spiWriteRegister(0x07, RF22B_PWRSTATE_RX );  // to rx mode
  spiWriteRegister(0x05, RF22B_PACKET_RECEIVED_INTERRUPT | RF22B_PACKET_SENT_INTERRUPT);
  ItStatus1 = spiReadRegister(0x03);  //read the Interrupt Status1 register
  ItStatus2 = spiReadRegister(0x04);
}


void to_tx_mode(void) 
{
  spiWriteRegister(0x08, 0x03);    // clear rx & tx fifo, disable multi packet
  spiWriteRegister(0x08, 0x00);    // clear fifo, disable multi packet
  // ph +fifo mode
  spiWriteRegister(0x34, 0x08);  // 64 nibble = 32byte preamble

  spiWriteRegister(0x3e, packetlength);    // total tx 
  spiWritePacket();

  spiWriteRegister(0x07, RF22B_PWRSTATE_TX);    // to tx mode
}

void Hopping(void)
{
  RF_channel++;
  if (RF_channel >= Max_RF_channel) RF_channel=0;
  spiWriteRegister(0x79, hop_list[RF_channel]);
}

