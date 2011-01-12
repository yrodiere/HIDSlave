#include "libXkbd.h"

Xkbd*
xkbd_realize(Display *display,
	     Drawable dest,
	     char *conf_file,
	     char *font_name,
	     int x,
	     int y,
	     int width,
	     int height,
	     int flags)
{
   Xkbd *xkbd; 
   xkbd = malloc(sizeof(Xkbd));
   
   xkbd->kb = kb_new(dest, display, x, y, 
		     width, height, conf_file,
		     font_name, flags  );
   xkbd->active_but = NULL;
   kb_size(xkbd->kb);
   return xkbd;
}

void
xkbd_resize(Xkbd *xkbd, int width, int height)
{
   xkbd->kb->vbox->act_width = width; 
   xkbd->kb->vbox->act_height = height;
   kb_size(xkbd->kb);
   kb_render(xkbd->kb);
   kb_paint(xkbd->kb);
}

void
xkbd_move(Xkbd *kb, int x, int y)
{
   ;
}

void
xkbd_repaint(Xkbd *xkbd)
{
   kb_size(xkbd->kb);
   kb_render(xkbd->kb);
   kb_paint(xkbd->kb);
}

void
xkbd_process(Xkbd *xkbd, XEvent *an_event)
{
   xkbd->active_but = kb_handle_events(xkbd->kb, *an_event);
}

void xkbd_process_repeats(Xkbd *xkbd)
{
   kb_do_repeat(xkbd->kb, xkbd->active_but);
}

int xkbd_get_width(Xkbd *xkbd)
{
   return xkbd->kb->vbox->act_width; 
}
     
int xkbd_get_height(Xkbd *xkbd)
{
   return xkbd->kb->vbox->act_height;
}

void
xkbd_destroy(Xkbd *kb)
{
   ;

}


