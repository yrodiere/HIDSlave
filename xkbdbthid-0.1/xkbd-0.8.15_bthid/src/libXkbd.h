#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>

#include "structs.h"
#include "kb.h"
#include "button.h"
#include "box.h"

#define XFT_FONT 1 

typedef struct _xkbd
{
   struct _keyboard *kb;
   struct _button    *active_but;
   int i;
   
} Xkbd;

Xkbd* xkbd_realize(Display *display,
		   Drawable dest,
		   char *conf_file,
		   char *font_name,
		   int x,
		   int y,
		   int width,
		   int height,
		   int flags);
void xkbd_resize(Xkbd *xkbd, int width, int height);
void xkbd_move(Xkbd *kb, int x, int y);
void xkbd_repaint(Xkbd *xkbd);
void xkbd_process(Xkbd *xkbd, XEvent *an_event);
void xkbd_process_repeats(Xkbd *xkbd);
int xkbd_get_width(Xkbd *xkbd);
int xkbd_get_height(Xkbd *xkbd);
void xkbd_destroy(Xkbd *xkbd);
