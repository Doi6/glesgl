#ifndef GSGH
#define GSGH

#include <stdlib.h>
#include <GL/gl.h>

void gsgInit();
void gsgDie( const char * fmt, ... );
void gsgDebug( const char * fmt, ... );
void gsgErr( GLenum value );
void gsgFree( void * );

extern GLenum gsgError;

#endif // GSGH
