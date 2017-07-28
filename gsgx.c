#include <EGL/egl.h>
#include <string.h>
#include <X11/Xlib.h>
#include <dlfcn.h>
#include "gsg.h"
#include "gsgx.h"
#include "gsga.h"

void * gsgxSelf = NULL;

Bool glXQueryExtension( Display * dpy,
   int * errorBase, int * eventBase)
{
   return True;
}

__GLXextFuncPtr glXGetProcAddressARB( const GLubyte * fname ) {
   if (NULL == gsgxSelf)
      gsgxSelf = dlopen( NULL, RTLD_LAZY );
   return dlsym( gsgxSelf, (const char *)fname );
}      
   


/*
/// count how many attribs are in array
int gsgxCountAttribs( const int * attrs ) {
   int ret = 0;
   while (None != attrs[2*ret])
      ++ret;
   return ret;
}
*/

/// convert glx attr to egl attr
EGLint gsgxEAttr( int attr ) {
   switch (attr) {
      case GLX_FBCONFIG_ID: return EGL_CONFIG_ID;
      case GLX_BUFFER_SIZE: return EGL_BUFFER_SIZE;
      case GLX_RED_SIZE:    return EGL_RED_SIZE;
      case GLX_GREEN_SIZE:  return EGL_GREEN_SIZE;
      case GLX_BLUE_SIZE:   return EGL_BLUE_SIZE;
      case GLX_ALPHA_SIZE:  return EGL_ALPHA_SIZE;
      case GLX_DEPTH_SIZE:  return EGL_DEPTH_SIZE;
      case GLX_VISUAL_ID:   return EGL_NATIVE_VISUAL_ID;
      case GLX_STENCIL_SIZE: return EGL_STENCIL_SIZE;
      case GLX_SAMPLE_BUFFERS: return EGL_SAMPLE_BUFFERS;
      case GLX_SAMPLES:     return EGL_SAMPLES;
      case GLX_X_VISUAL_TYPE:
         return GSGX_UNSUPPORTED;
      case GLX_DOUBLEBUFFER:
      case GLX_RGBA:
      case GLX_STEREO:
         return GSGX_UNSUPPORTED_SINGLE;
      default:
         gsgDie( "Unknown attrib: 0x%x", attr );
         return 0;
   }
}

/// convert glx attr value to egl value
EGLint gsgxEVal( int attr, int val ) {
   switch (attr) {
      case GLX_FBCONFIG_ID:
      case GLX_RED_SIZE:
      case GLX_GREEN_SIZE:
      case GLX_BLUE_SIZE:
      case GLX_ALPHA_SIZE:
         if ( GLX_DONT_CARE == val )
            val = EGL_DONT_CARE;
      break;
   }
   return val;
}

/// convert egl attr value to glx value
int gsgxGVal( EGLint eattr, EGLint val ) {
   switch (eattr) {
      case EGL_CONFIG_ID:
      case EGL_RED_SIZE:
      case EGL_GREEN_SIZE:
      case EGL_BLUE_SIZE:
      case EGL_ALPHA_SIZE:
         if ( EGL_DONT_CARE == val )
            val = GLX_DONT_CARE;
      break;
   }
   return val;
}

ARRDEF( EGLint );

/// convert glx attrib list to egl attrib list
EGLint * gsgxConvertAttribs( const int * attrs ) {
   gsgDebug("gsgxConvertAttribs\n");
   static ARR( EGLint ) * arr = NULL;
   if (NULL == arr)
      ARRINIT( arr, EGLint, 32 );
   arr->count = 0;
   const int * at = attrs;       
   while ( None != *at ) {
//      gsgDebug("at: %d\n", *at);
      int eattr = gsgxEAttr( *(at++) );
      switch (eattr) {
	 case GSGX_UNSUPPORTED_SINGLE: 
	    if (1 == *at)
	       ++at; 
	 break;
	 case GSGX_UNSUPPORTED: ++at; break;
	 default:
	    ARRADD( arr, EGLint, eattr );
	    ARRADD( arr, EGLint, gsgxEVal( eattr, *(at++) ));
      }
   }
   ARRADD( arr, EGLint, EGL_NONE );
   EGLint * ret = ALLOCN( EGLint, arr->count );
   COPYN( EGLint, arr->items, ret, arr->count );
   return ret;
}

