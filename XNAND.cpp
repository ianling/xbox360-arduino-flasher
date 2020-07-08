#include "XNAND.h"
#include "XSPI.h"
#include <stdio.h>

void XNAND_ClearStatus() {
  uint8_t* tmp;
  tmp = XSPI_Read(0x04);
  XSPI_Write(0x04, tmp);
  // cleanup
  delete tmp;
}

uint16_t XNAND_GetStatus() { return XSPI_ReadWord(0x04); }

uint8_t XNAND_WaitReady(uint16_t timeout) {
  do {
    if (!(XSPI_ReadByte(0x04) & 0x01)) {
      return 1;
    }
    delay(10);
  } while (timeout--);

  printf("Delay on WaitReady reached\n");
  return 0;
}

uint16_t XNAND_Erase(uint32_t blockNum) {
  uint8_t* tmp;

  XNAND_ClearStatus();

  tmp = XSPI_Read(0);
  tmp[0] |= 0x08;
  XSPI_Write(0, tmp);
  delete tmp;
  tmp = NULL;

  XSPI_WriteDword(0x0C, blockNum << 9);

  if (!XNAND_WaitReady(0x1000))
    return 0x8001;

  XSPI_WriteByte(0x08, 0xAA);
  XSPI_WriteByte(0x08, 0x55);

  if (!XNAND_WaitReady(0x1000))
    return 0x8002;

  XSPI_WriteByte(0x08, 0x5);

  if (!XNAND_WaitReady(0x1000))
    return 0x8003;

  return XNAND_GetStatus();
}

uint16_t XNAND_StartRead(uint32_t blockNum) {
  uint16_t res;

  XNAND_ClearStatus();

  XSPI_WriteDword(0x0C, blockNum << 9);

  XSPI_WriteByte(0x08, 0x03);

  if (!XNAND_WaitReady(0x1000))
    return 0x8011;

  res = XNAND_GetStatus();

  XSPI_Write0(0x0C);

  return res;
}

void XNAND_ReadWords(uint8_t wordCount) {
  while (wordCount--) {
    XSPI_Write0(0x08);
    XSPI_BlindRead(0x10);
  }
}

void XNAND_StartWrite() {
  XNAND_ClearStatus();
  XSPI_Write0(0x0C);
}

void XNAND_WriteProcess(uint8_t *buf, uint8_t wordCount) {
  while (wordCount--) {
    XSPI_Write(0x10, buf);
    XSPI_WriteByte(0x08, 0x01);
    buf += 4;
  }
}

uint16_t XNAND_WriteExecute(uint32_t blockNum) {
  XSPI_WriteDword(0x0C, blockNum << 9);

  XSPI_WriteByte(0x08, 0x55);
  XSPI_WriteByte(0x08, 0xAA);
  XSPI_WriteByte(0x08, 0x4);

  if (!XNAND_WaitReady(0x1000))
    return 0x8021;

  return XNAND_GetStatus();
}
