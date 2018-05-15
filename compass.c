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
#include <stdio.h>
#include <math.h>

#include "compass.h"
#include "Lcd.h"
#include "Twi.h"
//#include "boolean.h"

/* Global Variables */
// Will have interrupts for the rotary encoder
// boolean encoder_press = FALSE
// boolean encoder_turn = FALSE

/* The DataReady pin is hooked up to INT0 on the 328p */
/*ISR(INT0_vect) {
    encoder_press = TRUE;
}*/

/* Configure the pins on the ATmega328p */
/* Returns true is init is successful, false if not */
boolean InitDevice() {
    uint8_t test, status;
    char error_str[16];

    /* Disable all possible devices on the board */
    power_all_disable();

    test = 0;
    status = 0;

    /* LCD setup */
    /* All PORTB pins are data pins for the LCD screen */
    DDRB = PORT_ALL_OUTPUT;

    /* PDO, PD1, and PD3 are all used as control pins for the LCD screen.
     * PD0 is used as the R/S pin, which determines whether the LCD is getting a
     * command or a character
     * PD1 is the Enable pin, which signals the LCD to read the data pins
     * PD3 is the R/W Pin which is used to activate the Busy flag on the LCD and
     * is used for timing
     */
    DDRD |= (_BV(PD0) | _BV(PD1) | _BV(PD3));

    /* Initialize the LCD */
    LcdInit();

    /* Display title screen */
    LcdWriteString(TITLE, LCD_LINE_ONE);
    LcdWriteString(NAME, LCD_LINE_TWO);
    _delay_ms(STARTUP_DELAY);

    LcdWriteString(BOOTUP, LCD_LINE_TWO);

    /* Configure TWI */
    power_twi_enable();
    TWCR = _BV(TWEN); /* Enable TWI */

    /* Magnetometer set up */

    /* Set the magnetometer in continuous conversion mode */
    status = TwWriteByte(MAGNETOMETER_ADDR, MR_REG_M, 0x00);
    if(status != TW_SUCCESS) {
        sprintf(error_str, "setup error: %x", status);
        LcdWriteString(error_str, LCD_LINE_ONE);
        return FALSE;
    }

    status = TwWriteByte(MAGNETOMETER_ADDR, CRA_REG_M, 0x14);
    if(status != TW_SUCCESS) {
        sprintf(error_str, "setup error: %x", status);
        LcdWriteString(error_str, LCD_LINE_TWO);
        return FALSE;
    }

    LcdWriteString(WAITING, LCD_LINE_TWO);

    /* "prime" the magnetometer */
    status = tw_read_reg_byte(MAGNETOMETER_ADDR, MR_REG_M, &test);
    if(status != TW_SUCCESS) {
        sprintf(error_str, "setup error: %x", status);
        LcdWriteString(error_str, LCD_LINE_TWO);
        return FALSE;
    }

    return TRUE;
}

/* It's the main function for the program, what more do you need to know? */
int main() {
    double  Degrees = 0;
    uint16_t heading1, heading2;
    char degree_str[16];
    //double  Correction = 0;
	
	/* Disable interrupts during setup */
	//cli();

    // Debugging purposes
    DDRC = (uint8_t)(-1);

    /* Setup the ATmega328p.  If setup fails, exit. */
    if(!InitDevice()) {
        return 0;
    }

    /* Enable interrupts */
    // Currently not needed
    //sei();

    /* Main loop */
    while(1) {
        Degrees = ProcessData();

        /* Convert the double to ints for proper display */
        heading1 = (uint16_t)Degrees;
        heading2 = (uint16_t)((Degrees - heading1) * 100);

        /* Display the heading */
        LcdWriteString(HeadingString(Degrees), LCD_LINE_ONE);

        /* Display the direction in degrees */
        sprintf(degree_str, "Degrees: %3d.%d ", heading1, heading2);
        LcdWriteString(degree_str, LCD_LINE_TWO);
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
double CalculateDegHeading(int16_t X, int16_t Y) {
    double TempResult;
    //May not needed due to how atan is handled
    /*if(X == 0 && Y >= 0)
        return 0;
    else if (X == 0 && Y < 0)
        return 180;*/

    TempResult = atan2(Y, X);

    /* Convert the result to degrees */
    TempResult = TempResult * RAD_TO_DEG;

    /* Correct the result */
    if(TempResult >= 360)
        TempResult -= 360; /* Adjust, shouldn't be an issue */
    else if(TempResult < 0)
        TempResult += 360; /* Convert to a positive degree */
    //return floor(TempResult);
    return TempResult;
}

/* Process the data from the magnetometer
 *
 * Parameters: None
 *
 * Returns: The heading in degrees
 */
//Currently uses only the x and y data, but see if it's possible to use the z
//value to correct for it being held at an angle
double ProcessData() {
    int16_t  mag_x = 0;
    int16_t  mag_y = 0;
    int16_t  mag_z = 0; //may not need
    //int16_t  heading;
    uint8_t x_low, x_high, y_low, y_high, z_low, z_high;
    char temp_str[17];

    //read I2C data
    // Register order: X_H, X_L, Z_H, Z_L, Y_H, Y_L
    tw_read_reg_byte(MAGNETOMETER_ADDR, OUT_X_H_M, &x_high);
    tw_read_reg_byte(MAGNETOMETER_ADDR, OUT_X_L_M, &x_low);
    mag_x = (int16_t)((uint16_t)x_low | ((uint16_t)x_high << 8));

    tw_read_reg_byte(MAGNETOMETER_ADDR, OUT_Z_H_M, &z_high);
    tw_read_reg_byte(MAGNETOMETER_ADDR, OUT_Z_L_M, &z_low);
    mag_z = (int16_t)((uint16_t)z_low | ((uint16_t)z_high << 8));

    tw_read_reg_byte(MAGNETOMETER_ADDR, OUT_Y_H_M, &y_high);
    tw_read_reg_byte(MAGNETOMETER_ADDR, OUT_Y_L_M, &y_low);
    mag_y = (int16_t)((uint16_t)y_low | ((uint16_t)y_high << 8));

    //sprintf(temp_str, "%2x,%2x,%d    ", x_high, x_low, mag_x);
    //LcdWriteString(temp_str, LCD_LINE_ONE);

    //heading = floor(CalculateDegHeading(mag_x, mag_y));

    //sprintf(temp_str, "%2x,%2x,%d,%3d", y_low, y_high, mag_y, heading);
    //LcdWriteString(temp_str, LCD_LINE_TWO);

    //calculate heading
    return CalculateDegHeading(mag_x, mag_y);
}
