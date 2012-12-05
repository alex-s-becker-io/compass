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

/* #includes */
//clean these out when done
#include <avr/interrupts.h>
#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <util/twi.h>

#include "compass.h"

unsigned char Data; /* Treated as a boolean */

//Pins and Directions
//SDA
//SCL

/* The DataReady pin is hooked up to INT0 on the 328p */
//Mind you if I do use a different magnetometer, this may be moot, but it should have one... blah
ISR(INT0_vect) {
    Data = TRUE;
}

int main() {
    //Variables!
    int16_t  MagX;
    int16_t  MagY;
    int16_t  MagZ; //may not need
    float    Degrees; //may end up being int16_t to save space
    char    *HeadingStr;

    Data = FALSE;
    
    cli();

    //startup
    //display welcome screen
    delay_ms(STARTUP_DELAY);
    //Display "getting data"

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

//Could possibly return a rounded int16_t, do I really need float precision?
int16_t Calculate2dHeading(int16_t X, int16_t Y) {
    double TempResult = 0.0;

    //Perform calcs here, yay trig, gonna use tangent
    TempResult = tan(X / Y);
    TempResult = (TempResult * 180) / M_PI; /* Convert the result to degrees */ //Define!
    return (int16_t)TempResult;
}
