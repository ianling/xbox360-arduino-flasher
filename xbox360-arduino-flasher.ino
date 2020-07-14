#include "Arduino.h"
#include "XNAND.h"
#include "XSPI.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void setup() {
  Serial.begin(460800);
  Serial.setTimeout(10000);
  pinMode(EJ, OUTPUT);
  pinMode(XX, OUTPUT);
  pinMode(SS, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT_PULLUP);

  digitalWriteFast(EJ, 1);
  digitalWriteFast(SS, 1);
  digitalWriteFast(XX, 1);
  digitalWriteFast(SCK, 1);
  digitalWriteFast(MOSI, 0);
}

void readFlashConfig() {
  uint8_t* FlashConfig1;
  uint8_t* FlashConfig2;

  XSPI_EnterFlashmode();
  
  // why do we do this twice?
  FlashConfig1 = XSPI_Read(0);
  FlashConfig2 = XSPI_Read(0);

  Serial.write(FlashConfig2, 4);

  // cleanup
  delete FlashConfig1;
  delete FlashConfig2;
  FlashConfig1 = NULL;
  FlashConfig2 = NULL;
  
  XSPI_LeaveFlashmode(0);
}

void dumpNand() {
  /* arduino reads data from the Xbox 360's NAND.
   * arduino sends the data to the PC via serial.
   */
  // read the NAND
  uint32_t start = 0x00;
  uint32_t blocks = 0x400;
  uint32_t wordsLeft = 0;
  uint32_t nextPage = start << 5;
  uint8_t readNow;
  uint32_t len = blocks * (0x4200 / 4); // 0x4200 = 16896 (blocksize), 4 bytes per word (we read in words, not individual bytes)

  XSPI_EnterFlashmode();
  
  while (len) {

    if (!wordsLeft) {
      XNAND_StartRead(nextPage);
      nextPage++;
      wordsLeft = 0x84;
    }

    readNow = (len < wordsLeft) ? len : wordsLeft;
    XNAND_ReadWords(readNow);

    wordsLeft -= readNow;
    len -= readNow;
  }
  
  XSPI_LeaveFlashmode(0);
}

void flashNand() {
  /* gist: arduino asks PC for data to write to Xbox 360 NAND
   * details: PC tells arduino the length of the data to be written.
   *          Arduino requests 1 page of data from the PC.
   *          Arduino writes that data into the 360's NAND buffer.
   *          Arduino instructs 360 to save the contents of the buffer to a particular page in the NAND.
   *          Repeat until all data has been written.
   */
  uint32_t wordsPerPage = 0x84;
  uint32_t bytesPerPage = wordsPerPage * 4;  // 4 bytes per word
  uint32_t currentPage = 0;
  uint32_t totalPages;
  
  XSPI_EnterFlashmode();
  
  // get length of data to write from host machine (uint32_t)
  uint8_t readBuff[4];
  uint32_t len;
  Serial.readBytes(readBuff, 4);
  len = ((uint32_t)readBuff[0] << 24) + ((uint32_t)readBuff[1] << 16) + ((uint32_t)readBuff[2] << 8) + readBuff[3];

  // write length back out to serial as a string of digits for verification
  char outBuff[11];  // 10 is the max # of chars produced when a uint32_t is converted to a string, plus 1 for null termination
  sprintf(outBuff, "%lu", len);
  Serial.write(outBuff, 11);


  len /= 4;  // we write in words, not individual bytes
  totalPages = ceil(len / wordsPerPage);

  
  uint8_t serialBuffer[bytesPerPage];
  while(currentPage < totalPages) {
      if(currentPage % 32 == 0) {
        XNAND_Erase(currentPage);  // erase after every 32 pages
      }
      XNAND_StartWrite();
      // tell host machine we're ready to receive a page of data
      Serial.write((uint8_t)0);
      Serial.readBytes(serialBuffer, bytesPerPage);
      // write data to the 360's buffer, word-by-word
      // if the file we're reading from ends somewhere in the middle of the page, it should still be okay
      XNAND_WriteProcess(serialBuffer, wordsPerPage);
      
      // tell the 360 to write the contents of the buffer to the current page
      XNAND_WriteExecute(currentPage);
      currentPage += 1;
  }
  
  XSPI_LeaveFlashmode(0);
}

void loop() {
  while(!Serial.available()) {
    // wait for command from serial
  }
  
  char command = Serial.read();
  switch(command) {
    case 'd':  // dump
      dumpNand();
      break;
    case 'f': // flash
      flashNand();
      break;
    case 'c': // check flash config
      readFlashConfig();
      break;
  }
}
