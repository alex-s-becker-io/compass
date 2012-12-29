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
    do { 
        PORTD |= (1 << PD1); /* Toggle the enable pin */
        _delay_us(1); /* Delay long enough to that the LCD realizes there's a command */
        PORTD &= ~(1 << PD1); /* Toggle enable pin off */

        PortVal = PORTB; /* Read in the value of PORTB */
    } while(PortVal & BUSY_FLAG);
    //set R/W low
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
    _delay_us(100); /* Give enough time for the command to execute */ //TODO replace with function to impliment waiting
}

void InitLcd() {
    PORTD &= ~(1 << PD0); /* Make sure RS is set to 0 */
    
    //sofware init here!
    SendByte(FUNCTION_SET | 0x10, TRUE);
    _delay_us(4500); /* Delay for at least 4.1 ms */

    SendByte(FUNCTION_SET | 0x10, TRUE);
    _delay_us(150);

    SendByte(FUNCTION_SET | 0x10, TRUE); /* We should be good to go! */
    
    //Triggering Enable (and comments)
    SendByte(CLEAR_DISPLAY, TRUE);
    _delay_ms(200); /* The clear command needs a bit extra time */
    
    SendByte(ENTRY_MODE_SET | 0x02, TRUE); /* Set the entry mode to */
    _delay_us(50);
    
    SendByte(DISPLAY_CTRL | 0x04, TRUE); /* */
    _delay_us(50);
    
    SendByte(FUNCTION_SET | 0x1C, TRUE); /* */
    _delay_us(50);
}

void WriteLine(const char* Str, uint8_t Line) {
    uint8_t StartAddr = 0x40 * Line; //DEFINE 0x40
    uint8_t i;

    SendByte(SET_CURSOR_ADDR | StartAddr, TRUE); /* Go to the start of the Line */

    for(i = 0; i < strlen(Str); i ++) {
        SendByte(Str[i], FALSE);
        _delay_us(50);
    }
}
