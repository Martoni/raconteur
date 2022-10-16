#include "dfr0299.h"

#define delay usleep

void setTimeOut(unsigned long timeOutDuration){
  _timeOutDuration = timeOutDuration;
}

void uint16ToArray(uint16_t value, uint8_t *array){
  *array = (uint8_t)(value>>8);
  *(array+1) = (uint8_t)(value);
}

void uart_send(uint8_t cmd, uint8_t feedback, uint16_t data);

uint16_t calculateCheckSum(uint8_t *buffer){
  uint16_t sum = 0;
  int i;
  for (i=Stack_Version; i<Stack_CheckSum; i++) {
    sum += buffer[i];
  }
  return -sum;
}

void sendStack(){
  /*if the ack mode is on wait until the last transmition*/
  if (_sending[Stack_ACK]) {
    while (_isSending) {
      delay(0);
      available();
    }
  }

#ifdef _DEBUG
  Serial.println();
  Serial.print(F("sending:"));
  for (int i=0; i<DFPLAYER_SEND_LENGTH; i++) {
    Serial.print(_sending[i],HEX);
    Serial.print(F(" "));
  }
  Serial.println();
#endif
//  _serial->write(_sending, DFPLAYER_SEND_LENGTH);
  write(fserial, _sending, DFPLAYER_SEND_LENGTH);
  _timeOutTimer = millis();
  _isSending = _sending[Stack_ACK];
 /* if the ack mode is off wait 10 ms after one transmition.*/
  if (!_sending[Stack_ACK]) {
    delay(10);
  }
}

void sendStack_cmd(uint8_t command){
  sendStack_cmd_arg(command, 0);
}

void sendStack_cmd_arg(uint8_t command, uint16_t argument){
  _sending[Stack_Command] = command;
  uint16ToArray(argument, _sending+Stack_Parameter);
  uint16ToArray(calculateCheckSum(_sending), _sending+Stack_CheckSum);
  sendStack();
}

void sendStack_cmd_2args(uint8_t command,
                         uint8_t argumentHigh,
                         uint8_t argumentLow){
  uint16_t buffer = argumentHigh;
  buffer <<= 8;
  sendStack_cmd_arg(command, buffer | argumentLow);
}

void enableACK(){
  _sending[Stack_ACK] = 0x01;
}

void disableACK(){
  _sending[Stack_ACK] = 0x00;
}

bool waitAvailable(unsigned long duration){
  unsigned long timer = millis();
  if (!duration) {
    duration = _timeOutDuration;
  }
  while (!available()){
    if (millis() - timer > duration) {
      return false;
    }
    delay(0);
  }
  return true;
}

bool begin(int fserial, bool isACK, bool doReset){
  _serial = fserial;

  if (isACK) {
    enableACK();
  }
  else{
    disableACK();
  }

  if (doReset) {
    reset();
    waitAvailable(2000);
    delay(200);
  }
  else {
    /* assume same state as with reset(): online*/
    _handleType = DFPlayerCardOnline;
  }

  return (readType() == DFPlayerCardOnline) ||\
                      (readType() == DFPlayerUSBOnline) || !isACK;
}

uint8_t readType(){
  _isAvailable = false;
  return _handleType;
}

uint16_t simple_read(){
  _isAvailable = false;
  return _handleParameter;
}

bool handleMessage(uint8_t type, uint16_t parameter){
  _receivedIndex = 0;
  _handleType = type;
  _handleParameter = parameter;
  _isAvailable = true;
  return _isAvailable;
}

bool handleError(uint8_t type, uint16_t parameter){
  handleMessage(type, parameter);
  _isSending = false;
  return false;
}

uint8_t readCommand(){
  _isAvailable = false;
  return _handleCommand;
}

