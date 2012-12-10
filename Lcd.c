#include "Lcd.h"

void WriteLine(const char* Str, unit8_t Line) {
    uint8_t StartAddr = 0x40 * Line; //DEFINE 0x40
    uint8_t i;

    PORTD &= ~(1 << PD0); /* Make sure RS is set to 0 */
    PORTB |= SET_CURSOR | StartAddr; /* Go to the start of the Line */

    PORTD |= (1 << PD0); /* Set RS to 1 */

    for(i = 0; i < 16; i ++) {
        PORTB = Str[i];
    }
}
