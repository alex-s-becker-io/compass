#ifndef __LCD_H
#define __LCD_H

#define CLEAR_DISPLAY 0x01
#define RETURN_HOME   0x02
#define ENTRY_MODE    0x04
#define DISPLAY_CTRL  0x08
#define DISPLAY_SHIFT 0x10
#define FUNCTION_SET  0x20
#define SET_CGRAM     0x40
#define SET_CURSOR    0x80

/* Line Defines */
#define LINE_ONE 0
#define LINE_TWO 1

/* Prototypes */
void WriteLine(const char* Str, unit8_t Line);

#endif /* __LCD_H */
