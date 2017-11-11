#include "buttonEvents.h"
#include "reparent.h"

// variables from main.c
extern Display *d;
extern WMClient *clientHead;

// the initial position of the mouse on a movement event
int startMouseX = 0;
int startMouseY = 0;

// the initial position of the window on a movement event
int startWinX = 0;
int startWinY = 0;

// the initial size of the window on a resize event
int startWinWidth = 0;
int startWinHeight = 0;

Bool hButtonPress(const XButtonEvent e)
{
    printf("Button Press Event!\n");
    
    // Save the current mouse position to check for difference while moving
    startMouseX = e.x_root;
    startMouseY = e.y_root;
    
    // Save current window information
    
    WMClient *temp = clientHead;
    while(temp != NULL)
    {
        // either the frame or child window is clicked
        //if(temp->child == e.window) break;
        if(temp->child    == e.window ||
           temp->titleBar == e.window) break;
        temp = temp->next;
    }
    if(temp == NULL){
        printf("hButtonPress: failed to find child or frame within clients!\n");
        return False;
    }
    
    Window r;
    int x, y;
    unsigned int w, h, bWidth, depth;
    XGetGeometry(
        d,
        temp->frame,
        &r,
        &x, &y,
        &w, &h,
        &bWidth,
        &depth
    );
    
    startWinX = x;
    startWinY = y;
    
    startWinWidth = w;
    startWinHeight = h;
    
    XRaiseWindow(d, temp->frame);
    
    return True;
}

Bool hButtonRelease(const XButtonEvent e)
{
    printf("Button Release Event!\n");
    
    return True;
}
