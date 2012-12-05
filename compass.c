/* CODE BY Input/Output Space (iospace)
 *
 * This file contains the code to run the portable compass.
 *
 * Author: Alexandria "iospace" B.
 *
 * File name: compass.c
 *
 * Version: 0.0
 */

/* includes */
#include <avr/interrupts.h>
//"                " use for character length

bool Data;

ISR(INT0_vect) {
    Data = TRUE;
}

int main() {
    //Variables!

    //startup
    //display welcome screen
    //"Compass Ver X.Y "
    //" -Alex Becker-  "

    Data = FALSE;
    gei(); /* Enable interrupts */

    /* Main loop */
    while(TRUE) {
        
        /* If there is pending data, process it */
        if(Data) {
            cli(); /* Disable interrupts */
            //read I2C data
            //Update LCD 
            gei(); /* Re-enable interrupts */
        }
    }

    return 0;
}