/// as both are pointers, it can be casted
inline GLXFBConfig gsgxToGConfig( EGLConfig econf ) {
   return (GLXFBConfig)econf;
}

/// as both are pointers, it can be casted
inline EGLConfig gsgxToEConfig( GLXFBConfig gconf ) {
   return (EGLConfig)gconf;
}

/// as both are pointers, it can be casted
inline GLXContext gsgxToGContext( EGLContext econt ) {
   return (GLXContext)econt;
}

/// as both are pointers, it can be casted
inline EGLContext gsgxToEContext( GLXContext gcont ) {
   return (EGLContext)gcont;
}

/// dump config attributes
void gsgxDumpEAttrs( EGLint * eattrs ) {
   EGLint * at = eattrs;
   while (*at != EGL_NONE) {
      gsgDebug("%x ", *at);
      ++at;
   }
   gsgDebug("\n");
}

/// get and initialize display
EGLDisplay gsgxGetDisplay( Display * dpy, Bool init ) {
   EGLDisplay ret = eglGetDisplay( dpy );
   if (NULL == ret || EGL_NO_DISPLAY == ret )
      gsgDie("Cannot get display\n");
   if (init) {
	   if ( ! eglInitialize( ret, NULL, NULL ))
	      gsgDie("Could not initialize display:%x\n", eglGetError() );
   }
   return ret;
}

GLXFBConfig * glXChooseFBConfig( Display * dpy,
   int screen, const int * attrib_list, int * nelements )
{
   gsgDebug("glXChooseFBConfig\n");
   int MAXCONFIGS = 16;
   int i;
   *nelements = 0;
   GLXFBConfig * ret = NULL;
   EGLDisplay ed = gsgxGetDisplay( dpy, True );
   if ( EGL_NO_DISPLAY == ed )
      return ret;
   EGLint * elist = gsgxConvertAttribs( attrib_list );
   EGLConfig econfs [MAXCONFIGS];
   EGLint enconf;
   gsgDebug("ed:%x\n", ed );
   gsgxDumpEAttrs( elist );
   EGLBoolean any = eglChooseConfig( ed, elist, econfs, MAXCONFIGS, &enconf );
   gsgDebug("enconf:%d any:%d\n", enconf, any );
   gsgFree( elist );
   if ( ! any )
      enconf = 0;
   *nelements = MAXCONFIGS < enconf ? MAXCONFIGS : enconf;
//   gsgDebug("nelems:%d\n", *nelements);
   ret = ALLOCN( GLXFBConfig, *nelements );
   for (i=0; i < *nelements; ++i)
      ret[i] = gsgxToGConfig( econfs[i] );
   return ret;
}

XVisualInfo * glXGetVisualFromFBConfig( Display * dpy,
   GLXFBConfig cfg )
{
   gsgDebug("glXGetVisualFromFBConfig\n");
   XVisualInfo * ret = ALLOC( XVisualInfo );
   int val;
   int code = glXGetFBConfigAttrib( dpy, cfg, GLX_VISUAL_ID, & val );
   if ( Success == code ) {
      ret->visualid = val;
	  int n;
      XVisualInfo * tmp = XGetVisualInfo( dpy, VisualIDMask, ret, &n );
      if ( tmp ) {
	 COPY( XVisualInfo, tmp, ret );
	 XFree( tmp );
      }
   } else 
      gsgDie("could not get config attrib:%x\n", code );
   return ret;
}

int glXGetConfig( Display * dpy, XVisualInfo * vis, int attrib,
   int * value )
{
   switch (attrib) {
      default: return GLX_BAD_ATTRIBUTE; 
   }
}

