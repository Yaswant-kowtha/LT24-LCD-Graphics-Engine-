/*
 * Graphics.h
 *
 *  Created on: Apr 17, 2020
 *      Author: Yaswanth
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_
#include <stdbool.h>
//Error Codes
#define GRAPHICS_SUCCESS       0
#define GRAPHICS_ERRORNOINIT  -1

bool Graphics_isInitialised(void);
signed int Graphics_initialise(unsigned int lcd_pio_base,  unsigned int lcd_hw_base );
signed int Graphics_drawBox(unsigned int x1, unsigned int y1, unsigned int x2,  unsigned int y2, unsigned short colour, bool noFill,  unsigned short fillColour);
signed int Graphics_drawCircle(unsigned int x, unsigned int y, unsigned int r, unsigned short colour, bool noFill,  unsigned short fillColour);
signed int Graphics_drawLine(unsigned int x1, unsigned int y1, unsigned int x2,  unsigned int y2, unsigned short colour );
signed int Graphics_drawTriangle(unsigned int x1, unsigned int y1,  unsigned int x2,  unsigned int y2, unsigned int x3,  unsigned int y3, unsigned short colour, bool noFill,  unsigned short fillColour );
signed int Graphics_drawRandom(unsigned int n, unsigned short colour);

#endif /* GRAPHICS_H_ */
