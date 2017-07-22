// GSG group fro glBegin ... glEnd
#include "gsg.h"
#include "gsgg.h"
#include "gsga.h"

#define INITSIZE 256

typedef struct {
   GLfloat x,y,z;
} gsggVect;

ARRDEF( gsggVect );

typedef struct {
   GLenum mode;
   ARR( gsggVect ) * verts;
   ARR( gsggVect ) * norms;
} gsggGroup;

gsggGroup * gsgg = NULL;

gsggGroup * gsggCreate() {
   gsggGroup * ret = ALLOC( gsggGroup );
   ret->mode = 0;
   ARRINIT( ret->verts, gsggVect, INITSIZE );
   ARRINIT( ret->norms, gsggVect, INITSIZE );
   return ret;
}      


void gsggClear() {
   if ( ! gsgg ) 
      gsgg = gsggCreate();
   gsgg->verts->count = 0;
   gsgg->norms->count = 0;
}

extern void glBegin( GLenum mode ) {
   if (NULL == gsgg)
      gsgg  = gsggCreate();
   if (gsgg->mode) {
      gsgErr( GL_INVALID_OPERATION );
      return; 
   }
   switch (mode) {
      case GL_POINTS:
      case GL_LINES:
      case GL_LINE_LOOP:
      case GL_TRIANGLES:
      case GL_TRIANGLE_STRIP:
      case GL_TRIANGLE_FAN:
      case GL_QUADS:
      case GL_QUAD_STRIP:
      case GL_POLYGON:
         break;
      default:
         gsgErr( GL_INVALID_ENUM );
	 return;
   }
   gsggClear();
   gsgg->mode = mode;
}
   
void gsggDraw(GLenum mode) {
   glVertexPointer( 3, GL_FLOAT, 0, gsgg->verts->items );
   glNormalPointer( GL_FLOAT, 0, gsgg->norms->items );
   glDrawArrays( mode, 0, gsgg->verts->count );
}
   
void glEnd() {
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   switch ( gsgg->mode ) {
      case GL_QUADS: gsggDraw( GL_TRIANGLES ); break;
      default: gsggDraw( gsgg->mode );
   } 
   if ( GL_NO_ERROR != glGetError() )
      gsgDie("after draw:%x\n", glGetError() );
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   gsggClear();
}

/// add vertices as triangles for drawing
void gsggOnVertexQuad() {
   ARR( gsggVect ) * arr = gsgg->verts;
   int n = gsgg->verts->count;
   if ( 4 == n % 6 ) {
      gsggVect v = arr->items[n-4];
      ARRADD( arr, gsggVect, v );
      v = arr->items[n-2];
      ARRADD( arr, gsggVect, v );
      n += 2;
      // each vertex should have a normal
      int m = gsgg->norms->count;
      if ( 0 < m ) {
         v = gsgg->norms->items[m-1];
	 while ( gsgg->norms->count < n )
	    ARRADD( gsgg->norms, gsggVect, v );
      }
   }
}      
   
void gsggVertex( GLfloat x, GLfloat y, GLfloat z ) {
//   gsgDebug("vertex (%g,%g,%g,%g)\n", x, y, z, w );
   gsggVect v = { x, y, z };
   ARRADD( gsgg->verts, gsggVect, v );
   switch ( gsgg->mode ) {
      case GL_QUADS: gsggOnVertexQuad();
   }
}

void gsggNormal( GLfloat x, GLfloat y, GLfloat z ) {
//   gsgDebug("vertex (%g,%g,%g,%g)\n", x, y, z, w );
   gsggVect v = { x, y, z };
   ARRADD( gsgg->norms, gsggVect, v );
}
