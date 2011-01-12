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

#include <stdio.h>
#include <stdlib.h>

#include "structs.h"

#define WIDGET_BOX    0
#define WIDGET_BUTTON 1

box* box_new(void)
{
  box *bx = NULL;
  bx = malloc(sizeof(box));
  bx->root_kid = NULL;
  bx->tail_kid = NULL;
  bx->act_width = 0;
  bx->act_height = 0;
  bx->parent = NULL;
  return bx;
}

button *box_add_button(box *bx, button *but)
{
  list *new_ptr  = NULL;

  but->parent = bx; /* set its parent */

  if (bx->root_kid == NULL) /* new list */
    {
      bx->root_kid = (list *)malloc(sizeof(list));
      bx->root_kid->next = NULL;
      bx->tail_kid = bx->root_kid;
      bx->root_kid->data = but;
      bx->root_kid->type = WIDGET_BUTTON;
      
      return but;
    } 

  new_ptr = bx->tail_kid;
  new_ptr->next = malloc(sizeof(list));
  new_ptr->next->next = NULL;
  new_ptr->next->data = but;
  new_ptr->next->type = WIDGET_BUTTON;
  bx->tail_kid = new_ptr->next;
  
  return but;

}

box *box_add_box(box *bx, box *b)
{
  list *new_ptr  = NULL;

  b->parent = bx; /* set its parent */

  if (bx->root_kid == NULL) /* new list */
    {
      bx->root_kid = (list *)malloc(sizeof(list));
      bx->root_kid->next = NULL;
      bx->tail_kid = bx->root_kid;
      bx->root_kid->data = b;
      bx->root_kid->type = WIDGET_BOX;
      return b;
    } 

  new_ptr = bx->tail_kid;
  new_ptr->next = malloc(sizeof(list));
  new_ptr->next->next = NULL;
  new_ptr->next->data = b;
  new_ptr->next->type = WIDGET_BOX;
  bx->tail_kid = new_ptr->next;
  return b;

}

void box_list_contents(box *bx)
{
  list *ptr = bx->root_kid;
  if (ptr == NULL) return;
  while (ptr != NULL)
    {
      /*      if (ptr->type == WIDGET_BUTTON)
	      printf("listing %s\n", ((button *)ptr->data)->txt); */
      printf("size is %i\n", sizeof(*(ptr->data)));
      ptr = ptr->next;
    }
}



