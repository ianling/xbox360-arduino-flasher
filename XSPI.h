#ifndef _XSPI_H_
#define _XSPI_H_

#include "Arduino.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define EJ 8      // Eject
#define XX 9      // Xbox Magic
#define SS 10     // Chip Select
#define MOSI 11   // Master out Slave in
#define MISO 12   // Master in Slave out
#define SCK 13    // Serial Clock

#define PINOUT(PIN) pinMode(PIN, OUTPUT)
#define PININ(PIN)  pinMode(PIN, INPUT_PULLUP)

#define PINHIGH(PIN) digitalWrite(PIN, 1)
#define PINLOW(PIN) digitalWrite(PIN, 0)

#define PINGET(PIN) digitalRead(PIN)

#define _delay_ms(MS) delay(MS)

void XSPI_Powerup(void);
void XSPI_Shutdown(void);

void XSPI_EnterFlashmode(void);
void XSPI_LeaveFlashmode(uint8_t force);

uint8_t* XSPI_Read(uint8_t reg);
uint16_t XSPI_ReadWord(uint8_t reg);
uint8_t XSPI_ReadByte(uint8_t reg);
void XSPI_BlindRead(uint8_t reg);

void XSPI_Write(uint8_t reg, uint8_t *buf);
void XSPI_WriteByte(uint8_t reg, uint8_t byte);
void XSPI_WriteDword(uint8_t reg, uint32_t dword);
void XSPI_Write0(uint8_t reg);

uint8_t XSPI_FetchByte(void);
void XSPI_PutByte(uint8_t out);

#endif
