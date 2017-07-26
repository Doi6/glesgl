#ifndef GSGGH
#define GSGGH

#include "GL/glx.h"

#define GSGG_VERTEX 1
#define GSGG_NORMAL 2

#define GROUP( sign, name, ... )  \
   gsgOk();                       \
   if ( gsggInGroup() ) {         \
      gsgg##name( __VA_ARGS__ );  \
      return;                     \
   }

void gsggVertex( GLfloat x, GLfloat y, GLfloat z );
void gsggNormal( GLfloat x, GLfloat y, GLfloat z );
void gsggColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a );
void gsggTex( GLfloat s, GLfloat t, GLfloat p );

Bool gsggInGroup();

#endif // GSGGH
