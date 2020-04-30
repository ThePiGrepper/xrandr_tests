#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <stdio.h>
#include <stdlib.h>

// compile: g++ screen_dimension.cpp -lX11 -lXrandr

int main()
{
  int wid = atoi( getenv( "WINDOWID" ) );

  printf("window id: %i\n", wid);

  Display * dpy = XOpenDisplay(NULL);
  int screen  = DefaultScreen(dpy);
  Window root = DefaultRootWindow(dpy);

  XRRScreenResources * res = XRRGetScreenResourcesCurrent(dpy, root);

  XRROutputInfo * output_info;

  for (int i = 0; i < res->noutput; i++)
  {
      output_info = XRRGetOutputInfo (dpy, res, res->outputs[i]);

      if( output_info->connection ) continue; // No connection no crtcs
          printf(" (%lu %lu) mm Name: %s connection: %i ncrtc: %i \n",  output_info->mm_width
            , output_info->mm_height
            , output_info->name
            , output_info->connection
            , output_info->ncrtc
          );
  }

   printf("crtcs:\n");
  for( int j = 0; j < output_info->ncrtc; j++ ) {
    XRRCrtcInfo * crtc_info = XRRGetCrtcInfo( dpy, res, res->crtcs[ j ] );
    if( ! crtc_info->noutput ) continue;
    printf("%i w: %5i   h: %5i     x: %5i    y: %i\n", j
    , crtc_info->width
    , crtc_info->height
    , crtc_info->x
    , crtc_info->y
    );
  }
}
