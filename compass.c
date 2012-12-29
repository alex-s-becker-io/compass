/* CODE BY Input/Output Space
 *
 * This file contains the code to run the portable compass.
 *
 * Author: Alexandria "iospace" Becker
 *
 * File name: compass.c
 *
 * Version: 0.1
 *
 * Purpose: To serve as the main file for the compass.  The main loop is here
 *
 */

/* #includes */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <util/twi.h>
#include <stdlib.h>
#include <math.h>

#include "compass.h"
#include "Lcd.h"
#include "boolean.h"

#include <util/delay.h>

/* Global Variables */
boolean Data; /* Data is ready to be read */

/* The DataReady pin is hooked up to INT0 on the 328p */
//Mind you if I do use a different magnetometer, this may be moot, but it should
//have one... blah.  Interrupts are nice to have rather than blindly polling
ISR(INT0_vect) {
    Data = TRUE;
}

/* It's the main function for the program, what more do you need to know? */
int main() {
    int16_t  Correction = OFFSET;
    int16_t  MagX = 0;
    int16_t  MagY = 0;
    //int16_t  MagZ; //may not need
    int16_t  Degrees;
    uint8_t  SregSave;
    char    *HeadingStr; //may not need

    cli(); /* Because interrupts while setting things up is a bad idea */
    Data = FALSE;

    //startup
    //display welcome screen
    _delay_ms(STARTUP_DELAY);
    //display "booting up!" on line 2

    /* INT0 is attached to the DataReady pin on the magnometer, and will be used
     * to signal that data is ready to be read (obviously)
     */
    EIMSK |= (1 << INT0); /* Enable the INT0 interrupt */
    EICRA |= (1 << ISC01) | (1 << ISC00); /* Trigger on rising edge */

    /* Configure PD0 and PD1 */
    //TODO The other pin, R/W? is on PD as well
    //TODO globalize the LCD pins?
    /* PDO, PD1, and PDX are all used as the control pins for the LCD screen.
     * PD0 is used as the R/S pin, which determines whether the LCD is getting a
     * command or a character
     * PD1 is the Enable pin, which signals the LCD to read the data pins
     * PDX is the R/W Pin which is used to activate the Busy flag on the LCD and
     * is used for timing
     */
    DDRD |= ((1 << PD0) | (1 << PD1));

    /* PD4 is used to determine if the calibration circuit is active or not */
    DDRD &= ~(1 << PD4); /* Configure PD4 as an input */
    PORTD |= (1 << PD4); /* Pullup PD4 */

    /* All PORTB pins are data pins for the LCD screen */
    DDRB = (uint8_t)(-1);

    /* Configure the ADC */

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
            SregSave = SREG; /* Preserve the status register */
            cli(); /* We want the update completely performed without interruptions */
            Data = FALSE;

            //read I2C data
            //calculate heading
            Degrees = CalculateDegHeading(MagX, MagY) + Correction;
            HeadingStr = malloc(sizeof(Degrees) + 1);
            HeadingStr = utoa(Degrees, HeadingStr, BASE_TEN); /* Convert the degrees to a string */
            //Update LCD

            free(HeadingStr); /* Memleaks are bad, mmkay? */
            //sei(); /* Re-enable interrupts */
            SREG = SregSave; /* Restore the status register */
        } 
        //Read in pin from switch
        //I'm such a hypocrite, but this is a bit easier to deal with given that
        //it won't be so intermittent
        /* Check to so see if there is a low signal from the calibration circuit */
        if(!(PORTD & (1 << PD4))) {
            /* We don't need SREG preservation, technically doing it wrong but I'm
             * lazy here dammit!
             */
            EIMSK &= ~(1 << INT0); /* disable the INT0 interrupt */
            Correction = Calibrate();
            EIMSK |= (1 << INT0); /* Enable the INT0 interrupt */
        }
    }

    return 0; /* If this is ever called, I don't even know anymore */
}

int16_t CalculateDegHeading(int16_t X, int16_t Y) {
    double TempResult;
    //May not needed due to how atan is handled
    /*if(X == 0 && Y >= 0)
        return 0;
    else if (X == 0 && Y < 0)
        return 180;*/
    
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
    int16_t CalibrationOffset = OFFSET;
    int16_t AdcValue = 0;

    ADCSRA |= ((1 << ADEN) | (1 << ADIE)); /* Enable the ADC */ //May need to be offset
    //Display CALIB
    //start timer
    while(!(PORTD & (1 << PD4))) { 
        ADCSRA |= (1 << ADSC); /* Start the ADC conversion */
        while(!(ADCSRA & ADIF)); /* Wait for the ADC to finish */ 

        AdcValue = (ADCH << 8) | (ADCL); /* Read the value off the ADC */

        AdcValue -= 0x1f; /* a result of 0x1f refers to the pot pointing straight up and down */
        //5k (middle) = offset of 0
        //on timer display HURRY UP GARRUS (3 seconds)
        //lower line is the offset
    }
    //Display "waiting on data"
    return CalibrationOffset;
}
