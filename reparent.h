/* reframes given windows */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef REPARENT_H_INCLUDED
#define REPARENT_H_INCLUDED

#define BORDER_WIDTH 2  // border size of the parent window
#define TITLE_HEIGHT 20 // size of the title bar

#define BUTTON_SIZE  TITLE_HEIGHT // the width and height of max/min/close buttons

// groups frame windows and the actual app windows
typedef struct WMClient
{
    Window frame;    // the border/container window
    Window titleBar; // the title bar to click/drag
    Window minWin; // minimize, maximize, and close subwindows
    Window maxWin;
    Window closeWin;
    Window child;  // the actual application content window

    unsigned int x; // the x and y position before maximizing the window
    unsigned int y;
    unsigned int w; // the width and height before maximizing the window
    unsigned int h;
    
    Bool maximized; // true if the window is fullscreen
    Bool minimized; // true if the window is minimized

    struct WMClient *next;
} WMClient;

Bool reparentWindow(Window child, Bool before_wm);
Bool unparentWindow(Window child);
Bool deleteClient(Window child);

#endif
