#include "Arduino.h"
#include "XNAND.h"
#include "XSPI.h"
#include "unpack.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t FlashConfig1[4];
uint8_t FlashConfig2[4];

void setup() {
  Serial.begin(115200);
  // add 5 second delay for fun
  delay(5000);
  Serial.write("BEGIN\n");
  PINOUT(EJ);
  PINOUT(XX);
  PINOUT(SS);
  PINOUT(SCK);
  PINOUT(MOSI);
  PININ(MISO);

  PINHIGH(EJ);
  PINHIGH(SS);
  PINHIGH(XX);
  PINHIGH(SCK);
  PINLOW(MOSI);
}

void read_nand(uint32_t start, uint32_t blocks) {
  uint32_t wordsLeft = 0;
  uint32_t nextPage = start << 5;

  uint32_t len = blocks * (0x4200 / 4); // block size + spares
  while (len) {
    uint8_t readNow;

    if (!wordsLeft) {
      XNAND_StartRead(nextPage);
      nextPage++;
      wordsLeft = 0x84;
    }

    readNow = (len < wordsLeft) ? len : wordsLeft;
    XNAND_ReadWords(readNow);

    //buffer += (readNow * 4);
    wordsLeft -= readNow;
    len -= readNow;
  }
}

void loop() {
  Serial.write("Entering flashmode...\n");
  XSPI_EnterFlashmode();

  Serial.write("Reading flash config...\n");
  uint32_t flash_config1;
  uint32_t flash_config2;

  XSPI_Read(0, FlashConfig1);
  flash_config1 = unpack_uint32_le(FlashConfig1);
  char printbuf[40];
  sprintf(printbuf, "Flash config1: 0x%08x\n", flash_config1);
  Serial.write(printbuf);

  XSPI_Read(0, FlashConfig2);
  flash_config2 = unpack_uint32_le(FlashConfig2);
  char printbuf5[40];
  sprintf(printbuf5, "Flash config2: 0x%08x\n", flash_config2);
  Serial.write(printbuf5);

  if (flash_config2 <= 0) {
    Serial.write("Your flash config is incorrect, check your wiring!\n");
    exit(1);
  }

  //nand_to_file("nand1.bin");
  //nand_to_file("nand2.bin");
  //nand_to_file("nand3.bin");
  uint32_t start = 0x00;
  uint32_t blocks = 0x400;
  Serial.write("Reading NAND.... please be patient\n");
  read_nand(start, blocks);
  

  Serial.write("Leaving flashmode!\n");
  XSPI_LeaveFlashmode(0);

  exit(0);
}
