#include "Twi.h"
#include <util/twi.h>

uint8_t TwWriteByte(uint8_t Address, uint8_t Offset, uint8_t Value) {
    uint8_t Status = TW_SUCCESS;

    /* Start the TWI */
    TW_SEND_START;

    /* Wait till TWINT is set */
    loop_until_bit_is_set(TWCR, TWINT);

    /* If the TWI status is not START, then there is an error */
    if(TW_STATUS != TW_START) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Transmit the slave address and write bit */
    TWDR = TW_SLAVE_ADDR_WRITE(Address);
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Transmit the address within the device to be used */
    TWDR = Offset;
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Write the data to the bus */
    TWDR = Value;
    TW_SEND_DATA;

    loop_until_bit_is_set(TWCR, TWINT);

    if(TW_STATUS != TW_MT_DATA_ACK)
        Status = TW_STATUS;
        goto Cleanup;
    }

Cleanup:
    TW_SEND_STOP; 
    return Status;
}

uint8_t TwWriteMultiple(uint8_t Address, uint8_t Offset,
                        uint8_t *Bytes, uint16_t Num) {
    uint16_t    i;
    uint8_t     Status = TW_SUCCESS;

    /* Start the TWI */
    TW_SEND_START;

    /* Wait till TWINT is set */
    loop_until_bit_is_set(TWCR, TWINT);

    /* If the TWI status is not START, then there is an error */
    if(TW_STATUS != TW_START) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Transmit the slave address and write bit */
    TWDR = TW_SLAVE_ADDR_WRITE(Address);
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Transmit the address within the device to be used */
    TWDR = Offset | 0x80;
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Write the data to the bus */
    for(i = 0; i < Num; i ++) {
        TWDR = Bytes[i];
        TW_SEND_DATA;
        loop_until_bit_is_set(TWCR, TWINT);

        if(TW_STATUS != TW_MT_DATA_ACK) {
            Status = TW_STATUS;
            goto Cleanup;
        }
    }

Cleanup:
    TW_SEND_STOP;
    return Status;
}

uint8_t TwReadByte(uint8_t Address, uint8_t Offset, uint8_t *Value) {
    uint8_t Status = TW_SUCCESS;

    /* Start the TWI */
    TW_SEND_START;

    /* Wait till TWINT is set */
    loop_until_bit_is_set(TWCR, TWINT);

    /* If the TWI status is not START, then there is an error */
    if(TW_STATUS != TW_START) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Transmit the slave address and write bit */
    TWDR = TW_SLAVE_ADDR_WRITE(Address);
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Transmit the address within the device to be used */
    TWDR = Offset;
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Repeat start the TWI */
    TW_SEND_START;
    loop_until_bit_is_set(TWCR, TWINT);
    if(TW_STATUS != TW_REP_START) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Transmit the slave address and read bit */
    TWDR = TW_SLAVE_ADDR_READ(Address);
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);
    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Read the data from the bus */
    *Value = TWDR;

    /* Send NACK back then stop */
    TWCR |= _BV(TWINT);

Cleanup:
    TW_SEND_STOP; 
    return TW_SUCCESS;
}

uint8_t TwReadMultiple(uint8_t Address, uint8_t Offset,
                       uint8_t *Bytes, uint16_t Num) {
    uint16_t    i;
    uint8_t     Status = TW_SUCCESS;

    /* Start the TWI */
    TW_SEND_START;

    /* Wait till TWINT is set */
    loop_until_bit_is_set(TWCR, TWINT);

    /* If the TWI status is not START, then there is an error */
    if(TW_STATUS != TW_START) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Transmit the slave address and write bit */
    TWDR = TW_SLAVE_ADDR_WRITE(Address);
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Transmit the address within the device to be used */
    TWDR = Offset;
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Repeat start the TWI */
    TW_SEND_START;
    loop_until_bit_is_set(TWCR, TWINT);

    if(TW_STATUS != TW_REP_START) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Transmit the slave address and read bit */
    TWDR = TW_SLAVE_ADDR_READ(Address);
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    /* Read the data from the bus */
    for(i = 0; i < (Num - 1); i++) {
        Bytes[i] = TWDR;
        //Send ACK
    }

    Bytes[Num - 1] = TWDR;

    /* Send NACK back then stop */
    TWCR |= _BV(TWINT);

Cleanup:
    TW_SEND_STOP; 
    return Status;
}
