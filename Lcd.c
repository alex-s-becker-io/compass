/* Lcd.c
 *
 * This file contains all the LCD specific functions
 */
#include "Lcd.h"
#include <avr/io.h>
#include "boolean.h"
#include <string.h>
#include <stdlib.h>

//TODO impliment busywait flag, timing ftw!  
/* Waits till the Busy Flag clears, then returns */
void WaitTillDone() {
    uint8_t PortVal;

    //set R/W high
    /* Make PB8 an input pin to read the busy flag */
    DDRB &= ~(1 << PB7);
    do { 
        PORTD |= (1 << PD1); /* Toggle the enable pin */
        _delay_us(1); /* Delay long enough to that the LCD realizes there's a command */
        PORTD &= ~(1 << PD1); /* Toggle enable pin off */

        PortVal = PORTB; /* Read in the value of PORTB */
    } while(PortVal & BUSY_FLAG);
    //set R/W low
    DDRB |= (1 << PB7);
}

/* Helper function to send an individual byte to the LCD */
void SendByte(uint8_t Data, boolean Command) {
    if(Command)
        PORTD &= ~(1 << PD0); /* Make sure RS is set to 0 */
    else
        PORTD |= (1 << PD0); /* Set RS to 1 */

    PORTB = Data; /* Write the data to the pins */

    PORTD |= (1 << PD1); /* Toggle the enable pin */
    _delay_us(1); /* Delay long enough to that the LCD realizes there's a command */
    PORTD &= ~(1 << PD1); /* Toggle enable pin off */
    WaitTillDone(); /* Give enough time for the command to execute */ //TODO replace with function to impliment waiting
}

/* Initialize the LCD display with the settings we want.  The LCD does start up
 * with mostly the desired settings from the get go, but this is just to be sure
 */
void InitLcd() {
    /* In case the LCD is going through its own power on initialization */
    WaitTillDone();

    SendByte(FUNCTION_SET | FS_EIGHT_BIT | FS_TWO_LINES, TRUE);

    SendByte(DISPLAY_CTRL | DC_DISPLAY_ON, TRUE);

    SendByte(CLEAR_DISPLAY, TRUE);
    
    SendByte(ENTRY_MODE_SET | EMS_INCRIMENT, TRUE); /* Set the entry mode to */ 
}

void WriteString(const char* Str, uint8_t Pos) {
    uint8_t i;

    SendByte(SET_CURSOR_ADDR | Pos, TRUE); /* Go to the start of the Line */

    for(i = 0; i < strlen(Str); i ++)
        SendByte(Str[i], FALSE);
}
