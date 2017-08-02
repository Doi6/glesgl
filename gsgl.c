// Display lists
#include "gsg.h"
#include "gsgg.h"
#include "gsga.h"
#include "gsgl.h"

#define GSGL_NONE -1

ARRDEF( GLint );
ARRDEF( GLfloat );

/// a display list
typedef struct {
   ARR( GLint ) * ints;
   ARR( GLfloat ) * floats;
} gsgList;

typedef gsgList * gsgListp;

ARRDEF( gsgListp );

ARR( gsgListp ) * gsgLists = NULL;
int gsglCurrent = GSGL_NONE;
int gsglMode = 0;

gsgListp gsglNewList() {
   gsgListp ret = ALLOC( gsgList );
   ARRINIT( ret->ints, GLint, 64 );
   ARRINIT( ret->floats, GLfloat, 64 );
   return ret;
}

void gsglFreeList( gsgListp lst ) {
   ARRFREE( lst->ints );
   ARRFREE( lst->floats );
   FREE( lst );
}

extern GLuint glGenLists( GLsizei range ) {
   if (NULL == gsgLists) {
      ARRINIT( gsgLists, gsgListp, 8 );
      ARRADD( gsgLists, gsgListp, NULL );
   }
   int ret = gsgLists->count;
   int i;
   for (i=0; i < range; ++i) {
      gsgListp l = gsglNewList();
      ARRADD( gsgLists, gsgListp, l); 
   }
   return ret;
}

static void gsglDeleteList( int i ) {
   if ( 0 > i || i >= gsgLists->count )
      return;
   gsgListp lst = gsgLists->items[i];
   if (NULL == lst)
      return;
   gsglFreeList( lst );
   gsgLists->items[i] = NULL;
   /// free up indexes at end
   if (i == gsgLists->count-1) {
      while ( 0 < i && NULL == gsgLists->items[i] ) {
	 --gsgLists->count;
	 --i;
      }
   }
}

extern void glDeleteLists( GLuint list, GLsizei range ) {
   gsgOk();
   if (NULL == gsgLists)
      return;
   int i;
   for (i=0; i < range; ++i)
      gsglDeleteList( list+i );
}

Bool gsglInList() {
   return 0 <= gsglCurrent;
}

extern void glNewList( GLuint list, GLenum mode ) {
   if (!(GL_COMPILE == mode || GL_COMPILE_AND_EXECUTE == mode )) {
      gsgErr( GL_INVALID_ENUM );
      return;
   }
   if ( ! glIsList( list ) ) {
      gsgErr( GL_INVALID_VALUE );
      return;
   }
   if ( gsggInGroup() || gsglInList() ) {
      gsgErr( GL_INVALID_OPERATION );
      return;
   }
   gsglCurrent = list;
   gsglMode = mode;
}

extern void glEndList() {
   int save = gsglCurrent;
   gsglCurrent = GSGL_NONE;
   if ( GL_COMPILE_AND_EXECUTE == gsglMode )
      glCallList( save );
}

int gsglArrSizeP( int op, GLint i, GLenum e ) {
   switch (op) {
      case GLDRAWELEMENTS:
         switch (e) {
	    case GL_UNSIGNED_BYTE: return i;
	    case GL_UNSIGNED_SHORT: return 2*i;
	 }
      break;
   }
   gsgDie("Unkown ArrSizeP op:%x i:%d e:%x\n", op, i, e );
   return 0;
}

int gsglArrSizeF( int op, GLenum e ) {
   switch (op) {
      case GLMATERIALFV: 
         switch (e) {
	    case GL_AMBIENT: 
	    case GL_DIFFUSE: 
	    case GL_SPECULAR: 
	    case GL_EMISSION:
	    case GL_AMBIENT_AND_DIFFUSE: 
	       return 4;
	    case GL_COLOR_INDEXES: return 3;
	    case GL_SHININESS: return 1;
         }
      break;
   }
   gsgDie("Unkonwn ArrSizeF op:%d e:%x\n", op, e );
   return 0;
}

void gsglBytes( gsgList * l, int op, GLint gi, GLenum e,
   const GLvoid * v ) 
{
   int n = gsglArrSizeP( op, gi, e );
   int m = n/4;
   int i;
   GLint iv;
   GLint *at = (GLint *)v;
   for (i=0; i < m; ++i) {
      iv = *(at++);
      ARRADD( l->ints, GLint, iv );
   }
   if ( 0 == n % 4 ) return;
   // remainder bytes
   GLubyte *bat = (GLubyte *)at;
   iv = 0;
   for (i=0; i < n%4; ++i) {
      iv <<= 8;
      iv += *(bat++);
   }
   ARRADD( l->ints, GLint, iv );
}

void gsglFloats( gsgList * l, int op, GLenum e, const GLfloat * v ) {
   int n = gsglArrSizeF( op, e );
   int i;
   for (i=0; i < n; ++i)
      ARRADD( l->floats, GLfloat, v[i] );
}   

