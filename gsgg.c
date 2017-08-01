// GSG group fro glBegin ... glEnd
#include "gsg.h"
#include "gsgg.h"
#include "gsga.h"
#include "gsgl.h"

#define INITSIZE 256

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
   ARR( gsggVect ) * texs;
} gsggGroup;

static gsggGroup * gsgg = NULL;
static GLboolean gsggIn = False;

static gsggVect gsggnorm = {0.0f,0.0f,0.0f};
static gsggVect gsggtex = {0.0f,0.0f,0.0f};
static gsggCol gsggcol = {0.0f,0.0f,0.0f,0.0f};

gsggGroup * gsggCreate() {
   gsggGroup * ret = ALLOC( gsggGroup );
   ret->mode = 0;
   ARRINIT( ret->verts, gsggVect, INITSIZE );
   ARRINIT( ret->norms, gsggVect, INITSIZE );
   ARRINIT( ret->cols,  gsggCol,  INITSIZE );
   ARRINIT( ret->texs, gsggVect, INITSIZE );
   return ret;
}      

inline GLboolean gsggInGroup() {
   return gsggIn;
}

/// extend norms, cols, texs by idx
void gsggExtend( int idx ) {
   if ( 0 > idx )
      return;
   int nv = gsgg->verts->count;
   int nn = gsgg->norms->count;
   if ( idx < nn && nn < nv  ) {
      gsggVect v = gsgg->norms->items[idx];
      ARRADD( gsgg->norms, gsggVect, v );
   }
   int nc = gsgg->cols->count;
   if ( idx < nc && nc < nv ) {
      gsggCol c = gsgg->cols->items[idx];
      ARRADD( gsgg->cols, gsggCol, c );
   }
   int nt = gsgg->texs->count;
   if ( idx < nt && nt < nv ) {
      gsggVect v = gsgg->texs->items[idx];
      ARRADD( gsgg->texs, gsggVect, v );
   }
}

void gsggClear() {
   if ( ! gsgg ) 
      gsgg = gsggCreate();
   gsgg->verts->count = 0;
   gsgg->norms->count = 0;
   gsgg->cols->count  = 0;
   gsgg->texs->count = 0;
}

extern void glBegin( GLenum mode ) {
   LIST( i, GLBEGIN, mode );
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
   
void gsggDraw(GLenum mode) {
   glVertexPointer( 3, GL_FLOAT, 0, gsgg->verts->items );
   if ( 0 < gsgg->norms->count )
      glNormalPointer( GL_FLOAT, 0, gsgg->norms->items );
   if ( 0 < gsgg->cols->count )
      glColorPointer( 4, GL_FLOAT, 0, gsgg->cols->items );
   if ( 0 < gsgg->texs->count )
      glTexCoordPointer( 3, GL_FLOAT, 0, gsgg->texs->items );
   glDrawArrays( mode, 0, gsgg->verts->count );
}
   
void glEnd() {
   LIST( _, GLEND );
   glEnableClientState(GL_VERTEX_ARRAY);
   if ( 0 < gsgg->norms->count )
      glEnableClientState(GL_NORMAL_ARRAY);
   if ( 0 < gsgg->cols->count )
      glEnableClientState(GL_COLOR_ARRAY);
   if ( 0 < gsgg->texs->count )
      glEnableClientState( GL_TEXTURE_COORD_ARRAY );
   GLenum bef = glGetError();
   switch ( gsgg->mode ) {
      case GL_QUADS: 
         gsggDraw( GL_TRIANGLES ); 
      break;
      case GL_QUAD_STRIP: 
         gsggDraw( GL_TRIANGLE_STRIP );
      break;
      default: gsggDraw( gsgg->mode );
   } 
   GLenum err = glGetError();
   if ( GL_NO_ERROR != err )
      gsgDie("after draw:%x %x %x\n", gsgg->mode, bef, err );
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
      gsggExtend( n-4 );
      v = arr->items[n-2];
      ARRADD( arr, gsggVect, v );
      gsggExtend( n-2 );
      n += 2;
   }
}      

GLboolean gsggVertex( GLfloat x, GLfloat y, GLfloat z ) {
   if ( gsggIn ) {
      gsggVect v = { x, y, z };
      int idx = gsgg->verts->count-1;
      ARRADD( gsgg->verts, gsggVect, v );
      gsggExtend( idx );
      switch ( gsgg->mode ) {
         case GL_QUADS: gsggOnVertexQuad(); break;
      }
   }
   return gsggIn;
}

GLboolean gsggNormal( GLfloat x, GLfloat y, GLfloat z ) {
   gsggVect v = {x, y, z};
   if ( gsggIn ) {
      int nv = gsgg->verts->count;
      if ( gsgg->norms->count > nv )
         -- gsgg->norms->count;
      else while (gsgg->norms->count < nv)
         ARRADD( gsgg->norms, gsggVect, gsggnorm );
      ARRADD( gsgg->norms, gsggVect, v );
   } else
      gsggnorm = v;
   return gsggIn;
}

GLboolean gsggColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a ) {
   gsggCol c = { r,g,b,a };
   if ( gsggIn ) {
      int nv = gsgg->verts->count;
      if ( gsgg->cols->count > nv )
         -- gsgg->cols->count;
      else while (gsgg->cols->count < nv)
         ARRADD( gsgg->cols, gsggCol, gsggcol );
      ARRADD( gsgg->cols, gsggCol, c );
   } else 
      gsggcol = c;
   return gsggIn;
}
 
GLboolean gsggTex( GLfloat s, GLfloat t, GLfloat p ) {
   gsggVect v = { s, t, p };
   if ( gsggIn ) {
      int nv = gsgg->verts->count;
      if ( gsgg->texs->count > nv )
         -- gsgg->texs->count;
      else while (gsgg->texs->count < nv)
         ARRADD( gsgg->texs, gsggVect, gsggtex );
      ARRADD( gsgg->texs, gsggVect, v );
   } else
      gsggtex = v;
   return gsggIn;
}