int glXGetFBConfigAttrib( Display * dpy, GLXFBConfig config,
 	int attribute, int * value)
{
   gsgDebug("glXGetFBConfigAttrib %x\n", attribute);
   EGLConfig econf = gsgxToEConfig( config );
   EGLDisplay edpy = gsgxGetDisplay( dpy, True );
   EGLint eattr = gsgxEAttr( attribute );
   EGLint val;
   if ( EGL_FALSE == eglGetConfigAttrib( edpy, econf, eattr, & val ))
      gsgDie("Cannot get attribute: %x\n", eglGetError() );
//	   return GLX_BAD_ATTRIBUTE;
   *value = gsgxGVal( eattr, val );
    return Success;
}

/// list of contexts
typedef struct {
   GLXContext  gctx;
   EGLConfig   ecfg;
   EGLDisplay  edpy;
} gsgxCtx;

ARRDEF( gsgxCtx );
ARR( gsgxCtx ) * gsgxCtxs = NULL;

GLXContext glXCreateNewContext(	Display * dpy, GLXFBConfig config,
 	int render_type, GLXContext share_list, Bool direct)
{
   gsgDebug("glXCreateNewContext\n");
   if (render_type == GLX_COLOR_INDEX_TYPE)
      gsgDie( "Color index type not supported\n");
   EGLDisplay edpy = gsgxGetDisplay( dpy, True );
   EGLConfig econf = gsgxToEConfig( config );
gsgDebug("econf:%x config:%x\n", econf, config );
   EGLContext eshare = gsgxToEContext( share_list );
   EGLContext ectx = eglCreateContext( edpy, econf, eshare, NULL );
   if ( EGL_NO_CONTEXT == ectx )
      gsgDie("Could not create context: %x\n", eglGetError() );
   GLXContext ret = gsgxToGContext( ectx );
   if (NULL == gsgxCtxs)
      ARRINIT( gsgxCtxs, gsgxCtx, 8 );
   gsgxCtx gc = { .gctx = ret, .ecfg = econf, .edpy = edpy };
   ARRADD( gsgxCtxs, gsgxCtx, gc );
   return ret;
}

Bool glXIsDirect( Display * dpy, GLXContext gctx ) {
   gsgDebug("glXIsDirect\n");
   return True;
}


EGLDisplay gsgxEDisplay( GLXContext ctx ) {
   ARRLOOKUP( gsgxCtxs, gctx, ctx, edpy );
   gsgDie("Unkown display: %s\n", ctx );
   return NULL;
}	

EGLConfig gsgxEConfig( GLXContext ctx ) {
   ARRLOOKUP( gsgxCtxs, gctx, ctx, ecfg );
   gsgDie("Unkown config: %s\n", ctx );
   return NULL;
}

GLXContext gsgxGContext( EGLDisplay edpy ) {
   ARRLOOKUP( gsgxCtxs, edpy, edpy, gctx );
   gsgDie( "Unknown display: %x\n", edpy );
   return NULL;
}

/// list of surfaces
typedef struct {
   GLXDrawable gdraw;
   EGLSurface  esurf;
} gsgxSurf;

ARRDEF( gsgxSurf );
ARR( gsgxSurf ) * gsgxSurfs;


EGLSurface gsgxToESurf( GLXDrawable draw, GLXContext ctx ) {
   if (0 == draw)
      return NULL;
   if (NULL == gsgxSurfs)
      ARRINIT( gsgxSurfs, gsgxSurf, 8 );
   // try lookup
   ARRLOOKUP( gsgxSurfs, gdraw, draw, esurf );
   // create 
   EGLDisplay edpy = gsgxEDisplay( ctx );
   EGLConfig ecfg = gsgxEConfig( ctx );
   EGLSurface ret = eglCreateWindowSurface( edpy, ecfg, draw,
      NULL );
   if (EGL_NO_SURFACE == ret)
      gsgDie("Cannot create surface\n" );
   gsgxSurf s = { .gdraw = draw, .esurf = ret };
   ARRADD( gsgxSurfs, gsgxSurf, s );
   return ret;
}

