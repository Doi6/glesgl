// GSG group fro glBegin ... glEnd
#include "gsg.h"
#include "gsgg.h"
#include "gsga.h"

#define INITSIZE 256

typedef GLfloat gsggVect[4];

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
   
void gsggDrawQuads() {
   static int back[] = { -16, -15, -14, -13, -8, -7, -6, -5 };
   /// make space
   int vn = gsgg->verts->count;
   gsgDebug("vn:%d\n", vn);
   int vm = 2 * vn;
   if ( gsgg->tmp->size < vm )
      ARRSIZE( gsgg->tmp, GLfloat, vm );
   /// create triangles
   GLfloat *at = gsgg->verts->items;
   GLfloat *dest = gsgg->tmp->items;
   glVertexPointer( 4, GL_FLOAT, 0, dest );
   int n = 0;
   int m = 0;
   for (; 0<vn; --vn,++n) {
      *(dest++) = *(at++);
      if (15 == n%16) {
	 gsgDebug("15\n");
	 for (m=0; m<8; ++m)
	    *(dest++) = at[ back[m] ];
      }
   }
//   gsgDebug("nnorm: %d\n", dest - gsgg->tmp->items );
/*   for (n=0; n < vm / 16; ++n) {
      at = gsgg->tmp->items + 12*n;
      gsgDebug("tri: (%g,%g,%g),(%g,%g,%g),(%g,%g,%g)\n",
         at[0], at[1], at[2], at[4], at[5], at[6], at[8], at[9], at[10] );
   }
*/
   /// create normals
   if ( 4 <= gsgg->norms->count ) {
      while (gsgg->norms->count < gsgg->verts->count) {
	 GLfloat f = gsgg->norms->items[ gsgg->norms->count-4 ];
         ARRADD( gsgg->norms, GLfloat, f );
      }
      
      at = gsgg->norms->items;
      glNormalPointer( GL_FLOAT, 0, dest );
      int nn = gsgg->norms->count / 4;
      for (; 0<nn; --nn) {
         for (m=4; 0<m; --m) {
	    dest[4] = at[4];
            *(dest++) = *(at++);
         }
         dest += 4;
      }
      gsgDebug("nnorm: %d\n", dest - gsgg->tmp->items );
      glDrawArrays( GL_TRIANGLES, 0, gsgg->verts->count * 3 / 2 );   
   }
}   
   
void gsggDraw(GLenum mode) {
   glVertexPointer( 4, GL_FLOAT, 0, gsgg->verts->items );
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

   
void gsggVector( int kind, GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
   ARR( GLfloat ) * arr;
   switch (kind) {
      case GSGG_VERTEX: arr = gsgg->verts; break;
      case GSGG_NORMAL: arr = gsgg->norms; break;
      default:
         gsgDie("Unknown kind: %d\n", kind );
   }
   gsgDebug("vector %d (%g,%g,%g,%g)\n", kind, x, y, z, w );
   ARRADD( arr, GLfloat, x );
   ARRADD( arr, GLfloat, y );
   ARRADD( arr, GLfloat, z );
   ARRADD( arr, GLfloat, w );
   gsgDebug("arrcnt: %d\n", arr->count );
}
