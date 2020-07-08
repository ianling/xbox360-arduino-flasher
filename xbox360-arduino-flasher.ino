#include "Arduino.h"
#include "XNAND.h"
#include "XSPI.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t* FlashConfig1;
uint8_t* FlashConfig2;

void setup() {
  Serial.begin(115200);
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

void dumpNand() {
  XSPI_EnterFlashmode();

  // why do we do this twice?
  FlashConfig1 = XSPI_Read(0);
  FlashConfig2 = XSPI_Read(0);

  // check if FlashConfig2 is all zeroes
  if (!FlashConfig2[0] && !FlashConfig2[1] && !FlashConfig2[2] && !FlashConfig2[3]) {
    Serial.write("ERROR: Your flash config is incorrect, check your wiring!\n");
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
  
  XSPI_LeaveFlashmode(0);
}

void flashNand() {
  // TODO
}

void loop() {
  // wait for command from serial
  while(!Serial.available()) {
    // ...
  }
  
  char command = Serial.read();
  switch(command) {
    case 'd':  // dump
      dumpNand();
      break;
    case 'f': // flash
      flashNand();
      break;
  }
  
  exit(0);
}