void parseStack(){
  uint8_t handleCommand = *(_received + Stack_Command);
  if (handleCommand == 0x41) { /*handle the 0x41 ack feedback
                                 as a spcecial case,
                                 in case the pollusion of
                                 _handleCommand, _handleParameter,
                                 and _handleType.*/
    _isSending = false;
    return;
  }

  _handleCommand = handleCommand;
  _handleParameter =  arrayToUint16(_received + Stack_Parameter);

  switch (_handleCommand) {
    case 0x3D:
      handleMessage(DFPlayerPlayFinished, _handleParameter);
      break;
    case 0x3F:
      if (_handleParameter & 0x01) {
        handleMessage(DFPlayerUSBOnline, _handleParameter);
      }
      else if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardOnline, _handleParameter);
      }
      else if (_handleParameter & 0x03) {
        handleMessage(DFPlayerCardUSBOnline, _handleParameter);
      }
      break;
    case 0x3A:
      if (_handleParameter & 0x01) {
        handleMessage(DFPlayerUSBInserted, _handleParameter);
      }
      else if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardInserted, _handleParameter);
      }
      break;
    case 0x3B:
      if (_handleParameter & 0x01) {
        handleMessage(DFPlayerUSBRemoved, _handleParameter);
      }
      else if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardRemoved, _handleParameter);
      }
      break;
    case 0x40:
      handleMessage(DFPlayerError, _handleParameter);
      break;
    case 0x3C:
    case 0x3E:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4B:
    case 0x4C:
    case 0x4D:
    case 0x4E:
    case 0x4F:
      handleMessage(DFPlayerFeedBack, _handleParameter);
      break;
    default:
      handleError(WrongStack, 0);
      break;
  }
}

uint16_t arrayToUint16(uint8_t *array){
  uint16_t value = *array;
  value <<=8;
  value += *(array+1);
  return value;
}

bool validateStack(){
  return calculateCheckSum(_received) ==\
                                       arrayToUint16(_received+Stack_CheckSum);
}

bool available(){
  while (_serial->available()) {
    delay(0);
    if (_receivedIndex == 0) {
      _received[Stack_Header] = _serial->simple_read();
#ifdef _DEBUG
      Serial.print(F("received:"));
      Serial.print(_received[_receivedIndex],HEX);
      Serial.print(F(" "));
#endif
      if (_received[Stack_Header] == 0x7E) {
        _receivedIndex ++;
      }
    }
    else{
      _received[_receivedIndex] = _serial->simple_read();
#ifdef _DEBUG
      Serial.print(_received[_receivedIndex],HEX);
      Serial.print(F(" "));
#endif
      switch (_receivedIndex) {
        case Stack_Version:
          if (_received[_receivedIndex] != 0xFF) {
            return handleError(WrongStack, 0);
          }
          break;
        case Stack_Length:
          if (_received[_receivedIndex] != 0x06) {
            return handleError(WrongStack, 0);
          }
          break;
        case Stack_End:
#ifdef _DEBUG
          Serial.println();
#endif
          if (_received[_receivedIndex] != 0xEF) {
            return handleError(WrongStack, 0);
          }
          else{
            if (validateStack()) {
              _receivedIndex = 0;
              parseStack();
              return _isAvailable;
            }
            else{
              return handleError(WrongStack, 0);
            }
          }
          break;
        default:
          break;
      }
      _receivedIndex++;
    }
  }

  if (_isSending && (millis()-_timeOutTimer>=_timeOutDuration)) {
    return handleError(TimeOut, 0);
  }

  return _isAvailable;
}

void next(){
  sendStack_cmd(0x01);
}

void previous(){
  sendStack_cmd(0x02);
}

void play(int fileNumber){
  sendStack_cmd_arg(0x03, fileNumber);
}

void volumeUp(){
  sendStack_cmd(0x04);
}

void volumeDown(){
  sendStack_cmd(0x05);
}

void volume(uint8_t volume){
  sendStack_cmd_arg(0x06, volume);
}

void EQ(uint8_t eq) {
  sendStack_cmd_arg(0x07, eq);
}

void loop(int fileNumber) {
  sendStack_cmd_arg(0x08, fileNumber);
}

void outputDevice(uint8_t device) {
  sendStack_cmd_arg(0x09, device);
  delay(200);
}

