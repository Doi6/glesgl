// GSG group fro glBegin ... glEnd
#include "gsg.h"
#include "gsgg.h"
#include "gsga.h"
#include "gsgl.h"

#define INITSIZE 256

typedef struct {
   GLfloat x,y,z;
} gsggVect;

ARRDEF( gsggVect );

typedef struct {
   GLfloat r,g,b,a;
} gsggCol;

ARRDEF( gsggCol );

typedef struct {
   GLenum mode;
   ARR( gsggVect ) * verts;
   ARR( gsggVect ) * norms;
   ARR( gsggCol ) * cols;
} gsggGroup;

gsggGroup * gsgg = NULL;
static Bool gsggIn = False;

gsggGroup * gsggCreate() {
   gsggGroup * ret = ALLOC( gsggGroup );
   ret->mode = 0;
   ARRINIT( ret->verts, gsggVect, INITSIZE );
   ARRINIT( ret->norms, gsggVect, INITSIZE );
   ARRINIT( ret->cols,  gsggCol,  INITSIZE );
   return ret;
}      

Bool gsggInGroup() {
   return gsggIn;
}

void gsggClear() {
   if ( ! gsgg ) 
      gsgg = gsggCreate();
   gsgg->verts->count = 0;
   gsgg->norms->count = 0;
   gsgg->cols->count  = 0;
}

extern void glBegin( GLenum mode ) {
   LIST( e, GLBEGIN, mode );
   if (NULL == gsgg)
      gsgg  = gsggCreate();
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
   gsggIn = True;
}

/// make as many normals as vertices
void gsggExtendNorms() {
   int m = gsgg->norms->count;
   if ( 0 < m ) {
      gsggVect v = gsgg->norms->items[m-1];
      int n = gsgg->verts->count;
      while ( gsgg->norms->count < n )
	 ARRADD( gsgg->norms, gsggVect, v );
   }
}   

/// make as many colors as vertices
void gsggExtendCols() {
   int m = gsgg->cols->count;
   if ( 0 < m ) {
      gsggCol c = gsgg->cols->items[m-1];
      int n = gsgg->verts->count;
      while ( gsgg->cols->count < n )
	 ARRADD( gsgg->cols, gsggCol, c );
   }
}   
   
void gsggDraw(GLenum mode) {
   glVertexPointer( 3, GL_FLOAT, 0, gsgg->verts->items );
   if ( 0 < gsgg->norms->count )
      glNormalPointer( GL_FLOAT, 0, gsgg->norms->items );
   if ( 0 < gsgg->cols->count )
      glColorPointer( 4, GL_FLOAT, 0, gsgg->cols->items );
   glDrawArrays( mode, 0, gsgg->verts->count );
}
   
void glEnd() {
   LIST( _, GLEND );
   glEnableClientState(GL_VERTEX_ARRAY);
   if ( 0 < gsgg->norms->count ) {
      gsggExtendNorms();
      glEnableClientState(GL_NORMAL_ARRAY);
   }
   if ( 0 < gsgg->cols->count ) {
      gsggExtendCols();
      glEnableClientState(GL_COLOR_ARRAY);
   }
   switch ( gsgg->mode ) {
      case GL_QUADS: 
         gsggDraw( GL_TRIANGLES ); 
      break;
      case GL_QUAD_STRIP: 
         gsggDraw( GL_TRIANGLE_STRIP );
      break;
      default: gsggDraw( gsgg->mode );
   } 
   if ( GL_NO_ERROR != glGetError() )
      gsgDie("after draw:%x\n", glGetError() );
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   gsggClear();
   gsggIn = False;
}

/// add quad vertices as triangles for drawing
void gsggOnVertexQuad() {
   ARR( gsggVect ) * arr = gsgg->verts;
   int n = arr->count;
   if ( 4 == n % 6 ) {
      gsggVect v = arr->items[n-4];
      ARRADD( arr, gsggVect, v );
      v = arr->items[n-2];
      ARRADD( arr, gsggVect, v );
      n += 2;
   }
}      

/*
/// add quad strip vertices as triangles for drawing
void gsggOnVertexQuadStrip() {
   ARR( gsggVect ) * arr = gsgg->verts;
   int n = arr->count;
   if ( 3 >= n )
      return;
   gsggVect v;
   if ( 0 == n % 2 ) {
      v = arr->items[n-1];
      ARRADD( arr, gsggVect, v );
      v = arr->items[n-2];
      ARRADD( arr, gsggVect, v );
   } else {
      v = arr->items[n-2];
      ARRADD( arr, gsggVect, v );
      v = arr->items[n-1];
      ARRADD( arr, gsggVect, v );
   }
}
*/

void gsggVertex( GLfloat x, GLfloat y, GLfloat z ) {
   gsggVect v = { x, y, z };
   ARRADD( gsgg->verts, gsggVect, v );
   switch ( gsgg->mode ) {
      case GL_QUADS: gsggOnVertexQuad(); break;
//      case GL_QUAD_STRIP: gsggOnVertexQuadStrip(); break;
   }
}

void gsggNormal( GLfloat x, GLfloat y, GLfloat z ) {
   gsggExtendNorms();
   gsggVect v = { x, y, z };
   ARRADD( gsgg->norms, gsggVect, v );
}

void gsggColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a ) {
   gsggExtendCols();
   gsggCol c = { r, g, b, a };
   ARRADD( gsgg->cols, gsggCol, c );
}
