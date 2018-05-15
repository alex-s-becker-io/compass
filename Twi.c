#include "Twi.h"
#include <util/twi.h>

#include "Lcd.h" // debugging only
#include <stdio.h>

uint8_t TwWriteByte(uint8_t Address, uint8_t Offset, uint8_t Value) {
    uint8_t Status = TW_SUCCESS;
    char error_str[17];

    /* Start the TWI */
    TW_SEND_START;

    /* Wait till TWINT is set */
    loop_until_bit_is_set(TWCR, TWINT);

    /* If the TWI status is not START, then there is an error */
    if(TW_STATUS != TW_START) {
        Status = TW_STATUS;
        sprintf(error_str, "error 1: %x", Status);
        LcdWriteString(error_str, LCD_LINE_ONE);
        goto Cleanup;
    }

    /* Transmit the slave address and write bit */
    TWDR = TW_SLAVE_ADDR_WRITE(Address);
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        sprintf(error_str, "error 2: %x", Status);
        LcdWriteString(error_str, LCD_LINE_ONE);
        goto Cleanup;
    }

    /* Transmit the address within the device to be used */
    TWDR = Offset;
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_DATA_ACK) {
        Status = TW_STATUS;
        sprintf(error_str, "error 3: %x", Status);
        LcdWriteString(error_str, LCD_LINE_ONE);
        goto Cleanup;
    }

    /* Write the data to the bus */
    TWDR = Value;
    TW_SEND_DATA;

    loop_until_bit_is_set(TWCR, TWINT);

    if(TW_STATUS != TW_MT_DATA_ACK) {
        Status = TW_STATUS;
        sprintf(error_str, "error 4: %x", Status);
        LcdWriteString(error_str, LCD_LINE_ONE);
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
    uint8_t temp = 0;
    char error_str[17];

    /* Start the TWI */
    TW_SEND_START;

    /* Wait till TWINT is set */
    loop_until_bit_is_set(TWCR, TWINT);

    /* If the TWI status is not START, then there is an error */
    if(TW_STATUS != TW_START) {
        Status = TW_STATUS;
        //sprintf(error_str, "error 1: %x", Status);
        //LcdWriteString(error_str, LCD_LINE_TWO);
        goto Cleanup;
    }

    /* Transmit the slave address and write bit */
    TWDR = TW_SLAVE_ADDR_WRITE(Address);
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_SLA_ACK) {
        Status = TW_STATUS;
        //sprintf(error_str, "error 2: %x", Status);
        //LcdWriteString(error_str, LCD_LINE_TWO);
        goto Cleanup;
    }

    /* Transmit the address within the device to be used */
    TWDR = Offset;
    TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    /* Check for an ACK from the slave */
    if(TW_STATUS != TW_MT_DATA_ACK) {
        Status = TW_STATUS;
        //sprintf(error_str, "error 3: %x", Status);
        //LcdWriteString(error_str, LCD_LINE_TWO);
        goto Cleanup;
    }

    /* Repeat start the TWI */
    TW_SEND_START;
    loop_until_bit_is_set(TWCR, TWINT);
    if(TW_STATUS != TW_REP_START) {
        Status = TW_STATUS;
        //sprintf(error_str, "error 4: %x", Status);
        //LcdWriteString(error_str, LCD_LINE_TWO);
        goto Cleanup;
    }
/* Transmit the slave address and read bit */ TWDR = TW_SLAVE_ADDR_READ(Address); TW_SEND_DATA;
    loop_until_bit_is_set(TWCR, TWINT);

    if(TW_STATUS != TW_MR_SLA_ACK) {
        Status = TW_STATUS;
        //sprintf(error_str, "error 5: %x", Status);
        //LcdWriteString(error_str, LCD_LINE_TWO);
        goto Cleanup;
    }

    /* Send NACK back then stop */
    TWCR |= _BV(TWINT);

    loop_until_bit_is_set(TWCR, TWINT);
    if(TW_STATUS != TW_MR_DATA_NACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }
           
    /* Read the data from the bus */
    *Value = TWDR;

Cleanup:
    TW_SEND_STOP; 
    return Status;
}

uint8_t TwReadMultiple(uint8_t Address, uint8_t Offset,
                       uint8_t *Bytes, uint16_t Num) {
    uint16_t    i;
    uint8_t     Status = TW_SUCCESS;
    char error_str[17];

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
    if(TW_STATUS != TW_MT_DATA_ACK) {
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

    if(TW_STATUS != TW_MR_SLA_ACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    //PORTC |= _BV(PC0);

    /* Read the data from the bus */
    for(i = 0; i < (Num - 1); i++) {
        TW_SEND_ACK;

        PORTC |= _BV(PC1);
        loop_until_bit_is_set(TWCR, TWINT);
        PORTC |= _BV(PC0);

        if(TW_STATUS != TW_MR_DATA_ACK) {
            Status = TW_STATUS;
            sprintf(error_str, "error 2: %x", Status);
            LcdWriteString(error_str, LCD_LINE_ONE);
            goto Cleanup;
        }
        Bytes[i] = TWDR;
    }

    PORTC |= _BV(PC1);

    /* Send NACK back then stop */
    //TWCR |= _BV(TWINT);
    TW_SEND_NACK;

    loop_until_bit_is_set(TWCR, TWINT);
    if(TW_STATUS != TW_MR_DATA_NACK) {
        Status = TW_STATUS;
        goto Cleanup;
    }

    // Read in the last digit
    Bytes[Num - 1] = TWDR;

Cleanup:
    TW_SEND_STOP; 
    return Status;
}