void gsglExecute( gsgList * l ) {
   GLint * ip = l->ints->items;
   GLint * iq = ip + l->ints->count;
   GLfloat * fp = l->floats->items;
   while ( ip < iq ) {
      GLint op = *(ip++);
      switch ( op ) {
	 case GLMATERIALFV:
	    glMaterialfv( ip[0], ip[1], fp );
	    fp += gsglArrSizeF( op, ip[1] );
	    ip += 2;
	 break;
	 case GLSHADEMODEL: glShadeModel( *(ip++) ); break;
	 case GLVERTEX3F:
	    glVertex3fv( fp );
	    fp += 3;
	 break;
	 case GLNORMAL3F: 
	    glNormal3fv( fp ); 
	    fp += 3;
	 break;
	 case GLBEGIN: glBegin( *(ip++) ); break;
	 case GLEND: glEnd(); break;
	 case GLBINDTEXTURE: 
	    glBindTexture( ip[0], ip[1]  );
	    ip += 2;
	 break;
	 case GLTEXCOORD2F:
	    glTexCoord2fv( fp );
	    fp += 2;
	 break;
	 case GLCOLOR4F: 
	    glColor4fv( fp );
	    fp += 4;
	 break;
	 case GLENABLE: glEnable( *(ip++) ); break;
	 case GLDISABLE: glDisable( *(ip++) ); break;
	 case GLPOLYGONOFFSET: 
	    glPolygonOffset( fp[0], fp[1] );
	    fp += 2;
	 break;
	 case GLPUSHMATRIX: glPushMatrix(); break;
	 case GLTRANSLATEF: 
	    glTranslatef( fp[0], fp[1], fp[2] ); 
	    fp += 3;
	 break;
	 case GLINDEXF: glIndexf( *(fp++) ); break;
	 case GLPOPMATRIX: glPopMatrix(); break;
	 default: gsgDie("Unkown gsglExecute op: %d\n", op );
      }
   }
}
 
extern void glCallList( GLuint list ) {
   if ( NULL == gsgLists || list > gsgLists->count ) {
      gsgErr( GL_INVALID_VALUE );
      return;
   }
   gsglExecute( gsgLists->items[list] );
}



Bool gsglfvc( int op, const GLfloat * v ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   gsglFloats( l, op, 0, v );
   return True;
}

Bool gsglf( int op, GLfloat f ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->floats, GLfloat, f );
   return True;
}   

Bool gsglff( int op, GLfloat f1, GLfloat f2 ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->floats, GLfloat, f1 );
   ARRADD( l->floats, GLfloat, f2 );
   return True;
}   

Bool gsglfff( int op, GLfloat f1, GLfloat f2, GLfloat f3 ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->floats, GLfloat, f1 );
   ARRADD( l->floats, GLfloat, f2 );
   ARRADD( l->floats, GLfloat, f3 );
   return True;
}   

Bool gsglffff( int op, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4 ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->floats, GLfloat, f1 );
   ARRADD( l->floats, GLfloat, f2 );
   ARRADD( l->floats, GLfloat, f3 );
   ARRADD( l->floats, GLfloat, f4 );
   return True;
}   

Bool gsglffffff( int op, GLfloat f1, GLfloat f2, GLfloat f3, 
   GLfloat f4, GLfloat f5, GLfloat f6 ) 
{
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->floats, GLfloat, f1 );
   ARRADD( l->floats, GLfloat, f2 );
   ARRADD( l->floats, GLfloat, f3 );
   ARRADD( l->floats, GLfloat, f4 );
   ARRADD( l->floats, GLfloat, f5 );
   ARRADD( l->floats, GLfloat, f6 );
   return True;
}   

Bool gsglifvc( int op, GLint i, const GLfloat * v ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, i );
   gsglFloats( l, op, i, v );
   return True;
}

Bool gsgliifvc( int op, GLint i1, GLint i2, const GLfloat * v ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, i1 );
   ARRADD( l->ints, GLint, i2 );
   gsglFloats( l, op, i2, v );
   return True;
}

Bool gsgliiipc( int op, GLint i1, GLint i2, GLint i3,
   const GLvoid * v ) 
{
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, i1 );
   ARRADD( l->ints, GLint, i2 );
   ARRADD( l->ints, GLint, i3 );
   gsglBytes( l, op, i2, i3, v );
   return True;
}


Bool gsgliif( int op, GLint i1, GLint i2, GLfloat f ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, i1 );
   ARRADD( l->ints, GLint, i2 );
   ARRADD( l->floats, GLfloat, f );
   return True;
}   

Bool gsgliii( int op, GLint i1, GLint i2, GLint i3 ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, i1 );
   ARRADD( l->ints, GLint, i2 );
   ARRADD( l->ints, GLint, i3 );
   return True;
}

Bool gsglif( int op, GLint i, GLfloat f ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, i );
   ARRADD( l->floats, GLfloat, f );
   return True;
}   

Bool gsgl_( int op ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   return True;
}   

Bool gsgli( int op, GLint i ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, i );
   return True;
}   

Bool gsglii( int op, GLint i1, GLint i2 ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, i1 );
   ARRADD( l->ints, GLint, i2 );
   return True;
}   


Bool gsgliiii( int op, GLint i1, GLint i2, GLint i3, GLint i4 ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, i1 );
   ARRADD( l->ints, GLint, i2 );
   ARRADD( l->ints, GLint, i3 );
   ARRADD( l->ints, GLint, i4 );
   return True;
}

Bool gsgliiiiiiii( int op, GLint i1, GLint i2, GLint i3, GLint i4,
   GLint i5, GLint i6, GLint i7, GLint i8 ) 
{
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, i1 );
   ARRADD( l->ints, GLint, i2 );
   ARRADD( l->ints, GLint, i3 );
   ARRADD( l->ints, GLint, i4 );
   ARRADD( l->ints, GLint, i5 );
   ARRADD( l->ints, GLint, i6 );
   ARRADD( l->ints, GLint, i7 );
   ARRADD( l->ints, GLint, i8 );
   return True;
}

extern GLboolean glIsList( GLuint list ) {
   if (NULL == gsgLists)
      return False;
   if ( list >= gsgLists->count )
      return False;
   return NULL != gsgLists->items[ list ];
}

