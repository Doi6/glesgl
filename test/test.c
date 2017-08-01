#include <EGL/egl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>

void die( const char * fmt, ... ) {
   va_list args;
   va_start( args, fmt );
   vfprintf( stderr, fmt, args );
   va_end( args );
   exit(1);
}

void debug( const char * fmt, ... ) {
   va_list args;
   va_start( args, fmt );
   vfprintf( stderr, fmt, args );
   va_end( args );
}

void debugconfig( EGLDisplay * edpy, EGLConfig * ec ) {
   EGLint atis [] = {
EGL_ALPHA_SIZE,
EGL_ALPHA_MASK_SIZE,
EGL_BIND_TO_TEXTURE_RGB,
EGL_BIND_TO_TEXTURE_RGBA,
EGL_BLUE_SIZE,
EGL_BUFFER_SIZE,
EGL_COLOR_BUFFER_TYPE,
EGL_CONFIG_CAVEAT,
EGL_CONFIG_ID,
EGL_CONFORMANT,
EGL_DEPTH_SIZE,
EGL_GREEN_SIZE,
EGL_LEVEL,
EGL_LUMINANCE_SIZE,
EGL_MAX_PBUFFER_WIDTH,
EGL_MAX_PBUFFER_HEIGHT,
EGL_MAX_PBUFFER_PIXELS,
EGL_MAX_SWAP_INTERVAL,
EGL_MIN_SWAP_INTERVAL,
EGL_NATIVE_RENDERABLE,
EGL_NATIVE_VISUAL_ID,
EGL_NATIVE_VISUAL_TYPE,
EGL_RED_SIZE,
EGL_RENDERABLE_TYPE,
EGL_SAMPLE_BUFFERS,
EGL_SAMPLES,
EGL_STENCIL_SIZE,
EGL_SURFACE_TYPE,
EGL_TRANSPARENT_TYPE,
EGL_TRANSPARENT_RED_VALUE,
EGL_TRANSPARENT_GREEN_VALUE,
EGL_TRANSPARENT_BLUE_VALUE,
EGL_ALPHA_MASK_SIZE,
EGL_BIND_TO_TEXTURE_RGB,
EGL_BIND_TO_TEXTURE_RGBA,
EGL_BLUE_SIZE,
EGL_BUFFER_SIZE,
EGL_COLOR_BUFFER_TYPE,
EGL_CONFIG_CAVEAT,
EGL_CONFIG_ID,
EGL_CONFORMANT,
EGL_DEPTH_SIZE,
EGL_GREEN_SIZE,
EGL_LEVEL,
EGL_LUMINANCE_SIZE,
EGL_MAX_PBUFFER_WIDTH,
EGL_MAX_PBUFFER_HEIGHT,
EGL_MAX_PBUFFER_PIXELS,
EGL_MAX_SWAP_INTERVAL,
EGL_MIN_SWAP_INTERVAL,
EGL_NATIVE_RENDERABLE,
EGL_NATIVE_VISUAL_ID,
EGL_NATIVE_VISUAL_TYPE,
EGL_RED_SIZE,
EGL_RENDERABLE_TYPE,
EGL_SAMPLE_BUFFERS,
EGL_SAMPLES,
EGL_STENCIL_SIZE,
EGL_SURFACE_TYPE,
EGL_TRANSPARENT_TYPE,
EGL_TRANSPARENT_RED_VALUE,
EGL_TRANSPARENT_GREEN_VALUE,
EGL_TRANSPARENT_BLUE_VALUE
};


   char * (atns []) = {
"ALPHA_SIZE",
"ALPHA_MASK_SIZE",
"BIND_TO_TEXTURE_RGB",
"BIND_TO_TEXTURE_RGBA",
"BLUE_SIZE",
"BUFFER_SIZE",
"COLOR_BUFFER_TYPE",
"CONFIG_CAVEAT",
"CONFIG_ID",
"CONFORMANT",
"DEPTH_SIZE",
"GREEN_SIZE",
"LEVEL",
"LUMINANCE_SIZE",
"MAX_PBUFFER_WIDTH",
"MAX_PBUFFER_HEIGHT",
"MAX_PBUFFER_PIXELS",
"MAX_SWAP_INTERVAL",
"MIN_SWAP_INTERVAL",
"NATIVE_RENDERABLE",
"NATIVE_VISUAL_ID",
"NATIVE_VISUAL_TYPE",
"RED_SIZE",
"RENDERABLE_TYPE",
"SAMPLE_BUFFERS",
"SAMPLES",
"STENCIL_SIZE",
"SURFACE_TYPE",
"TRANSPARENT_TYPE",
"TRANSPARENT_RED_VALUE",
"TRANSPARENT_GREEN_VALUE",
"TRANSPARENT_BLUE_VALUE",
"ALPHA_MASK_SIZE",
"BIND_TO_TEXTURE_RGB",
"BIND_TO_TEXTURE_RGBA",
"BLUE_SIZE",
"BUFFER_SIZE",
"COLOR_BUFFER_TYPE",
"CONFIG_CAVEAT",
"CONFIG_ID",
"CONFORMANT",
"DEPTH_SIZE",
"GREEN_SIZE",
"LEVEL",
"LUMINANCE_SIZE",
"MAX_PBUFFER_WIDTH",
"MAX_PBUFFER_HEIGHT",
"MAX_PBUFFER_PIXELS",
"MAX_SWAP_INTERVAL",
"MIN_SWAP_INTERVAL",
"NATIVE_RENDERABLE",
"NATIVE_VISUAL_ID",
"NATIVE_VISUAL_TYPE",
"RED_SIZE",
"RENDERABLE_TYPE",
"SAMPLE_BUFFERS",
"SAMPLES",
"STENCIL_SIZE",
"SURFACE_TYPE",
"TRANSPARENT_TYPE",
"TRANSPARENT_RED_VALUE",
"TRANSPARENT_GREEN_VALUE",
"TRANSPARENT_BLUE_VALUE"
};
    int na = sizeof(atis)/sizeof(int);
    EGLint val;
    int j;
    for (j=0; j < na; ++j) {
       if ( EGL_TRUE == eglGetConfigAttrib( *edpy, *ec, atis[j], &val ) ) {
          debug( "%s: %d\n", atns[j], val );
       }
//          debug( "%s: %d\n", atns[j], val );
    }
}


int main() {
   int w = 300;
   int h = 300;

   int api_mask = 2;

   Display * ndpy = XOpenDisplay( NULL );
   if ( ! ndpy ) die( "Cannot open display\n" );

   int surftyp = EGL_WINDOW_BIT;

   EGLDisplay edpy = eglGetDisplay( EGL_DEFAULT_DISPLAY );
   if ( ! edpy ) die("Cannot get display\n");
   if ( ! eglInitialize( edpy, NULL, NULL ))
      die( "Cannot initialize\n");

   int NCONF=100;
   EGLConfig config[NCONF];
   int nconf;
   EGLint attrs [] = { EGL_RED_SIZE, 1, EGL_BLUE_SIZE, 1,
      EGL_GREEN_SIZE, 1, EGL_DEPTH_SIZE, 1, 
      EGL_CONFIG_ID, EGL_DONT_CARE, EGL_NONE };
   debug("edpy:%x\n", edpy );
   if ( ! eglChooseConfig( edpy, attrs, config, NCONF, &nconf ))
      die("Cannot get configs");
   debug( "n of configs: %d\n", nconf );

/*   int i;
   for (i=0; i < nconf; ++i) {
      debug( "\nCONFIG %d\n", i);
      EGLConfig * ec = config+i;
      debugconfig( &edpy, ec );
   }
   */

   return 0;
}