inline Bool gsgxToBool( EGLBoolean b ) {
	return b ? True : False;
}

Bool glXMakeContextCurrent(	Display * display, GLXDrawable draw,
   GLXDrawable read, GLXContext ctx)
{
//   gsgDebug( "glxMakeContextCurrent dpy:%x draw:%x read:%x ctx:%x\n",
//      display, draw, read, ctx );
   EGLDisplay edpy = gsgxGetDisplay( display, False );
   EGLSurface edraw = gsgxToESurf( draw, ctx );
   EGLSurface eread = gsgxToESurf( read, ctx );
   EGLContext ectx = gsgxToEContext( ctx );
   return gsgxToBool( eglMakeCurrent( edpy, edraw, eread, ectx ));
}

void glXSwapBuffers( Display * dpy, GLXDrawable draw ) {
   EGLDisplay edpy = gsgxGetDisplay( dpy, False );
   GLXContext gctx = gsgxGContext( edpy );
   EGLSurface edraw = gsgxToESurf( draw, gctx );
//   gsgDebug("draw: %x edraw:%x\n", draw, edraw );
   if ( ! eglSwapBuffers( edpy, edraw ))
      gsgDie( "Cannot swap buffers: %x\n", eglGetError() );
}

void glXDestroyContext( Display * dpy, GLXContext gctx ) {
   EGLDisplay edpy = gsgxGetDisplay( dpy, False );
   EGLContext ectx = gsgxToEContext( gctx );
   if ( eglDestroyContext( edpy, ectx )) 
      ARRREMOVEBY( gsgxCtxs, gsgxCtx, gctx, gctx );
}

XVisualInfo * glXChooseVisual( Display *dpy, int screen, 
   int * attriblist ) 
{
   XVisualInfo * ret = NULL;
   int nem;
   GLXFBConfig * cfg = glXChooseFBConfig( dpy, screen, attriblist, & nem );
   if ( cfg ) {
      ret = glXGetVisualFromFBConfig( dpy, *cfg );
      gsgFree( cfg );
   }
   return ret;
}

/// number of set bits in mask
int gsgxMaskSize( unsigned long mask ) {
   int ret = 0;
   int n;
   for (n=32; 0<n; --n)  {
      if (mask & 1)
         ++ret;
      mask >>= 1;
   }
   return ret;
}

/// return attribute set by visual
const int * gsgxAttrsByVisual( XVisualInfo * vis ) {
   static int ret[32];
   int *at = ret;
   *(at++) = GLX_DEPTH_SIZE;
   *(at++) = vis->depth;
   *(at++) = GLX_RED_SIZE;
   *(at++) = gsgxMaskSize( vis->red_mask );
   *(at++) = GLX_GREEN_SIZE;
   *(at++) = gsgxMaskSize( vis->green_mask );       
   *(at++) = GLX_BLUE_SIZE;
   *(at++) = gsgxMaskSize( vis->blue_mask );
   *(at++) = None;
   return ret;
}


GLXContext glXCreateContext( Display * dpy, 
   XVisualInfo * vis, GLXContext share_list, Bool direct)
{
   gsgDebug("glXCreateContext\n");
   int nem;
   const int * attrs = gsgxAttrsByVisual( vis );
   GLXFBConfig * cfg = glXChooseFBConfig( dpy, GSGX_DEFAULT_SCREEN, 
      attrs, & nem );
   return glXCreateNewContext( dpy, *cfg, GLX_RGBA_TYPE, share_list, 
      direct );
}

Bool glXMakeCurrent( Display * dpy, GLXDrawable drawable,
   GLXContext ctx )
{
   EGLDisplay edpy = gsgxGetDisplay( dpy, True );
   EGLSurface esurf = gsgxToESurf( drawable, ctx );
   EGLContext ectx = gsgxToEContext( ctx );
   return gsgxToBool( eglMakeCurrent( edpy, esurf, esurf, ectx ) );
}

const char * glXQueryExtensionsString(	Display * dpy,
   int screen)
{
   return "";
}   
  
