#include "Lcd.h"
#include <string.h>
#include <util/delay.h>

void InitLcd() {
    PORTD &= ~(1 << PD0); /* Make sure RS is set to 0 */ 
    
    //ADD IN DELAYS (and comments)
    PORTB = CLEAR_DISPLAY;
    delay_ms(200);
    
    PORTB = ENTRY_MODE | 0x02; /* Set the entry mode to */
    delay_us(50);
    
    PORTB = DISPLAY_CTRL | 0x04; /* */
    delay_us(50);
    
    PORTB = FUNCTION_SET | 0x1C; /* */
    delay_us(50);
}

void WriteLine(const char* Str, unit8_t Line) {
    uint8_t StartAddr = 0x40 * Line; //DEFINE 0x40
    uint8_t i;

    PORTD &= ~(1 << PD0); /* Make sure RS is set to 0 */
    PORTB |= SET_CURSOR | StartAddr; /* Go to the start of the Line */
    delay_us(50);

    PORTD |= (1 << PD0); /* Set RS to 1 */

    for(i = 0; i < strlen(Str); i ++) {
        PORTB = Str[i];
        delay_us(50);
    }
}
