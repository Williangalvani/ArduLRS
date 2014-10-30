
//#define RX
//#define TX
//#define Board_TX  // orangerx transmitter module
//#define Board_HWSPI  // my selfbuilt rx/tx module
#define Board_RX  // orangerx rx/tx module

//0 = 2400 baud, 1 = 4800, 2 = 9600, 3 = 19200
#define filterset 0

// the mesage needs 60ms, so 100ms is adequate
#define SendMS 66     // normal transmit / receive timing
#define ReceiveMS 70  // time to detect missing packet
#define SyncMS 250     // receive / hop timing when not synced
#define RssiMS  45     // time to read RSSI
#define MaxFail 10     // number of missed packets to go unsynced mode

#if defined(Board_TX)
  #define TX
#endif
#if defined(Board_RX)
  #define RX
#endif
  
#if defined(Board_HWSPI)
  #define HARDWARE_SPI
#else
  #define BITBANG_SPI
#endif

//#define ICP
//#define PPMSUM_OUTPUT
//#define SWAP_GPIOS //blackboard RX TX ant switch

//######### TRANSMISSION VARIABLES ##########
#define CARRIER_FREQUENCY 435003000  // Hz  startup frequency

//###### RF DEVICE ID HEADER #######
// Change this 1 byte for isolating your transmission,
// RF module accepts only data with same header
static unsigned char RF_Header = 'l';

// 7 == 100mW, 6 == 50mW, 5 == 25mW, 4 == 13mW  524mw
// 3 == 6mW, 2 == 3mW, 1 == 1.6mW,  0 == 1.3mW 40mw
#define RF_POWER 7
//#define NOBOOSTER  //GPIO switch

#define RC_CHANNELS 6
#define packetlength 5

//###### HOPPING CHANNELS #######
// put only single channel to the list to disable hopping
#define HOPPING
#define DEFAULT_HOPLIST 0,16,32,48
#define Max_RF_channel 4

//###### SERIAL PORT SPEED - just debugging atm. #######
#define SERIAL_BAUD_RATE 115200 //115.200 baud serial port speed


