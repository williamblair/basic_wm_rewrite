/* For when windows are drawn to the screen */

#include "displayEvents.h"

// variables from main.c
extern Display *d;

Bool hCreateNotify(const XCreateWindowEvent e)
{
    printf("Create Notify Event!\n");
    
    return True;
}

Bool hReparentNotify(const XReparentEvent e)
{
    printf("Reparent Notify Event!\n");
    
    return True;
}

Bool hMapNotify(const XMapEvent e)
{
    printf("Map Notify Event!\n");
    
    return True;
}

Bool hMapRequest(const XMapRequestEvent e)
{
    printf("Map Request Event!\n");
    
    reparentWindow(e.window, False);
    
    XMapWindow(d, e.window);
    
    return True;
}
