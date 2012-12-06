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
//Get rid of XYY and YXY directions, would be easier to calculate
#define NORTH_S "North"
#define NE_S    "Northeast"
#define EAST_S  "East"
#define SE_S    "Southeast"
#define SOUTH_S "South"
#define SW_S    "Southwest"
#define WEST_S  "West"
#define NW_S    "Northwest"
#define TITLE   "Compass Ver X.Y"
#define NAME    " -Alex Becker-"

/* Directional defines */
#define OFFSET 0 /* Corrects the heading so that a reading of OFFSET is north */
//Define the ranges here, figure out how to make this work.
// 338 <= N  <=  22
//  23 <= NE <=  67
//  68 <= E  <= 112
// 113 <= SE <= 157
// 158 <= S  <= 202
// 203 <= SW <= 247
// 248 <= W  <= 292
// 293 <= NW <= 337

/* LCD Defines */
//will need adjusting as I get deeper into the project
#define LINE_ONE 0 //1
#define LINE_TWO 1 //2

/* Other defines */
//2 or 3 seconds?  Figure out later
#define STARTUP_DELAY 2000 /* Delay between startup screens */

/* Function prototypes */
//writeI2c
//readI2c
int16_t Calculate2dHeading(int16_t X, int16_t Y);
void writeLCD(const char* str, uint8_t line);

#endif /* __COMPASS_H */
