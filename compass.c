/* CODE BY Input/Output Space
 *
 * This file contains the code to run the portable compass.
 *
 * Author: Alexandria "iospace" Becker
 *
 * File name: compass.c
 *
 * Version: 0.1
 */

/* #includes */ //clean these out when done
#include <avr/interrupts.h>
#include <string.h>
#include <util/delay.h>
#include "compass.h"

bool Data;

/* The DataReady pin is hooked up to INT0 on the 328p */
ISR(INT0_vect) {
    Data = TRUE;
}

int main() {
    //Variables!
    int16_t  MagX;
    int16_t  MagY;
    int16_t  MagZ; //may not need
    float    Degrees;
    char    *HeadingStr;

    Data = FALSE;
    
    cli();

    //startup
    //display welcome screen

    delay_ms(STARTUP_DELAY);
    gei(); /* Enable interrupts */

    /* Main loop */
    while(TRUE) {
        /* If there is pending data, process it */
        if(Data) {
            cli(); /* We want the update completely performed without interruptions */
            Data = FALSE;

            //read I2C data
            //calculate heading
            Degrees = Calculate2dHeading(MagX, MagY);
            //Update LCD

            gei(); /* Re-enable interrupts */
        }
    }

    return 0;
}

float Calculate2dHeading(int16_t X, int16_t Y) {
    //Perform calcs here 
    return 0.0;
}
