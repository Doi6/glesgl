#ifndef GSGH
#define GSGH

#include <stdlib.h>
#include <GL/gl.h>

void gsgInit();
extern void gsgDie( const char * fmt, ... );
extern void gsgUnsupp( const char * fmt, ... );
extern void gsgDebug( int level, const char * fmt, ... );
extern void * gsgDl( const char * name );
extern void gsgErr( GLenum value );
extern void gsgOk();
void gsgFree( void * );

#endif // GSGH
