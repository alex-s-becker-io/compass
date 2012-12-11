#ifndef __COMPASS_H
#define __COMPASS_H

/* Boolean defines */
typdef unsigned char boolean;
#define TRUE  1
#define FALSE 0

/* Pin defines */
#define PORT_ALL_HIGH   0xff
#define PORT_ALL_OUTPUT 0xff
#define PORT_ALL_PULLUP 0xff
//may not need

/* Direction Strings */
//May need to be const char* to play nice with the LCD
#define NORTH_S   "North"
#define NE_S      "Northeast"
#define EAST_S    "East"
#define SE_S      "Southeast"
#define SOUTH_S   "South"
#define SW_S      "Southwest"
#define WEST_S    "West"
#define NW_S      "Northwest"
#define DIRECTION "Degrees:     "

/* Bootup strings */
#define TITLE     "Compass Ver X.Y"
#define NAME      " -Alex Becker-"
#define BOOTUP    "Booting up"
#define WAITING   "Waiting on data"

/* Calibration strings */
#define CALIB     "Calibrating"
#define GARRUS    "Hurry up, Garrus" 

/* Directional defines */
#define OFFSET 0 /* Corrects the heading so that a reading of OFFSET is North */ //Probably will be 90
//Define the ranges here, figure out how to make this work.
// 338 <= N  <=  22
//  23 <= NE <=  67
//  68 <= E  <= 112
// 113 <= SE <= 157
// 158 <= S  <= 202
// 203 <= SW <= 247
// 248 <= W  <= 292
// 293 <= NW <= 337

/* Other defines */
//2 or 3 seconds?  Figure out later
#define STARTUP_DELAY 2000 /* Delay between startup screens */
#define BASE_TEN 10

/* Function prototypes */
boolean WriteI2c(uint16_t Addr, uint16_t Cmd, void* Data); //Will need adjusting, just a prototype atm
boolean ReadI2c(uint16_t Addr, uint16_t Cmd, void* Data);
int16_t CalculateDegHeading(int16_t X, int16_t Y);
int16_t Calibrate();

#endif /* __COMPASS_H */