void simple_sleep(){
  sendStack_cmd(0x0A);
}

void reset(){
  sendStack_cmd(0x0C);
}

void start(){
  sendStack_cmd(0x0D);
}

void simple_pause(){
  sendStack_cmd(0x0E);
}

void playFolder(uint8_t folderNumber, uint8_t fileNumber){
  sendStack_cmd_2args(0x0F, folderNumber, fileNumber);
}

void outputSetting(bool enable, uint8_t gain){
  sendStack_cmd_2args(0x10, enable, gain);
}

void enableLoopAll(){
  sendStack_cmd_arg(0x11, 0x01);
}

void disableLoopAll(){
  sendStack_cmd_arg(0x11, 0x00);
}

void playMp3Folder(int fileNumber){
  sendStack_cmd_arg(0x12, fileNumber);
}

void advertise(int fileNumber){
  sendStack_cmd_arg(0x13, fileNumber);
}

void playLargeFolder(uint8_t folderNumber, uint16_t fileNumber){
  sendStack_cmd_arg(0x14, (((uint16_t)folderNumber) << 12) | fileNumber);
}

void stopAdvertise(){
  sendStack_cmd(0x15);
}

void stop(){
  sendStack_cmd(0x16);
}

void loopFolder(int folderNumber){
  sendStack_cmd_arg(0x17, folderNumber);
}

void randomAll(){
  sendStack_cmd(0x18);
}

void enableLoop(){
  sendStack_cmd_arg(0x19, 0x00);
}

void disableLoop(){
  sendStack_cmd_arg(0x19, 0x01);
}

void enableDAC(){
  sendStack_cmd_arg(0x1A, 0x00);
}

void disableDAC(){
  sendStack_cmd_arg(0x1A, 0x01);
}

int readState(){
  sendStack_cmd(0x42);
  if (waitAvailable(0)) {
    if (readType() == DFPlayerFeedBack) {
      return simple_read();
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
  return -1;
}

int readVolume(){
  sendStack_cmd(0x43);
  if (waitAvailable(0)) {
    return simple_read();
  }
  else{
    return -1;
  }
  return -1;
}

int readEQ(){
  sendStack_cmd(0x44);
  if (waitAvailable(0)) {
    if (readType() == DFPlayerFeedBack) {
      return simple_read();
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
  return -1;
}

int readFileCounts_dev(uint8_t device){
  switch (device) {
    case DFPLAYER_DEVICE_U_DISK:
      sendStack_cmd(0x47);
      break;
    case DFPLAYER_DEVICE_SD:
      sendStack_cmd(0x48);
      break;
    case DFPLAYER_DEVICE_FLASH:
      sendStack_cmd(0x49);
      break;
    default:
      break;
  }

  if (waitAvailable(0)) {
    if (readType() == DFPlayerFeedBack) {
      return simple_read();
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
}

int readCurrentFileNumber_dev(uint8_t device){
  switch (device) {
    case DFPLAYER_DEVICE_U_DISK:
      sendStack_cmd(0x4B);
      break;
    case DFPLAYER_DEVICE_SD:
      sendStack_cmd(0x4C);
      break;
    case DFPLAYER_DEVICE_FLASH:
      sendStack_cmd(0x4D);
      break;
    default:
      break;
  }
  if (waitAvailable(0)) {
    if (readType() == DFPlayerFeedBack) {
      return simple_read();
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
}

int readFileCountsInFolder(int folderNumber){
  sendStack_cmd_arg(0x4E, folderNumber);
  if (waitAvailable(0)) {
    if (readType() == DFPlayerFeedBack) {
      return simple_read();
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
  return -1;
}

int readFolderCounts(){
  sendStack_cmd(0x4F);
  if (waitAvailable(0)) {
    if (readType() == DFPlayerFeedBack) {
      return simple_read();
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
  return -1;
}

int readFileCounts(){
  return readFileCounts_dev(DFPLAYER_DEVICE_SD);
}

int readCurrentFileNumber(){
  return readCurrentFileNumber_dev(DFPLAYER_DEVICE_SD);
}
