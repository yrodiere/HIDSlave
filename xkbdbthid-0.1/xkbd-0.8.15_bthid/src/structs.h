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

#ifndef _STRUCT_H_
#define _STRUCT_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif
#define BUTTON_PRESSED  0
#define BUTTON_RELEASED 1
#define BUTTON_LOCKED   2

#define KB_STATE_NORMAL 0 
#define KB_STATE_SHIFT  (1<<1)
#define KB_STATE_MOD    (1<<2)
#define KB_STATE_CTRL   (1<<3)
#define KB_STATE_CAPS   (1<<4)
#define KB_STATE_META   (1<<5)
#define KB_STATE_ALT    (1<<6)

#define BUT_NORMAL      0
#define BUT_SHIFT       (1<<1)
#define BUT_MOD         (1<<2)
#define BUT_CTRL        (1<<3)
#define BUT_CAPS        (1<<4)
#define BUT_META        (1<<5)
#define BUT_ALT         (1<<6)

#define OPT_NORMAL      0
#define OPT_OBEYCAPS    (1<<0)

#define TRUE            1
#define FALSE           0

#define UP              1
#define DOWN            2
#define LEFT            3
#define RIGHT           4

#define MAX_LAYOUTS     3

#define KB_STATE_ALT_L (1<<1)
#define KB_STATE_ALT_R (1<<2)
#define KB_STATE_CTRL_L (1<<3)
#define KB_STATE_CTRL_R (1<<4)
#define KB_STATE_SHIFT_L (1<<5)
#define KB_STATE_SHIFT_R (1<<6)

typedef struct _list
{
  struct _list *next;
  int type;
  void *data;

} list;


typedef struct _box
{
  enum { vbox, hbox } type;
  list *root_kid;
  list *tail_kid;
  int min_width;        /* ( num_kids*(kid_c_width+(kid_border*2) ) */
  int min_height; 
  int act_width;        /* actual calculated width */
  int act_height;       /* ( num_kids*(kid_c_width+padding+(kid_border*2) ) */
  int x;                /* relative to parent ? */
  int y;

  struct _box *parent;  /* pointer to parent keyboard */

} box;

typedef struct _keyboard 
{
  int mode;
  box *kbd_layouts[MAX_LAYOUTS];
  int total_layouts;
  box *vbox;  /* container */

  int x;      /* but vbox contains this ? */
  int y;
  Window win;
  Display *display;
  Pixmap backing;

  GC gc;
  GC rev_gc;   /* inverse gc of above */
  GC txt_gc;   /* gc's for button txt */
  GC txt_rev_gc;
  GC bdr_gc;
  
  XFontStruct* font_info;
  int state;  /* shifted | caps | modded | normal */
  int state_locked;  /* shifted | modded | normal */
  
  enum { oldskool, xft } render_type;
  enum { rounded, square, plain } theme;
  
  int slide_margin;
  int key_delay_repeat; /* delay time before key repeat */
  int key_repeat;       /* delay time between key repeats */
  
#ifdef USE_XFT
  XftDraw *xftdraw;   /* xft aa bits */
  XftFont *xftfont;
  XftColor color_bg; 
  XftColor color_fg;
#endif

  int bthid_state;
	
} keyboard;

typedef struct _button
{
  int x;             /* actual co-ords relative to window */
  int y;
  
  char *default_txt; /* default button txt */
  KeySym default_ks; /* default button Xkeysym */
  char *shift_txt;
  KeySym shift_ks;
  char *mod_txt;
  KeySym mod_ks;

  KeySym slide_up_ks;
  KeySym slide_down_ks;
  KeySym slide_left_ks;
  KeySym slide_right_ks;

  enum { none, up, down, left, right } slide;

  int modifier; /* set to BUT_ if key is shift,ctrl,caps etc */

  int options; /* bit-field of OPT_* */

  int c_width;  /* width  of contents ( min width ) */
  int c_height; /* height of contents ( min height ) */
  int x_pad;    /* total padding horiz */
  int y_pad;    /* total padding vert  */
  int b_size;   /* size of border in pixels */
                /* eg. total width = c_width+pad_x+(2*b_size) */ 

  Bool is_width_spec; /* width is specified in conf file */ 
  int key_span_width; /* width in number of keys spanned */
   
  int act_width; 
  int act_height;

  keyboard *kb;   /* pointer to parent keyboard */
  box   *parent;  /* pointer to holding box */

  GC fg_gc;       /* gc's for 'general' button cols */
  GC bg_gc;

  signed int layout_switch; /* Signals the button switches layout 
			       set to -1 for no switch            */

#ifdef USE_XFT
  XftColor *xft_fg_col;  /* xft */ 
  XftColor *xft_bg_col;
#endif

  Pixmap *pixmap;
  Pixmap *mask;
  GC mask_gc;

  int scancode;
	
} button;


#endif






