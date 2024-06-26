/*
 * tab:4
 *
 * text.h - font data and text to mode X conversion utility header file
 *
 * "Copyright (c) 2004-2009 by Steven S. Lumetta."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO 
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE 
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE, 
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:        Steve Lumetta
 * Version:       2
 * Creation Date: Thu Sep  9 22:08:16 2004
 * Filename:      text.h
 * History:
 *    SL    1    Thu Sep  9 22:08:16 2004
 *        First written.
 *    SL    2    Sat Sep 12 13:40:11 2009
 *        Integrated original release back into main code base.
 */

#ifndef TEXT_H
#define TEXT_H

/* The default VGA text mode font is 8x16 pixels. */
#define FONT_WIDTH   8
#define FONT_HEIGHT  16

#define IMAGE_X_DIM 320

// MAX BUF SIZE
// 320 * 18 = 5760
// 4 Planes per buf, plane size = buf size / 4
#define BUF_SIZE 5760
#define PLANE_SIZE BUF_SIZE / 4

/* PX_ROW -- number of pixels per row */
/* PX_PLANE_ROW -- number of pixels per row per plane */
#define PX_ROW 320
#define PX_PLANE_ROW PX_ROW / 4

/* set ON_COLOR to white and off color to palette 0x23 which will be updated */
#define ON_COLOR 0x0F

/* decimal 34 = 0x22 = wall palette */
#define OFF_COLOR 34

/* max number of characters on screen */
#define MAX_STRING_LENGTH 40

/* predefined palette size */
#define PALETTE_SIZE 64

/* number of registers allocated to user */
#define USER_PALETTE_SIZE 16
#define START_USER_PALETTE 0x20

/* wall color is second palette in user palettes */
#define WALL_PALETTE_INDEX 2

/* translate an input string into graphical display 
   and store in input buffer */
void text_to_graphics_routine( char* string, unsigned char* buffer );

/* translate an input string into graphical display 
   and store in input buffer in non modex format */
void fruit_text_to_graphics_routine( char* string, unsigned char* buffer );

/* Standard VGA text font. */
extern unsigned char font_data[256][16];

#endif /* TEXT_H */
