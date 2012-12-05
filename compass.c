/* CODE BY Input/Output Space (iospace)
 *
 * This file contains the code to run the portable compass.
 *
 * Author: Alexandria "iospace" B.
 *
 * File name: compass.c
 *
 * Version: 0.1
 */

/* includes */ //clean these out when done
#include <avr/interrupts.h>
#include <string.h>
#include <util/delay.h>
#include "compass.h"

bool Data;

ISR(INT0_vect) {
    Data = TRUE;
}

int main() {
    //Variables!
    Data = FALSE;
    cli();

    //startup
    //display welcome screen

    delay_ms(3000); //DEFINE
    gei(); /* Enable interrupts */

    /* Main loop */
    while(TRUE) {
        
        /* If there is pending data, process it */
        if(Data) { 
            /* Disable interrupts */
            cli();
            //read I2C data
            //Update LCD

            /* Re-enable interrupts */
            gei();
        }
    }

    return 0;
}
