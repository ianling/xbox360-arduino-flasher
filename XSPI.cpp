#include "XSPI.h"

uint8_t IN_FLASHMODE = 0;

void XSPI_Shutdown(void) {
  digitalWriteFast(SS, 1);
  digitalWriteFast(XX, 0);
  digitalWriteFast(EJ, 0);

  delay(50);

  digitalWriteFast(EJ, 1);
}

void XSPI_EnterFlashmode(void) {
  digitalWriteFast(XX, 0);

  delay(50);

  digitalWriteFast(SS, 0);
  digitalWriteFast(EJ, 0);

  delay(50);

  digitalWriteFast(XX, 1);
  digitalWriteFast(EJ, 1);

  delay(50);

  digitalWriteFast(SS, 0);
  IN_FLASHMODE = 1;
}

void XSPI_LeaveFlashmode(uint8_t force) {
  if (IN_FLASHMODE || force) {
    digitalWriteFast(SS, 1);
    digitalWriteFast(EJ, 0);

    delay(50);

    digitalWriteFast(XX, 0);
    digitalWriteFast(EJ, 1);

    IN_FLASHMODE = 0;
  }
}

uint8_t* XSPI_Read(uint8_t reg) {
  // returns 4 uint8_t's
  uint8_t* buff = new uint8_t[4];
  
  digitalWriteFast(SS, 0);
  //delay(2);

  XSPI_PutByte((reg << 2) | 1);
  XSPI_PutByte(0xFF);
  buff[0] = XSPI_FetchByte();
  buff[1] = XSPI_FetchByte();
  buff[2] = XSPI_FetchByte();
  buff[3] = XSPI_FetchByte();

  digitalWriteFast(SS, 1);

  return buff;
}

uint16_t XSPI_ReadWord(uint8_t reg) {
  uint16_t res;

  digitalWriteFast(SS, 0);
  //delay(2);

  XSPI_PutByte((reg << 2) | 1);
  XSPI_PutByte(0xFF);

  res = XSPI_FetchByte();
  res |= ((uint16_t)XSPI_FetchByte()) << 8;

  digitalWriteFast(SS, 1);

  return res;
}

uint8_t XSPI_ReadByte(uint8_t reg) {
  uint8_t res;

  digitalWriteFast(SS, 0);
  //delay(2);

  XSPI_PutByte((reg << 2) | 1);
  XSPI_PutByte(0xFF);

  res = XSPI_FetchByte();

  digitalWriteFast(SS, 1);

  return res;
}

void XSPI_Write(uint8_t reg, uint8_t *buf) {
  digitalWriteFast(SS, 0);
  //delay(2);

  XSPI_PutByte((reg << 2) | 2);
  XSPI_PutByte(*buf++);
  XSPI_PutByte(*buf++);
  XSPI_PutByte(*buf++);
  XSPI_PutByte(*buf++);

  digitalWriteFast(SS, 1);
}

void XSPI_WriteByte(uint8_t reg, uint8_t byte) {
  uint8_t data[] = {0, 0, 0, 0};
  data[0] = byte;

  XSPI_Write(reg, data);
}

void XSPI_WriteDword(uint8_t reg, uint32_t dword) {
  XSPI_Write(reg, (uint8_t *)&dword);
}

void XSPI_Write0(uint8_t reg) {
  uint8_t tmp[] = {0, 0, 0, 0};
  XSPI_Write(reg, tmp);
}

uint8_t XSPI_FetchByte() {
  uint8_t in = 0;
  digitalWriteFast(MOSI, 0);
  for (uint8_t i = 0; i < 8; i++) {
    digitalWriteFast(SCK, 1);
    in |= digitalReadFast(MISO) ? (1 << i) : 0x00;
    digitalWriteFast(SCK, 0);
  }
  return in;
}

void XSPI_PutByte(uint8_t out) {
  for (uint8_t i = 0; i < 8; i++) {
    if (out & (1 << i)) {
      digitalWriteFast(MOSI, 1);
    } else {
      digitalWriteFast(MOSI, 0);
    }
    digitalWriteFast(SCK, 1);
    digitalWriteFast(SCK, 0);
  }
}
