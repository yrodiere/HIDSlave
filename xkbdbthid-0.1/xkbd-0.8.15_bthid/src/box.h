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

#ifndef _BOX_H_
#define _BOX_H_

box* box_new(void);
button *box_add_button(box *bx, button *but);
box *box_add_box(box *bx, box *b);
void box_list_contents(box *bx);

#endif


