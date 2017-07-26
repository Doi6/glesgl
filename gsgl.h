#ifndef GSGLH
#define GSGLH

#include <GL/gl.h>

#define LIST( sign, op, ... ) \
   if ( gsgl##sign( op, ##__VA_ARGS__ )) \
      return;

#define GLMULTMATRIXF   1
#define GLTRANSLATEF    2
#define GLDRAWARRAYS    3 
#define GLLIGHTFV       4
#define GLENABLE        5
#define GLMATRIXMODE    6
#define GLROTATEF       7
#define GLVIEWPORT      8
#define GLCLEAR         9
#define GLMATERIALFV   10
#define GLNORMAL3F     11
#define GLVERTEX3F     12
#define GLSHADEMODEL   13
#define GLLOADIDENTITY 14
#define GLFRUSTUMF     15
#define GLPUSHMATRIX   16
#define GLPOPMATRIX    17
#define GLCLEARCOLOR   18
#define GLORTHOF       19
#define GLCOLOR4F      20
#define GLBEGIN        21
#define GLEND          22
#define GLBINDTEXTURE  23
#define GLBLENDFUNC    24
#define GLDISABLE      25
#define GLPOPATTRIB    26
#define GLPUSHATTRIB   27
#define GLTEXCOORD2F   28
#define GLTEXENVF      29

Bool gsglInList();

Bool gsgl_( int );
Bool gsglb( int, GLbitfield );
Bool gsgle( int, GLenum );
Bool gsglee( int, GLenum, GLenum );
Bool gsgleef( int, GLenum, GLenum, GLfloat );
Bool gsgleefvc( int, GLenum, GLenum, const GLfloat * );
Bool gsgleis( int, GLenum, GLint, GLsizei );
Bool gsgleu( int, GLenum, GLuint );
Bool gsglff( int, GLfloat, GLfloat );
Bool gsglfff( int, GLfloat, GLfloat, GLfloat );
Bool gsglffff( int, GLfloat, GLfloat, GLfloat, GLfloat );
Bool gsglffffff( int, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
Bool gsglfvc( int, const GLfloat * );
Bool gsgliiss( int, GLint, GLint, GLsizei, GLsizei );

#endif // GSGLH
