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
//#include "boolean.h"

/* Global Variables */
boolean DataReady; /* Data is ready to be read */

/* The DataReady pin is hooked up to INT0 on the 328p */
ISR(INT0_vect) {
    DataReady = TRUE;
}

/* Configure the pins on the ATmega328p */
void InitDevice() {
    /* LCD pins */
    /* All PORTB pins are data pins for the LCD screen */
    DDRB = (uint8_t)(-1);

    /* PDO, PD1, and PD3 are all used as control pins for the LCD screen.
     * PD0 is used as the R/S pin, which determines whether the LCD is getting a
     * command or a character
     * PD1 is the Enable pin, which signals the LCD to read the data pins
     * PD3 is the R/W Pin which is used to activate the Busy flag on the LCD and
     * is used for timing
     */
    DDRD |= (_BV(PD0) | _BV(PD1) | _BV(PD3));

    InitLcd();

    /* Display title screen */
    WriteString(TITLE, LCD_LINE_ONE);
    WriteString(NAME, LCD_LINE_TWO);
    _delay_ms(STARTUP_DELAY);

    WriteString(BOOTUP, LCD_LINE_TWO);

    /* Calibration circuit */
    /* PD4 is used to determine if the calibration circuit is active or not */
    DDRD &= ~_BV(PD4); /* Configure PD4 as an input */
    PORTD |= _BV(PD4); /* Pullup PD4 */ 

    /* Configure the ADC */

    /* Magnetometer set up */
    /* INT0 is attached to the DataReady pin on the magnometer, and will be used
     * to signal that data is ready to be read (obviously)
     */
    EIMSK |= _BV(INT0); /* Enable the INT0 interrupt */
    EICRA |= _BV(ISC01) | _BV(ISC00); /* Trigger on rising edge */

    /* Configure TWI */
    TWCR = _BV(TWEN); /* Enable TWI */ //May not be needed here, but definitely elsewhere
    //Check to see if the magnetometer needs initialization from the ATmega
    //will need init
    //Display "Waiting on data" on line 2
}

/* It's the main function for the program, what more do you need to know? */
int main() {
    int16_t  Degrees = 0;
    uint8_t  SregSave;
    int16_t  Correction = 0;

    Degrees = Degrees; //Because it's a warning otherwise 
    DataReady = FALSE; 

    cli(); /* Because interrupts while setting things up is a bad idea */
    InitDevice();
    sei(); /* Enable interrupts */

    while(!DataReady); /* Wait till we get data */

    /* Main loop */
    while(TRUE) {
        /* If there is pending data, process it */
        if(DataReady) {
            SregSave = SREG; /* Preserve the status register */
            cli(); /* We want the update completely performed without interruptions */
            DataReady = FALSE;
            //Hand off to a function

            Degrees = ProcessData();

            SREG = SregSave; /* Restore the status register */
        } 

        if(bit_is_clear(PORTD, PD4)) {
            /* Check to so see if there is a low signal from the calibration circuit */
            Correction = Calibrate(Correction, Degrees);
        } else {
            /* If we aren't calibrating, display the direction and such */
        } 
        Degrees += Correction;
        //WriteString(("Degrees: %d", Degrees), LCD_LINE_TWO); //TODO fix this!
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
    return floor(TempResult);
}

/* Return the calibration value */
int16_t Calibrate(int16_t CalibrationOffset, int16_t Degrees) {
    int16_t AdcValue = 0;

    ADCSRA |= (_BV(ADEN) | _BV(ADIE)); /* Enable the ADC */ //May need to be offset
    ADCSRA |= _BV(ADSC); /* Start the ADC conversion */

    loop_until_bit_is_set(ADCSRA, ADIF); /* Wait for the ADC to finish */ 

    AdcValue = (ADCH << 8) | (ADCL); /* Read the value off the ADC */

    AdcValue -= 0x100; /* a result of 0x100 refers to the pot pointing straight up and down */
    //5k (middle) = offset of 0
    //lower line is the offset
    return CalibrationOffset;
}

int16_t ProcessData() {
    int16_t  MagX = 0;
    int16_t  MagY = 0;
    int16_t  MagZ = 0; //may not need

    MagZ = MagZ; //Warning killer hack YOU BETTER REMOVE THIS LATER ON FOOLE

    //read I2C data
    //calculate heading
    return CalculateDegHeading(MagX, MagY);
}
