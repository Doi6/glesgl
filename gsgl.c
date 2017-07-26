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

Bool gsglInList() {
   return 0 <= gsglCurrent;
}

extern void glNewList( GLuint list, GLenum mode ) {
   gsgDebug("glNewList %d %x\n", list, mode );
   if (!(GL_COMPILE == mode || GL_COMPILE_AND_EXECUTE == mode )) {
      gsgErr( GL_INVALID_ENUM );
      return;
   }
   gsgDebug("glNewList b\n" );
   if ( 0 >= list || list > gsgLists->count ) {
      gsgErr( GL_INVALID_VALUE );
      return;
   }
   if ( gsggInGroup() || gsglInList() ) {
      gsgErr( GL_INVALID_OPERATION );
      return;
   }
   gsgDebug("glNewList d\n" );
   gsglCurrent = list;
   gsglMode = mode;
}

extern void glEndList() {
   int save = gsglCurrent;
   gsglCurrent = GSGL_NONE;
   gsgDebug("endlist %d mode:%x\n", gsglInList(), gsglMode );
   if ( GL_COMPILE_AND_EXECUTE == gsglMode )
      glCallList( save );
}

int gsglArrSize( int op, GLenum e ) {
   switch (op) {
      case GLMATERIALFV: 
         switch (e) {
	    case GL_AMBIENT: 
	    case GL_DIFFUSE: 
	    case GL_SPECULAR: 
	    case GL_EMISSION: 
	    case GL_AMBIENT_AND_DIFFUSE: 
	       return 4;
	    case GL_COLOR_INDEXES:
	       return 3;
         }
      break;
   }
   gsgDie("Unkonwn ArrSize op:%d\n", op );
   return 0;
}

void gsglFloats( gsgList * l, int op, GLenum e, const GLfloat * v ) {
   int n = gsglArrSize( op, e );
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
	    fp += gsglArrSize( op, ip[1] );
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
	 default: gsgDie("Unkown gsglExecute op: %d\n", op );
      }
   }
}

extern void glCallList( GLuint list ) {
   if ( list > gsgLists->count )
      return;
   gsglExecute( gsgLists->items[list] );
}



Bool gsglfvc( int op, const GLfloat * v ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   gsglFloats( l, op, 0, v );
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

Bool gsgleis( int op, GLenum e, GLint i, GLsizei s ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, e );
   ARRADD( l->ints, GLint, i );
   ARRADD( l->ints, GLint, s );
   return True;
}   

Bool gsgleu( int op, GLenum e, GLuint u ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, e );
   ARRADD( l->ints, GLint, u );
   return True;
}   

Bool gsgleefvc( int op, GLenum e1, GLenum e2, const GLfloat * v ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, e1 );
   ARRADD( l->ints, GLint, e2 );
   gsglFloats( l, op, e2, v );
   return True;
}

Bool gsgle( int op, GLenum e ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, e );
   return True;
}   

Bool gsglee( int op, GLenum e1, GLenum e2 ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, e1 );
   ARRADD( l->ints, GLint, e2 );
   return True;
}   

Bool gsgleef( int op, GLenum e1, GLenum e2, GLfloat f ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, e1 );
   ARRADD( l->ints, GLint, e2 );
   ARRADD( l->floats, GLfloat, f );
   return True;
}   

Bool gsgl_( int op ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   return True;
}   

Bool gsglb( int op, GLbitfield b ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, b );
   return True;
}   

Bool gsgliiss( int op, GLint i1, GLint i2, GLsizei s1, GLsizei s2 ) {
   if ( ! gsglInList() ) return False;
   gsgList * l = gsgLists->items[ gsglCurrent ];
   ARRADD( l->ints, GLint, op );
   ARRADD( l->ints, GLint, i1 );
   ARRADD( l->ints, GLint, i2 );
   ARRADD( l->ints, GLint, s1 );
   ARRADD( l->ints, GLint, s2 );
   return True;
}



