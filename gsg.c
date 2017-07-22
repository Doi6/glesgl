#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "gsg.h"
#include "gsgg.h"

char * gsgGLlib  = "/usr/lib/intel-cdv/libGLES.so";
void * gsgGLlibp = NULL;
GLenum gsgError;

void gsgDie( const char * fmt, ... ) {
   va_list args;
   va_start( args, fmt );
   vfprintf( stderr, fmt, args );
   va_end( args );
   exit(1);
}

void gsgDebug( const char * fmt, ... ) {
   va_list args;
   va_start( args, fmt );
   vfprintf( stderr, fmt, args );
   va_end( args );
}

void gsgFree( void * ptr ) {
   free( ptr );
}

void gsgErr( GLenum value ) {
   gsgError = value;
}

void * gsgDl( const char * name ) {
   if ( ! gsgGLlibp ) {
      if ( ! (gsgGLlibp = dlopen( gsgGLlib, RTLD_LAZY | RTLD_LOCAL )))
         gsgDie("Could not load library: %s\n", dlerror());
   }
   gsgDebug("libp:%x name:'%s'\n", gsgGLlibp, name );
   void * ret;
   if ( ! (ret = dlsym( gsgGLlibp, name )))
      gsgDie("Could not find symbol: %s\n", dlerror());
   return ret;
}

inline float * gsgToMatrixf( const GLdouble * md ) {
   static GLfloat mf[16];
   GLfloat * at = mf;
   int n;
   for (n=16; 0<n; --n)
      *(at++) = *(md++);
   return mf;
}   
   
extern void glMultMatrixd( const GLdouble * md ) {
   glMultMatrixf( gsgToMatrixf( md ) );
}

extern void glTranslated( GLdouble x, GLdouble y, GLdouble z ) {
   glTranslatef( x, y, z );
}

extern void glNormal3fv( const GLfloat * v ) {
   gsggVector( GSGG_NORMAL, v[0], v[1], v[2], 1.0f );      
}

extern void glVertex3fv( const GLfloat * v ) {
   gsggVector( GSGG_VERTEX, v[0], v[1], v[2], 1.0f );
}



