/* Lcd.c
 *
 * This file contains all the LCD specific functions
 */
#include "Lcd.h"
#include <avr/io.h>
#include "boolean.h"
#include <string.h>
#include <stdlib.h>

void ToggleEnable() {
    PORTD |= _BV(PD1);
    _delay_us(10);
    PORTD &= ~(_BV(PD1));
}

/* Waits till the Busy Flag clears, then returns */
void WaitTillDone() {
    boolean busy = TRUE;
    /* Set R/W high and R/S low */
    PORTD |= _BV(PD4);
    PORTD &= ~(_BV(PD0));

    /* Make PORTB input */
    DDRB = 0;
    do {
        /* Toggle the enable pin */
        //ToggleEnable();
        PORTD |= _BV(PD1);
        PORTD &= ~(_BV(PD1));
        _delay_us(10);
        busy = bit_is_set(PORTB, PB7);
    } while(busy);

    /* Set R/W low */
    PORTD &= ~(_BV(PD4));

    /* Restore PORTB to output */
    DDRB = (uint8_t)(-1);
}

/* Helper function to send an individual byte to the LCD */
void SendByte(uint8_t Data, boolean Command) {
    if(Command)
        PORTD &= ~(_BV(PD0)); /* Make sure RS is set to 0 */
    else
        PORTD |= _BV(PD0); /* Set RS to 1 */

    PORTB = Data; /* Write the data to the pins */

    /* Tell the LCD controller that there's a command waiting on the data pins */
    ToggleEnable();

    /* Return RS to 0 if set (if not set this effectively does nothing */
    PORTD &= ~(_BV(PD0));

    /* Give enough time for the command to execute */
    //WaitTillDone();
    _delay_ms(5);
}

/* Initialize the LCD display with the settings we want.  The LCD does start up
 * with mostly the desired settings from the get go, but this is just to be sure
 */
void LcdInit() {
    /* In case the LCD is going through its own power on initialization */
    _delay_ms(50);
    //WaitTillDone();

    SendByte(FUNCTION_SET | FS_EIGHT_BIT | FS_TWO_LINES, TRUE);

    //SendByte(DISPLAY_CTRL | DC_DISPLAY_ON, TRUE);
    SendByte(DISPLAY_CTRL, TRUE);

    SendByte(CLEAR_DISPLAY, TRUE);

    SendByte(ENTRY_MODE_SET | EMS_INCRIMENT, TRUE); /* Set the entry mode to */

    SendByte(DISPLAY_CTRL | DC_DISPLAY_ON, TRUE);
}

void LcdWriteString(const char* Str, uint8_t Pos) {
    uint8_t i;

    SendByte(SET_CURSOR_ADDR | Pos, TRUE); /* Go to the start of the Line */

    for(i = 0; i < strlen(Str); i ++)
        SendByte(Str[i], FALSE);
}
