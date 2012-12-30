#ifndef __LCD_H
#define __LCD_H

/* Includes */
#include <stdint.h> 
#define F_CPU 1000000UL /* CPU speed = 1 MHz */
#include <util/delay.h>

/* Command instructions */
#define CLEAR_DISPLAY   0x01 /* Clears the display */
#define RETURN_HOME     0x02 /* Returns to the top left position on the screen */
#define ENTRY_MODE_SET  0x04 /* Sets the method in which new chars appear */
#define DISPLAY_CTRL    0x08
#define DISPLAY_SHIFT   0x10
#define FUNCTION_SET    0x20
#define SET_CGRAM_ADDR  0x40
#define SET_CURSOR_ADDR 0x80

/* Entry mode set defines */
#define EMS_INCRIMENT     0x02 /* Setting increments the cursor position */
#define EMS_SHIFT_DISPLAY 0x01 /* Shift the display */

/* Display on/off control defines */
#define DC_DISPLAY_ON   0x04 /* Turns the display on */
#define DC_CURSOR_ON    0x02 /* Display the cursor */
#define DC_CURSOR_BLINK 0x01 /* Makes the cursor blink */

/* Cursor/Display shift defines */
#define DS_SHIFT_DISPLAY 0x80
#define DS_SHIFT_RIGHT   0x40

/* Function set defines */
#define FS_EIGHT_BIT 0x10 /* Data is sent over 8 bits */
#define FS_TWO_LINES 0x08 /* The screen has two lines */
#define FS_FONT_SIZE 0x04 /* Use 5x10 font */

/* Other defines */
#define BUSY_FLAG 0x80 /* Active while processing command */

/* Line Defines */
#define LCD_LINE_ONE 0x00
#define LCD_LINE_TWO 0x40

/* Prototypes */
void WriteString(const char* Str, uint8_t Pos); // Maybe make WriteString ?
void WriteChar(const uint8_t Byte, uint8_t pos);
void InitLcd();

#endif /* __LCD_H */
