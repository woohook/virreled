#define REALN float
/*float or double*/

#define SCREENWIDTH 640
/*screen width (pixels)*/

#define SCREENHEIGHT 480
/*screen height (pixels)*/

#define SDLSCREENFLAG SDL_SWSURFACE
/*SDL_FULLSCREEN or SDL_SWSURFACE*/

#define CLBITS 32
/*number of colour bits; can be 16, 24 or 32*/

#define DOUBLEPIX 0
/*1 - large pixels; 0 - normal pixels*/

#define FOV 90
/*horizontal viewing angle (in degrees)*/

#define ASPCOR 0
/*1 - aspect correction; 0 - no aspect correction (WIDTHFACTOR ignored)*/

#define WIDTHFACTOR 0.78125
/*0.78125 recommended for monitor with 1024x600 resolution, set at 800x600
(if the video driver doesn't support 1024x600)*/
