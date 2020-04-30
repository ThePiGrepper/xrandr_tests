#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <stdio.h>
#include <stdlib.h>

// compile: g++ screen_dimension.cpp -lX11 -lXrandr
static double mode_refresh(const XRRModeInfo *mode_info)
{
	double rate;
	double vTotal = mode_info->vTotal;

	if (mode_info->modeFlags & RR_DoubleScan) {
/* doublescan doubles the number of lines */
	vTotal *= 2;
	}

	if (mode_info->modeFlags & RR_Interlace) {
/* interlace splits the frame into two fields */
/* the field rate is what is typically reported by monitors */
	vTotal /= 2;
	}

	if (mode_info->hTotal && vTotal)
		rate = ((double) mode_info->dotClock /
		((double) mode_info->hTotal * (double) vTotal));
	else
		rate = 0;
	return rate;
}

int main()
{
	int wid = atoi( getenv( "WINDOWID" ) );

	printf("window id: %i\n", wid);

	Display * dpy = XOpenDisplay(NULL);
	int screen  = DefaultScreen(dpy);
	Window root = DefaultRootWindow(dpy);

	XRRScreenResources * res = XRRGetScreenResourcesCurrent(dpy, root);

	 printf("modes in res:%d\n", res->nmode);
	for (int i = 0; i < res->nmode; i++)
	{
		XRRModeInfo mode_info = res->modes[i];
		printf("mode[%d] in resources:\n"
				"xid: %ld, width: %d, height: %d\n"
				"dotClock: %ld, hSyncStart: %d, hSyncEnd: %d\n"
				"hTotal: %d, hSkew: %d, vSyncStart: %d\n"
				"vSyncEnd: %d, vTotal: %d, nameLength: %d\n"
				"Flags: 0x%lx\n",
				i,
				mode_info.id,
				mode_info.width,
				mode_info.height,
				mode_info.dotClock,
				mode_info.hSyncStart,
				mode_info.hSyncEnd,
				mode_info.hTotal,
				mode_info.hSkew,
				mode_info.vSyncStart,
				mode_info.vSyncEnd,
				mode_info.vTotal,
				mode_info.nameLength,
				mode_info.modeFlags);
		printf("name: ");
		fwrite(mode_info.name, mode_info.nameLength, 1, stdout);
		printf("\n===================\n");
	}

	 printf("outputs in res:%d\n", res->noutput);
  for (int i = 0; i < res->noutput; i++)
  {
			XRROutputInfo * output_info = XRRGetOutputInfo (dpy, res, res->outputs[i]);
			if( output_info->connection ) continue; // No connection no crtcs
				printf(" (%lu %lu) mm Name: %s connection: %i ncrtc: %i \n",
						output_info->mm_width,
						output_info->mm_height,
						output_info->name,
						output_info->connection,
						output_info->ncrtc);

				for (int t = 0; t < output_info->ncrtc ; t++)
					printf("crtc: #%lu\n", output_info->crtcs[t]);

				for (int t = 0; t < output_info->nmode ; t++)
					printf("mode: #%lu\n", output_info->modes[t]);

				printf("preferred modes:%d\n",output_info->npreferred);
  }

	printf("crtcs in res:%d\n", res->ncrtc);
	for( int j = 0; j < res->ncrtc; j++ ) {
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
