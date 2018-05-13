/* CODE BY Input/Output Space
 *
 * This file contains the code to run the portable compass.
 *
 * Author: Alex "iospace" Becker
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
#include <avr/power.h>
#include <string.h>
#include <util/twi.h>
#include <stdlib.h>
#include <math.h>

#include "compass.h"
#include "Lcd.h"
//#include "boolean.h"

/* Global Variables */
boolean DataReady = FALSE; /* Data is ready to be read */

/* The DataReady pin is hooked up to INT0 on the 328p */
ISR(INT0_vect) {
    DataReady = TRUE;
}

/* Configure the pins on the ATmega328p */
void InitDevice() {
    /* Disable all possible devices on the board */
    power_all_disable();

    /* LCD setup */
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

    LcdInit();

    /* Display title screen */
    LcdWriteString(TITLE, LCD_LINE_ONE);
    LcdWriteString(NAME, LCD_LINE_TWO);
    _delay_ms(STARTUP_DELAY);

    //LcdWriteString(BOOTUP, LCD_LINE_TWO);

    /* Calibration circuit */
    /* PD4 is used to determine if the calibration circuit is active or not */
    DDRD &= ~_BV(PD4); /* Configure PD4 as an input */
    PORTD |= _BV(PD4); /* Pullup PD4 */

    /* Configure the ADC */
    //Currently not implemented
    //power_adc_enable();

    /* Magnetometer set up */
    /* INT0 is attached to the DataReady pin on the magnometer, and will be used
     * to signal that data is ready to be read (obviously).  INT0 is set to go
     * off on a rising edge.
     */
    EIMSK |= _BV(INT0);
    EICRA |= _BV(ISC01) | _BV(ISC00);

    /* Configure TWI */
    power_twi_enable();
    TWCR = _BV(TWEN); /* Enable TWI */ //May not be needed here, but definitely elsewhere
    //Check to see if the magnetometer needs initialization from the ATmega
    //will need init
    //Display "Waiting on data" on line 2
}

/* It's the main function for the program, what more do you need to know? */
int main() {
    int16_t  Degrees = 0;
    uint8_t  SregSave;
    //int16_t  Correction = 0;
	
	/* Disable interrupts during setup */
	cli();

    Degrees = Degrees; //Because it's a warning otherwise, and I don't think I can get rid of it
    DataReady = FALSE;

    DDRC = (uint8_t)(-1);
    PORTC |= _BV(PC0);

    /* Setup the ATmega328p */
    InitDevice();

    //Debug
    PORTC |= _BV(PC1);

    /* Enable interrupts */
    sei();


    /* Wait till we get data */
    //while(!DataReady);

    /* Main loop */
    while(TRUE) {
        /* If there is pending data, process it */
        if(DataReady) {
            SregSave = SREG; /* Preserve the status register */
            /* We want the data retrevial completely performed without interruptions */
            cli();
            //Hand off to a function

            Degrees = ProcessData();

            DataReady = FALSE;

            /* Restore the status register */
            SREG = SregSave;
        }


        //Currently not used, add in later once supplies are received
        /* Check to see if the calibration circuit is active, and if so, adjust
         * change the mode to calibration mode
         */
        //if(bit_is_clear(PORTD, PD4)) {
            /* Check to so see if there is a low signal from the calibration circuit */
            //Correction = Calibrate(Correction, Degrees);
        //} else {
            /* If we aren't calibrating, display the direction and such */
            //LcdWriteString(HeadingString(Degrees), LCD_LINE_ONE);
        //}
        //Degrees += Correction;

        //LcdWriteString(("Degrees: %d", Degrees), LCD_LINE_TWO);
    }
    /* If this is ever called, I don't even know anymore */
    return 0;
}

/* Convert the degrees into a string
 *
 * Parameters:
 *     Degrees - the degree value of the heading vector
 *
 * Returns: The string representation
 */
char* HeadingString(int16_t Degrees) {
// 338 <= N  <=  22
//  23 <= NE <=  67
//  68 <= E  <= 112
// 113 <= SE <= 157
// 158 <= S  <= 202
// 203 <= SW <= 247
// 248 <= W  <= 292
// 293 <= NW <= 337
    if((338 <= Degrees) || (Degrees <= 22)) {
        return NORTH_S;
    } else if((23 <= Degrees) && (Degrees <= 67)) {
        return NE_S;
    } else if((68 <= Degrees) && (Degrees <= 112)) {
        return EAST_S;
    } else if((113 <= Degrees) && (Degrees <= 157)) {
        return SE_S;
    } else if((158 <= Degrees) && (Degrees <= 202)) {
        return SOUTH_S;
    } else if((203 <= Degrees) && (Degrees <= 247)) {
        return SW_S;
    } else if((248 <= Degrees) && (Degrees <= 292)) {
        return WEST_S;
    } else if((293 <= Degrees) && (Degrees <= 337)) {
        return NW_S;
    } else {
        return "ERROR";
    }
}

/* Calculate the heading based on the data passed from the magnetometer
 *
 * Parameters:
 *     X - The X value of the heading vector
 *     Y - The Y value of the heading vector
 *
 * Returns: The number of degrees from the X axis the vector is from
 */
int16_t CalculateDegHeading(int16_t X, int16_t Y) {
    double TempResult;
    //May not needed due to how atan is handled
    /*if(X == 0 && Y >= 0)
        return 0;
    else if (X == 0 && Y < 0)
        return 180;*/

    TempResult = atan2(Y, X);

    /* Convert the result to degrees */
    TempResult = -TempResult * RAD_TO_DEG;

    /* Correct the result */
    if(TempResult >= 360)
        TempResult -= 360; /* Adjust, shouldn't be an issue */
    else if(TempResult < 0)
        TempResult += 360; /* Convert to a positive degree */
    return floor(TempResult);
}

//Currently not implemented
/* Return the calibration value */
//int16_t Calibrate(int16_t CalibrationOffset, int16_t Degrees) {
    //int16_t RawValue = 0;

    //The following should get moved to its own file!
    //ADCSRA |= (_BV(ADEN) | _BV(ADIE)); /* Enable the ADC */ //May need to be offset
    //ADCSRA |= _BV(ADSC); /* Start the ADC conversion */

    //loop_until_bit_is_set(ADCSRA, ADIF); /* Wait for the ADC to finish */

    //RawValue = (ADCH << 8) | (ADCL); /* Read the value off the ADC */

    //Print out RawValue to LCD
    /* A result of 0x100 refers to the pot pointing straight up and down */ //TODO VERIFY THIS
    //RawValue -= 0x100;
    //5k (middle) = offset of 0
    //lower line is the offset
    //return CalibrationOffset;
//}

/* Process the data from the magnetometer
 *
 * Parameters: None
 *
 * Returns: The heading in degrees
 */
//Currently uses only the x and y data, but see if it's possible to use the z
//value to correct for it being held at an angle
int16_t ProcessData() {
    int16_t  MagX = 0;
    int16_t  MagY = 0;
    //int16_t  MagZ = 0; //may not need
    //uint8_t  Buffer[6];

    //MagZ = MagZ; //Warning killer hack YOU BETTER REMOVE THIS LATER ON FOOLE

    //read I2C data
    //calculate heading
    return CalculateDegHeading(MagX, MagY);
}
