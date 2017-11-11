#include "motionEvents.h"
#include "reparent.h"

// variables from main.c
extern Display *d;
extern WMClient *clientHead;

// declared in buttonEvents.c
// the initial position of the mouse on a movement event
extern int startMouseX;
extern int startMouseY;

// the initial position of the window on a movement event
extern int startWinX;
extern int startWinY;

// the initial size of the window on a resize event
extern int startWinWidth;
extern int startWinHeight;

Bool hMotionNotify(const XMotionEvent e)
{
    /* Where the end position of the mouse is */
    int dragPosX = e.x_root;
    int dragPosY = e.y_root;
    
    int dX = dragPosX - startMouseX;
    int dY = dragPosY - startMouseY;
    
    // find the client frame
    WMClient *temp = clientHead;
    while(temp != NULL)
    {
        //if(temp->child == e.window) break;
        if(temp->child == e.window ||
           temp->titleBar == e.window) break;
        temp = temp->next;
    }
    if(temp == NULL){
        printf("hMotionNotify: Failed to find child within client windows!\n");
        return False;
    }
    
    // left mouse button so movement
    if(e.state & Button1Mask)
    {
        int newPosX = startWinX + dX;
        int newPosY = startWinY + dY;
        
        XMoveWindow(
            d,
            temp->frame,
            newPosX,
            newPosY
        );
    }
    
    // right mouse button so resize
    else if(e.state & Button3Mask) 
    {
        int newWidth = startWinWidth + dX;
        int newHeight = startWinHeight + dY;
        
        XResizeWindow(
            d,
            (e.window) ? e.window : e.subwindow, 
            (newWidth >= 10) ? newWidth : 10,
            (newHeight >=10) ? newHeight : 10
        );
        
        XResizeWindow(
            d,
            temp->frame,
            (newWidth >= 10) ? newWidth : 10,
            (newHeight >= 10) ? newHeight: 10
        );
        
        // resize the titlebar
        XWindowAttributes tAttribs;
        XGetWindowAttributes(d, temp->titleBar, &tAttribs);
        XResizeWindow(
            d,
            temp->titleBar,
            (newWidth >= 10) ? newWidth : 10,
            tAttribs.height
        );
    }
    
    return True;
}