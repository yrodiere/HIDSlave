/* 
   xkbd - xlib based onscreen keyboard.

   Copyright (C) 2001 Matthew Allum

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
*/

#ifndef _BUTTON_H_
#define _BUTTON_H_

button* button_new(keyboard *k);

int _max3( int a, int b, int c );

GC _createGC(Display *display, Window win);

int _XColorFromStr(Display *display, XColor *col, const char *defstr);

void button_set_layout(button *b, char *txt);

void button_set_bg_col(button *b, char *txt);

void button_set_fg_col(button *b, char *txt);

void button_set_pixmap(button *b, char *txt);

void button_set_txt(button *b, char *txt);

void button_set_scancode(button *b, char *txt);

void button_set_txt_ks(button *b, char *txt);

void button_set_shift_txt(button *b, char *txt);

void button_set_shift_ks(button *b, char *txt);

void button_set_mod_txt(button *b, char *txt);

void button_set_mod_ks(button *b, char *txt);
void button_set_slide_ks(button *b, char *txt, int dir);
int _button_get_txt_size(button *b, char *txt);
int button_calc_c_width(button *b);

int button_calc_c_height(button *b);

int button_get_c_height(button *b);

int button_set_b_size(button *b, int size);

void button_render(button *b, int mode);
void button_paint(button *b);

int button_get_abs_x(button *b);
int button_get_abs_y(button *b);

#endif
