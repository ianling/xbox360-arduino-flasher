#include "XSPI.h"

uint8_t IN_FLASHMODE = 0;

void XSPI_Shutdown(void) {
  digitalWrite(SS, 1);
  digitalWrite(XX, 0);
  digitalWrite(EJ, 0);

  delay(50);

  digitalWrite(EJ, 1);
}

void XSPI_EnterFlashmode(void) {
  digitalWrite(XX, 0);

  delay(50);

  digitalWrite(SS, 0);
  digitalWrite(EJ, 0);

  delay(50);

  digitalWrite(XX, 1);
  digitalWrite(EJ, 1);

  delay(50);

  digitalWrite(SS, 0);
  IN_FLASHMODE = 1;
}

void XSPI_LeaveFlashmode(uint8_t force) {
  if (IN_FLASHMODE || force) {
    digitalWrite(SS, 1);
    digitalWrite(EJ, 0);

    delay(50);

    digitalWrite(XX, 0);
    digitalWrite(EJ, 1);

    IN_FLASHMODE = 0;
  }
}

uint8_t* XSPI_Read(uint8_t reg) {
  // returns 4 uint8_t's
  uint8_t* buff = new uint8_t[4];
  
  digitalWrite(SS, 0);
  //delay(2);

  XSPI_PutByte((reg << 2) | 1);
  XSPI_PutByte(0xFF);
  buff[0] = XSPI_FetchByte();
  buff[1] = XSPI_FetchByte();
  buff[2] = XSPI_FetchByte();
  buff[3] = XSPI_FetchByte();

  digitalWrite(SS, 1);

  return buff;
}

uint16_t XSPI_ReadWord(uint8_t reg) {
  uint16_t res;

  digitalWrite(SS, 0);
  //delay(2);

  XSPI_PutByte((reg << 2) | 1);
  XSPI_PutByte(0xFF);

  res = XSPI_FetchByte();
  res |= ((uint16_t)XSPI_FetchByte()) << 8;

  digitalWrite(SS, 1);

  return res;
}

uint8_t XSPI_ReadByte(uint8_t reg) {
  uint8_t res;

  digitalWrite(SS, 0);
  //delay(2);

  XSPI_PutByte((reg << 2) | 1);
  XSPI_PutByte(0xFF);

  res = XSPI_FetchByte();

  digitalWrite(SS, 1);

  return res;
}

void XSPI_Write(uint8_t reg, uint8_t *buf) {
  digitalWrite(SS, 0);
  //delay(2);

  XSPI_PutByte((reg << 2) | 2);
  XSPI_PutByte(*buf++);
  XSPI_PutByte(*buf++);
  XSPI_PutByte(*buf++);
  XSPI_PutByte(*buf++);

  digitalWrite(SS, 1);
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
  digitalWrite(MOSI, 0);
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(SCK, 1);
    in |= digitalRead(MISO) ? (1 << i) : 0x00;
    digitalWrite(SCK, 0);
  }
  return in;
}

void XSPI_PutByte(uint8_t out) {
  for (uint8_t i = 0; i < 8; i++) {
    if (out & (1 << i)) {
      digitalWrite(MOSI, 1);
    } else {
      digitalWrite(MOSI, 0);
    }
    digitalWrite(SCK, 1);
    digitalWrite(SCK, 0);
  }
}
