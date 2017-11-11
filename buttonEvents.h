#include <X11/Xlib.h>
#include <stdio.h>

#ifndef BUTTON_EVENTS_H_INCLUDED
#define BUTTON_EVENTS_H_INCLUDED

Bool hButtonPress(const XButtonEvent e);
Bool hButtonRelease(const XButtonEvent e);

#endif
