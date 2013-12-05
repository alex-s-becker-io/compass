#ifndef __TWI_H
#define __TWI_H

#include <util/twi.h>
#include <avr/io.h>
#include "boolean.h"

/* Address Macros */
#define TW_SLAVE_ADDR_WRITE(a)  ((a & 0xFE) | TW_WRITE)
#define TW_SLAVE_ADDR_READ(a)   ((a & 0xFE) | TW_READ)

/* Start/Stop Macros */
#define TW_SEND_STOP    (TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN))
#define TW_SEND_START   (TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN))
#define TW_SEND_DATA    (TWCR = _BV(TWINT) | _BV(TWEN))
#define TW_SEND_ACK     (TWCR = _BV(TWINT) | _BV(TWEA))

/* Error define */
#define TW_SUCCESS ((uint8_t)~TW_STATUS_MASK)

/* Function Prototypes */
uint8_t TwWriteByte(uint8_t Address, uint8_t Offset, uint8_t Value);
uint8_t TwReadByte(uint8_t Address, uint8_t Offset, uint8_t *Value);

uint8_t TwWriteMultiple(uint8_t Address, uint8_t Offset,
                        uint8_t *Bytes, uint16_t Num);
uint8_t TwReadMultiple(uint8_t Address, uint8_t Offset,
                       uint8_t *Bytes, uint16_t Num);

#endif
