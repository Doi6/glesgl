#ifndef GSGGH
#define GSGGH

#include "GL/glx.h"

#define GSGG_VERTEX 1
#define GSGG_NORMAL 2

#define GROUP( sign, name, ... )   \
   gsgOk();                        \
   if ( gsgg##name( __VA_ARGS__ )) \
      return;                      
 
typedef struct {
   GLfloat x,y,z;
} gsggVect;
 
/// in group
GLboolean gsggVertex( GLfloat x, GLfloat y, GLfloat z );
GLboolean gsggNormal( GLfloat x, GLfloat y, GLfloat z );
GLboolean gsggColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a );
GLboolean gsggTex( GLfloat s, GLfloat t, GLfloat p );
GLboolean gsggIndex( GLfloat c );

GLboolean gsggInGroup();

#endif // GSGGH
