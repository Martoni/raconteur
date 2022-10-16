#ifndef DFR0299
#define DFR0299

#include <stdint.h>
#include <unistd.h>

#define DFPLAYER_EQ_NORMAL 0
#define DFPLAYER_EQ_POP 1
#define DFPLAYER_EQ_ROCK 2
#define DFPLAYER_EQ_JAZZ 3
#define DFPLAYER_EQ_CLASSIC 4
#define DFPLAYER_EQ_BASS 5

#define DFPLAYER_DEVICE_U_DISK 1
#define DFPLAYER_DEVICE_SD 2
#define DFPLAYER_DEVICE_AUX 3
#define DFPLAYER_DEVICE_SLEEP 4
#define DFPLAYER_DEVICE_FLASH 5

#define DFPLAYER_RECEIVED_LENGTH 10
#define DFPLAYER_SEND_LENGTH 10

#define TimeOut 0
#define WrongStack 1
#define DFPlayerCardInserted 2
#define DFPlayerCardRemoved 3
#define DFPlayerCardOnline 4
#define DFPlayerPlayFinished 5
#define DFPlayerError 6
#define DFPlayerUSBInserted 7
#define DFPlayerUSBRemoved 8
#define DFPlayerUSBOnline 9
#define DFPlayerCardUSBOnline 10
#define DFPlayerFeedBack 11

#define Busy 1
#define Sleeping 2
#define SerialWrongStack 3
#define CheckSumNotMatch 4
#define FileIndexOut 5
#define FileMismatch 6
#define Advertise 7

#define Stack_Header 0
#define Stack_Version 1
#define Stack_Length 2
#define Stack_Command 3
#define Stack_ACK 4
#define Stack_Parameter 5
#define Stack_CheckSum 7
#define Stack_End 9

#define bool int
#define false (-1)
#define true (1)

unsigned long _timeOutTimer;
unsigned long _timeOutDuration = 500;

/* serial port */

int _serial = -1;

/* Buffers */
uint8_t _received[DFPLAYER_RECEIVED_LENGTH];
uint8_t _sending[DFPLAYER_SEND_LENGTH] =\
         {0x7E, 0xFF, 06, 00, 01, 00, 00, 00, 00, 0xEF};

uint8_t _receivedIndex=0;

void sendStack();
void sendStack_cmd(uint8_t command);
void sendStack_cmd_arg(uint8_t command, uint16_t argument);
void sendStack_cmd_2args(uint8_t command,
                         uint8_t argumentHigh,
                         uint8_t argumentLow);

void enableACK();
void disableACK();

void uint16ToArray(uint16_t value,uint8_t *array);

uint16_t arrayToUint16(uint8_t *array);

uint16_t calculateCheckSum(uint8_t *buffer);


void parseStack();
bool validateStack();

uint8_t device = DFPLAYER_DEVICE_SD;

uint8_t _handleType;
uint8_t _handleCommand;
uint16_t _handleParameter;
bool _isAvailable = false;
bool _isSending = false;

bool handleMessage(uint8_t type, uint16_t parameter);
bool handleError(uint8_t type, uint16_t parameter);

uint8_t readCommand();

/*bool begin(Stream& stream, bool isACK = true, bool doReset = true);*/
bool begin(Stream& stream, bool isACK, bool doReset);

bool waitAvailable(unsigned long duration/* = 0=*/);

bool available();

uint8_t readType();

uint16_t simple_read();

void setTimeOut(unsigned long timeOutDuration);

void next();

void previous();


void play(int fileNumber /*=1*/);

void volumeUp();

void volumeDown();

void volume(uint8_t volume);

void EQ(uint8_t eq);

void loop(int fileNumber);

void outputDevice(uint8_t device);

void simple_sleep();

void reset();

void start();

void simple_pause();

void playFolder(uint8_t folderNumber, uint8_t fileNumber);

void outputSetting(bool enable, uint8_t gain);

void enableLoopAll();

void disableLoopAll();

void playMp3Folder(int fileNumber);

void advertise(int fileNumber);

void playLargeFolder(uint8_t folderNumber, uint16_t fileNumber);

void stopAdvertise();

void stop();

void loopFolder(int folderNumber);

void randomAll();

void enableLoop();

void disableLoop();

void enableDAC();

void disableDAC();

int readState();

int readVolume();

int readEQ();

int readFileCounts_dev(uint8_t device);

int readCurrentFileNumber_dev(uint8_t device);

int readFileCountsInFolder(int folderNumber);

int readFileCounts();

int readFolderCounts();

int readCurrentFileNumber();

#endif /* DFR0299 */
