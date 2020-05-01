#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <stdio.h>
#include <stdlib.h>

// compile: g++ screen_dimension.cpp -lX11 -lXrandr

static double mode_refresh(const XRRModeInfo *mode_info)
{
	double rate;
	double vTotal = mode_info->vTotal;

	/* double-scan implies twice the number of vertical lines. */
	if (mode_info->modeFlags & RR_DoubleScan) {
		vTotal *= 2;
	}

	/* interlace splits the frame into two fields */
	/* the field rate is what is typically reported by monitors */
	if (mode_info->modeFlags & RR_Interlace) {
		vTotal /= 2;
	}

	if (mode_info->hTotal && vTotal)
		rate = ((double) mode_info->dotClock /
				((double) mode_info->hTotal * (double) vTotal));
	else
		rate = 0;
	return rate;
}

static XRRModeInfo *XRRGetModeInfo(XRRScreenResources *res, RRMode mode)
{
	for(int i = 0; i < res->nmode; i++) {
		if (res->modes[i].id == mode)
			return &(res->modes[i]);
	}
	return NULL;
}

int main()
{
	Display *dpy;
	Window root;
	XRRScreenResources *res;

	dpy = XOpenDisplay(NULL);
	if (dpy == NULL)
	{
		fprintf(stderr, "cannot connect\n");
		return 1;
	}
	root = DefaultRootWindow(dpy);
	res = XRRGetScreenResourcesCurrent(dpy, root);

	//first, let's find those pesky dimensions per output.
	for (int i = 0; i < res->noutput; i++) {
		XRROutputInfo *output_info = XRRGetOutputInfo(dpy, res, res->outputs[i]);
		if (output_info->connection || !output_info->crtc)
			continue;
		XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(dpy, res, output_info->crtc);
		XRRModeInfo *mode_info = XRRGetModeInfo(res, crtc_info->mode);

		printf("%dx%d @ %.2fHz, ", crtc_info->width, crtc_info->height, mode_refresh(mode_info)); 
	}
	return 0;
}
