#ifndef __COMPASS_H
#define __COMPASS_H

/* Boolean defines */
#define TRUE  1
#define FALSE 0

/* Pin defines */
//TODO fill this out, may not be needed

/* Direction Strings */
#define NORTH_S "North"
#define NEE_S   "North North East"
#define NE_S    "NorthEast"
#define ENE_S   "East North East"
#define EAST_S  "East"
#define ESE_S   "East South East"
#define SE_S    "SouthEast"
#define SSE_S   "South South East"
#define SOUTH_S "South"
#define SWW_S   "South South West"
#define SW_S    "Southwest"
#define WSW_S   "West South West"
#define WEST_S  "West"
#define WNW_S   "West North West"
#define NW_S    "North West"
#define NNW_S   "North North West"
#define TITLE   "Compass Ver X.Y"
#define NAME    " -Alex Becker-"

/* Directional defines */
#define CORRECTION 0 /* Corrects the heading */
//Define the ranges here, figure out how to make this work.
#define NORTH_LOWER 347.5 /* -12.5 */
#define NORTH_UPPER  12.5

/* Function prototypes */
//TODO fill this out
//writeI2c
//readI2c

/**
*/
writeLCD(const char* str);

#endif /* __COMPASS_H */
