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

boolean Data;

//Pins and Directions
//SDA PC4
//SCL PC5
//INT0 PD2
//PORTB To LCD/LED

/* The DataReady pin is hooked up to INT0 on the 328p */
//Mind you if I do use a different magnetometer, this may be moot, but it should have one... blah
ISR(INT0_vect) {
    Data = TRUE;
}

int main() {
    int16_t  MagX;
    int16_t  MagY;
    //int16_t  MagZ; //may not need
    int16_t  Degrees;
    char    *HeadingStr; //may not need
    Data = FALSE;

    cli();

    //startup
    //display welcome screen
    delay_ms(STARTUP_DELAY);
    //display "booting up!"

    /* Configure INT0 */ //Check for pullup req later
    PORTD = 0; /* Disable pullups on PORTD */
    DDRD  = 0; /* Configure PORTD as input */
    EIMSK = (1 << INT0); /* Enable the INT0 interrupt */
    EICRA = (1 << ISC01) || (1 << ISC00); /* Trigger on rising edge */

    /* Configure PORTB */
    PORTB = 0xFF; /* Set PORTD to output high */ //DEFINE THESE
    DDRB  = 0xFF; /* All PORTD pins are output */

    /* Configure TWI */
    TWCR = (1 << TWEN); /* Enable TWI */

    //Display "Waiting on data"
    sei(); /* Enable interrupts */

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

            sei(); /* Re-enable interrupts */
        }
    }

    return 0;
}

int16_t Calculate2dHeading(int16_t X, int16_t Y) { 
    double TempResult = tan(X / Y);
    TempResult = TempResult * (180 / M_PI); /* Convert the result to degrees */ //Define!
    if(TempResult >= 360)
        TempResult -= 360; /* Adjust, shouldn't be an issue */
    return round(TempResult);
}
