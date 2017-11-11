/* reframes given windows */

#include "reparent.h"

// variables from main.c
extern Display *d;
extern WMClient *clientHead; // the head of the WMClient linked list

Bool reparentWindow(Window child, Bool before_wm)
{
    WMClient *c = NULL; // our current client to work on
    
    // Get the name of the window
    char *childName = NULL;
    if(XFetchName(d, child, &childName)) {
        printf("Reparenting Window! Name: %s\n", childName);
    }
    if(childName) XFree(childName);

    /* create a new entry in the list */
    if(clientHead == NULL) {
        clientHead = (WMClient *)malloc(sizeof(WMClient));
        /* TODO - test for NULL */

        c = clientHead;
    }
    else {
        /* get the last entry in the client list */
        WMClient *temp = clientHead;
        while(temp->next != NULL)
            temp = temp->next;

        c = (WMClient *)malloc(sizeof(WMClient));
        /* TODO - test for NULL */

        /* assign the new client structure to the end of the list */
        temp->next = c;
    }
    c->next  = NULL;
    c->child = child;
    c->maximized = False;
    c->minimized = False;
    c->w = c->h = c->x = c->y=0; // initialize the position variables
    
    // Get window info
    XWindowAttributes childWinInfo;
    XGetWindowAttributes(d, child, &childWinInfo);
    
    // if the window was created before the wm started,
    // frame only if visible and doesn't set override redirect
    if(before_wm) {
        if(childWinInfo.override_redirect ||
           childWinInfo.map_state != IsViewable) {
            printf("Before WM & override_redirect OR mapstate != IsViewable!\n");
            return True;
        }
    }
    
    // create the frame
    c->frame = XCreateSimpleWindow(
        d,
        RootWindow(d, DefaultScreen(d)),
        childWinInfo.x,
        childWinInfo.y,
        childWinInfo.width,
        childWinInfo.height,
        BORDER_WIDTH,
        0xFF0000,      // TODO - change this to the read in border color
        0x000000       // TODO - change to frame bg color
    );
    
    // map the child window to our client
    c->child = child;
    
    // select input on the frame
    XSelectInput(
        d,
        c->frame,
        SubstructureNotifyMask | SubstructureRedirectMask
    );
    
    // add the child to save set to restore if we crash
    XAddToSaveSet(d, child);
    
    // reparent the child window to our frame
    XReparentWindow(
        d,
        child,
        c->frame,
        0, 0
    );
    
    // display the frame
    XMapWindow(d, c->frame);
    
    // Grab buttons on the child window
    //   a. Move windows with alt + left button.
    XGrabButton(
        d,
        Button1,
        Mod1Mask,
        child,
        False,
        ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
        GrabModeAsync,
        GrabModeAsync,
        None,
        None);
    //   b. Resize windows with alt + right button.
    XGrabButton(
        d,
        Button3,
        Mod1Mask,
        child,
        False,
        ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
        GrabModeAsync,
        GrabModeAsync,
        None,
        None);
    //   c. Kill windows with alt + f4.
    XGrabKey(
        d,
        XKeysymToKeycode(d, XK_F4),
        Mod1Mask,
        child,
        False,
        GrabModeAsync,
        GrabModeAsync);
    //   d. Switch windows with alt + tab.
    XGrabKey(
        d,
        XKeysymToKeycode(d, XK_Tab),
        Mod1Mask,
        child,
        False,
        GrabModeAsync,
        GrabModeAsync);
    
    return True;
}

Bool unparentWindow(Window child)
{
    // Find the client of the corresponding child window
    WMClient *temp = clientHead;
    while(temp != NULL){
        if(temp->child == child)
            break;
        temp = temp->next;
    }
    if(temp == NULL) {
        printf("unparentwindow: failed to find child window!\n");
        return False;
    }
    
    // Unmap the frame
    XUnmapWindow(d, temp->frame);
    
    // reparent the client window to the root window
    XReparentWindow(
        d,
        child,
        RootWindow(d, DefaultScreen(d)),
        0,0
    );
    
    // remove the app window from the save set since we aren't reparenting it
    XRemoveFromSaveSet(d, child);
    
    // delete the client and all of its subwindows
    deleteClient(child);
    
    return True;
}

Bool deleteClient(Window child)
{
    WMClient *temp = clientHead;
    WMClient *head = temp;
    WMClient *caboose = NULL;
    if(temp!= NULL) caboose = temp->next;
    
    /* Make temp equal to the client with the sent child */
    while(temp != NULL){

        //if(temp->frame == parent) break;
        if(temp->child == child) break;
        /* keep track of the entry before and after temp */
        head = temp;
        temp = temp->next;
        if(temp != NULL) caboose = temp->next;
        else             caboose = NULL;
    }
    
    /* If we found a matching client with the given child */
    if(temp != NULL){
        /* Destroy the frame of the window (its parent) 
         * and free memory */
        printf("Before destroy subwindows!\n");
        XDestroySubwindows(d, temp->frame);
        
        printf("Before destroy frame!\n");
        XDestroyWindow(d, temp->frame);
        printf("After destroy frame!\n");
    
        /* reconnect the previous and after
         * WMclients in the list */

        if(temp != clientHead) head->next = caboose;
        else                   clientHead = caboose;

        free(temp);

    }
    /* We didn't find the child, send error */
    else{
        printf("deleteClient: Failed to find Client with sent child!\n");
    }
    
    return True;
}
