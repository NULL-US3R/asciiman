#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/keysym.h>

int main(){
    Display * dpy = XOpenDisplay(0);
    int scr = XDefaultScreen(dpy);
    Window root_w = XRootWindow(dpy, scr);
    Window main_w = XCreateSimpleWindow(dpy, root_w, 0, 0, 600, 600, 0, 0, 0);

    XSelectInput(dpy, main_w, ExposureMask | KeyPressMask | KeyReleaseMask);

    Colormap cmap = XDefaultColormap(dpy, scr);
    XColor col;
    col.red = 0xff00;
    col.green = 0xff00;
    col.blue = 0x0;
    col.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(dpy, cmap, &col);
    GC gc = XCreateGC(dpy, main_w, 0, NULL);
    XSetForeground(dpy, gc, col.pixel);
    XMapWindow(dpy, main_w);

    XEvent ev;
    int x=0,y=0,vx=0,vy=0;
    KeySym ks;
    while(1){
        while(XPending(dpy)){
            XNextEvent(dpy, &ev);
            switch(ev.type){
                case KeyPress:
                ks = XLookupKeysym(&ev.xkey, 0);
                printf("%lu\n",ks);
                switch(ks){
                    case XK_w:
                    vy=-2;
                    break;
                    case XK_s:
                    vy=2;
                    break;
                    case XK_a:
                    vx=-2;
                    break;
                    case XK_d:
                    vx=2;
                    break;
                }
                break;
                case KeyRelease:
                ks = XLookupKeysym(&ev.xkey, 0);
                switch(ks){
                    case XK_w:
                    case XK_s:
                    vy=0;
                    break;
                    case XK_a:
                    case XK_d:
                    vx=0;
                    break;
                }
                printf("x: %i y: %i\n",x,y);
                break;
            }
        }
        x+=vx,y+=vy;
        XClearWindow(dpy, main_w);
        XFillRectangle(dpy, main_w, gc, x, y, 20, 20);
        XFlush(dpy);
        usleep(1e4);
    }
    return 0;
}
