#include "Twi.h"
#include <util/twi.h>

/* If an error occurs, stop the TWI so it's not in use still */
uint8_t TwiError(uint8_t Status) {
    TW_SEND_STOP;
    return Status;
}

uint8_t TwWriteByte(uint8_t Address, uint8_t Offset, uint8_t Value) {
    /* Start the TWI */
    //TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    TW_SEND_START;

    /* Wait till TWINT is set */
    loop_until_bit_is_set(TWCR, TWINT);

    /* If the TWI status is not START, then there is an error */
    if(TW_STATUS != TW_START)
        return TwError(TW_STATUS);

    /* Transmit the slave address and write bit */
    TWDR = SLAVE_ADDR_WRITE(Address);
    //TWCR = _BV(TWINT) | _BV(TWEN); 
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK)
        return TwError(TW_STATUS);

    /* Transmit the address within the device to be used */
    TWDR = Offset;
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK)
        return TwError(TW_STATUS);

    /* Write the data to the bus */
    TWDR = Value;
    TW_SEND_DATA;

    loop_until_bit_is_set(TWCR, TWINT);

    if(TW_STATUS != TW_MT_DATA_ACK)
        return TwError(TW_STATUS);

    //TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
    TW_SEND_STOP;

    return TW_SUCCESS;
}
