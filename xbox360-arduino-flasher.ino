#include "Arduino.h"
#include "XNAND.h"
#include "XSPI.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t* FlashConfig1;
uint8_t* FlashConfig2;

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
  // why do we do this twice?
  FlashConfig1 = XSPI_Read(0);
  // i'm not good at c
  char printbuf[40];
  sprintf(printbuf, "Flash config1: 0x%02x%02x%02x%02x\n", FlashConfig1[3], FlashConfig1[2], FlashConfig1[1], FlashConfig1[0]);
  Serial.write(printbuf);

  FlashConfig2 = XSPI_Read(0);
  char printbuf5[40];
  sprintf(printbuf5, "Flash config2: 0x%02x%02x%02x%02x\n", FlashConfig2[3], FlashConfig2[2], FlashConfig2[1], FlashConfig2[0]);
  Serial.write(printbuf5);

  // check if FlashConfig2 is all zeroes
  if (!FlashConfig2[0] && !FlashConfig2[1] && !FlashConfig2[2] && !FlashConfig2[3]) {
    Serial.write("Your flash config is incorrect, check your wiring!\n");
    exit(1);
  }

  // cleanup
  delete FlashConfig1;
  delete FlashConfig2;
  FlashConfig1 = NULL;
  FlashConfig2 = NULL;

  // read the NAND
  uint32_t start = 0x00;
  uint32_t blocks = 0x400;
  Serial.write("Reading NAND.... please be patient\n");
  read_nand(start, blocks);
  

  Serial.write("Leaving flashmode!\n");
  XSPI_LeaveFlashmode(0);

  exit(0);
}
