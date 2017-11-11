/* Testing example manager rewrite in c */
#include <X11/Xlib.h>

#include <stdio.h>

#include "reparent.h"
#include "displayEvents.h"
#include "buttonEvents.h"
#include "configureEvents.h"
#include "destroyEvents.h"
#include "displayEvents.h"
#include "keyEvents.h"
#include "motionEvents.h"

#include "initCapstone.h"

/***************************************/
/**         GLOBAL VARIABLES          **/
/***************************************/
Display *d; // connection to the X display
WMClient *clientHead; // the head of the WMClient linked list

/* location of the programs list for the program menu,
 * set by parseRC in initCapstone.c */
char *menuFilePath = NULL;

/***************************************/
/**        FUNCTION PROTOTYPES        **/
/***************************************/
int errorHandler(Display *d, XErrorEvent *e);
Bool initX(void);
Bool reparentExistingWindows(void);
Bool mainLoop(void);


int main(int argc, char **argv)
{
    // parse the rc file
    if(argc != 2){
        printf("No rc file given! Using default location\n");
        if(!parseRC("files/capstoneInit.rc")){
            printf("Failed to load file!\n");
        }
        else{
            printf("Loaded file!\n");
        }
    }
    else{
        printf("Using argument file!\n");
        parseRC(argv[1]);
    }
    
    // init X
    if(!initX()) return -1;
    
    // load pixmaps for the window borders
    reparentLoadPixmaps("files/minimize.xpm",
                        "files/maximize.xpm",
                        "files/unmaximize.xpm",
                        "files/close.xpm");
    
    // frame any programs running before the 
    // window manager started
    reparentExistingWindows();
    
    // check for and handle events
    mainLoop();
    
    reparentClosePixmaps();
    if(d) XCloseDisplay(d);
    return 0;
}

/***************************************/
/**        FUNCTION DEFINITIONS       **/
/***************************************/
int errorHandler(Display *d, XErrorEvent *e)
{
    
    if(e->error_code == BadAccess) {
        fprintf(stderr, "Another Window Manager Already Detected!\n");
        return -1;
    }
    
    return 0;
}

Bool initX(void)
{
    // open the display
    d = XOpenDisplay(NULL);
    if(!d){
        fprintf(stderr, "initX: Failed to open display!\n");
        return False;
    }
    
    // set error handler
    XSetErrorHandler(&errorHandler);
    
    // we want to be able to reparent windows
    XSelectInput(
        d,
        DefaultRootWindow(d),
        SubstructureRedirectMask | SubstructureNotifyMask
    );
    
    // flushes outpu buffer, waits until all requests have
    // been received and processed
    XSync(d, False);
    
    return True;
}

Bool reparentExistingWindows(void)
{
    // Prevent windows from changing under us
    XGrabServer(d);
    
    // Get existing top level windows
    Window r, p, *children; // root, parent, children
    unsigned int numChildren;
    XQueryTree(
        d, 
        DefaultRootWindow(d),
        &r,
        &p,
        &children,
        &numChildren
    );
    
    // reframe each top level window
    unsigned int i;
    for(i=0; i<numChildren; i++)
    {
        reparentWindow(children[i], True);
    }
    
    // free allocated memory
    if(children) XFree(children);
    
    // finished chokeholding the server
    XUngrabServer(d);
    
    return True;
}

Bool mainLoop(void)
{
    XEvent e;
    
    while(True)
    {
        XNextEvent(d, &e);
        
        switch(e.type)
        {
            case CreateNotify:    hCreateNotify(e.xcreatewindow);   break;
            case DestroyNotify:   hDestroyNotify(e.xdestroywindow); break;
            case ReparentNotify:  hReparentNotify(e.xreparent);     break;
            case MapNotify:       hMapNotify(e.xmap);               break;
            case UnmapNotify:     hUnmapNotify(e.xunmap);           break;
            case ConfigureNotify: hConfigureNotify(e.xconfigure);   break;
            case MapRequest:      hMapRequest(e.xmaprequest);       break;
            case ConfigureRequest: hConfigureRequest(e.xconfigurerequest); break;
            case ButtonPress:      hButtonPress(e.xbutton);         break;
            case ButtonRelease:    hButtonRelease(e.xbutton);       break;
            case MotionNotify:     hMotionNotify(e.xmotion);        break;
            case KeyPress:         hKeyPress(e.xkey);               break;
            case KeyRelease:       hKeyRelease(e.xkey);             break;
            
            default:
                printf("Unhandled Event: %d\n", e.type);
                break;
        }
    }
    
    return True;
}