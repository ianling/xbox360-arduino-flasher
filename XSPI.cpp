#include "XSPI.h"

uint8_t IN_FLASHMODE = 0;

void XSPI_Shutdown(void) {
  PINHIGH(SS);
  PINLOW(XX);
  PINLOW(EJ);

  delay(50);

  PINHIGH(EJ);
}

void XSPI_EnterFlashmode(void) {
  PINLOW(XX);

  delay(50);

  PINLOW(SS);
  PINLOW(EJ);

  delay(50);

  PINHIGH(XX);
  PINHIGH(EJ);

  delay(50);

  PINLOW(SS);
  IN_FLASHMODE = 1;
}

void XSPI_LeaveFlashmode(uint8_t force) {
  if (IN_FLASHMODE || force) {
    PINHIGH(SS);
    PINLOW(EJ);

    delay(50);

    PINLOW(XX);
    PINHIGH(EJ);

    IN_FLASHMODE = 0;
  }
}

uint8_t* XSPI_Read(uint8_t reg) {
  // returns 4 uint8_t's
  uint8_t* buff = new uint8_t[4];
  
  PINLOW(SS);
  //delayMicroseconds(2);

  XSPI_PutByte((reg << 2) | 1);
  XSPI_PutByte(0xFF);
  buff[0] = XSPI_FetchByte();
  buff[1] = XSPI_FetchByte();
  buff[2] = XSPI_FetchByte();
  buff[3] = XSPI_FetchByte();

  PINHIGH(SS);

  return buff;
}

uint16_t XSPI_ReadWord(uint8_t reg) {
  uint16_t res;

  PINLOW(SS);
  //delayMicroseconds(2);

  XSPI_PutByte((reg << 2) | 1);
  XSPI_PutByte(0xFF);

  res = XSPI_FetchByte();
  res |= ((uint16_t)XSPI_FetchByte()) << 8;

  PINHIGH(SS);

  return res;
}

uint8_t XSPI_ReadByte(uint8_t reg) {
  uint8_t res;

  PINLOW(SS);
  //delayMicroseconds(2);

  XSPI_PutByte((reg << 2) | 1);
  XSPI_PutByte(0xFF);

  res = XSPI_FetchByte();

  PINHIGH(SS);

  return res;
}

void XSPI_Write(uint8_t reg, uint8_t *buf) {
  PINLOW(SS);
  //delayMicroseconds(2);

  XSPI_PutByte((reg << 2) | 2);
  XSPI_PutByte(*buf++);
  XSPI_PutByte(*buf++);
  XSPI_PutByte(*buf++);
  XSPI_PutByte(*buf++);

  PINHIGH(SS);
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
  PINLOW(MOSI);
  for (uint8_t i = 0; i < 8; i++) {
    PINHIGH(SCK);
    in |= PINGET(MISO) ? (1 << i) : 0x00;
    PINLOW(SCK);
  }
  return in;
}

void XSPI_PutByte(uint8_t out) {
  for (uint8_t i = 0; i < 8; i++) {
    if (out & (1 << i)) {
      PINHIGH(MOSI);
    } else {
      PINLOW(MOSI);
    }
    PINHIGH(SCK);
    PINLOW(SCK);
  }
}
