#ifndef GSGLH
#define GSGLH

#include <GL/gl.h>

#define LIST( sign, op, ... ) \
   if ( gsgl##sign( op, ##__VA_ARGS__ )) \
      return;

#define GLMULTMATRIXF    1
#define GLTRANSLATEF     2
#define GLDRAWARRAYS     3 
#define GLLIGHTFV        4
#define GLENABLE         5
#define GLMATRIXMODE     6
#define GLROTATEF        7
#define GLVIEWPORT       8
#define GLCLEAR          9
#define GLMATERIALFV    10
#define GLNORMAL3F      11
#define GLVERTEX3F      12
#define GLSHADEMODEL    13
#define GLLOADIDENTITY  14
#define GLFRUSTUMF      15
#define GLPUSHMATRIX    16
#define GLPOPMATRIX     17
#define GLCLEARCOLOR    18
#define GLORTHOF        19
#define GLCOLOR4F       20
#define GLBEGIN         21
#define GLEND           22
#define GLBINDTEXTURE   23
#define GLBLENDFUNC     24
#define GLDISABLE       25
#define GLPOPATTRIB     26
#define GLPUSHATTRIB    27
#define GLTEXCOORD2F    28
#define GLTEXENVF       29
#define GLTEXPARAMETERI 30
#define GLLIGHTMODELF   31
#define GLDEPTHFUNC     32
#define GLPOLYGONOFFSET 33
#define GLSTENCILFUNC   34
#define GLSTENCILOP     35
#define GLCOLORMASK     36
#define GLDEPTHMASK     37
#define GLFRONTFACE     38
#define GLSCALEF        39
#define GLCLIPPLANE     40
#define GLCULLFACE      41
#define GLTEXPARAMETERF 42
#define GLSCISSOR       43
#define GLFOGF          44
#define GLFOGFV         45
#define GLHINT          46
#define GLTEXENVI       47
#define GLCOPYTEXSUBIMAGE2D 48
#define GLLIGHTMODELFV  49
#define GLCLEARDEPTHF   50
#define GLMATERIALF     51
#define GLLINEWIDTH     52
#define GLPUSHCLIENTATTRIB 53
#define GLPOPCLIENTATTRIB 54
#define GLCLEARSTENCIL  55
#define GLDRAWELEMENTS  56
#define GLALPHAFUNC     57
#define GLINDEXF        58

Bool gsglInList();

Bool gsgl_( int );
Bool gsglf( int, GLfloat );
Bool gsglff( int, GLfloat, GLfloat );
Bool gsglfff( int, GLfloat, GLfloat, GLfloat );
Bool gsglffff( int, GLfloat, GLfloat, GLfloat, GLfloat );
Bool gsglffffff( int, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
Bool gsglfvc( int, const GLfloat * );
Bool gsgli( int, GLint );
Bool gsglif( int, GLint, GLfloat ); 
Bool gsglifvc( int, GLint, const GLfloat * ); 
Bool gsglii( int, GLint, GLint );
Bool gsgliif( int, GLint, GLint, GLfloat );
Bool gsgliifvc( int, GLint, GLint, const GLfloat * );
Bool gsgliii( int, GLint, GLint, GLint );
Bool gsgliiipc( int, GLint, GLint, GLint, const GLvoid * );
Bool gsgliiii( int, GLint, GLint, GLint, GLint );
Bool gsgliiiiiiii( int, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint );
#endif // GSGLH
