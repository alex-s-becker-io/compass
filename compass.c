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
#include <avr/interrupts.h>
#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <util/twi.h>

#include "compass.h"
#include "Lcd.h"
#include "boolean.h"

/* Global Variables */
boolean Data;

/* The DataReady pin is hooked up to INT0 on the 328p */
//Mind you if I do use a different magnetometer, this may be moot, but it should
//have one... blah.  Interrupts are nice to have rather than blindly polling
ISR(INT0_vect) {
    Data = TRUE;
}

int main() {
    int16_t  Correction = OFFSET;
    int16_t  MagX;
    int16_t  MagY;
    int16_t  MagZ; //may not need
    int16_t  Degrees;
    char    *HeadingStr; //may not need

    cli(); /* Because interrupts while setting things up is a bad idea */
    Data = FALSE;

    //startup
    //display welcome screen
    delay_ms(STARTUP_DELAY);
    //display "booting up!" on line 2

    /* Configure INT0 */
    EIMSK |= (1 << INT0); /* Enable the INT0 interrupt */
    EICRA |= (1 << ISC01) | (1 << ISC00); /* Trigger on rising edge */

    /* Configure PD0 and PD1 */
    DDRD |= ((1 << PD0) | (1 << PD1));

    /* Configure PD4 */
    DDRD &= ~(1 << PD4); /* Configure PD4 as an input */
    PORTD |= (1 << PD4); /* Pullup PD4 */

    /* Configure PORTB */
    DDRB = (uint8_t)(-1); /* All PORTD pins are output for the LCD screen */

    /* Configure TWI */
    TWCR = (1 << TWEN); /* Enable TWI */ //May not be needed here, but definitely elsewhere
    //Check to see if the magnetometer needs initialization from the ATmega
    //will need init

    //Display "Waiting on data" on line 2
    sei(); /* Enable interrupts */

    /* Main loop */
    while(TRUE) {
        /* If there is pending data, process it */
        if(Data) {
            cli(); /* We want the update completely performed without interruptions */
            Data = FALSE;

            //read I2C data
            //calculate heading
            Degrees = CalculateDegHeading(MagX, MagY) + Correction;
            HeadingStr = malloc(sizeof(Degrees) + 1);
            HeadingStr = utoa(Degrees, HeadingStr, BASE_TEN);
            //Update LCD

            free(HeadingStr); /* Memleaks are bad, mmkay? */
            sei(); /* Re-enable interrupts */
        } 
        //Read in pin from switch
        //I'm such a hypocrite, but this is a bit easier to deal with given that
        //it won't be so intermittent
        if(!(PORTD & (1 << PD4))) {
            cli();
            Correction = Calibrate();
            sei();
        }
    }

    return 0; /* If this is ever called, I don't even know anymore */
}

int16_t CalculateDegHeading(int16_t X, int16_t Y) {
    double TempResult;
    if(X == 0 && Y >= 0)
        return 0;
    else if (X == 0 && Y < 0)
        return 180;
    
    TempResult = atan2(Y, X);
    TempResult = -TempResult * (180 / M_PI); /* Convert the result to degrees */ //Define!
    
    /* Correct the result */
    if(TempResult >= 360)
        TempResult -= 360; /* Adjust, shouldn't be an issue */
    else if(TempResult < 360)
        TempResult += 360; /* Convert to a positive degree */
    return round(TempResult);
}

/* Return the calibration value */
int16_t Calibrate() {
    int16_t CalibrationOffset = 0;
    //Display CALIB
    //start timer
    while(!(PORTD & (1 << PD4))) { 
        //Pull off the ADC
        //5k (middle) = offset of 0
        //on timer display HURRY UP GARRUS (3 seconds)
        //lower line is the offset
    }
    //Display "waiting on data"
}
