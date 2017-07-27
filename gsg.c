#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "gsg.h"
#include "gsgg.h"
#include "gsgl.h"

char * gsgGLlib  = "libGLES.so";
// char * gsgGLlib  = "/usr/lib/intel-cdv/libGLES.so";
void * gsgGLlibp = NULL;
GLenum gsgError = GL_NO_ERROR;

void gsgDie( const char * fmt, ... ) {
   va_list args;
   va_start( args, fmt );
   vfprintf( stderr, fmt, args );
   va_end( args );
   exit(1);
}

extern void gsgUnsupp( const char * fmt, ... ) {
/*   va_list args;
   va_start( args, fmt );
   vfprintf( stderr, fmt, args );
   va_end( args );
*/
   gsgErr( GL_INVALID_OPERATION );
}


void gsgDebug( const char * fmt, ... ) {
   va_list args;
   va_start( args, fmt );
   vfprintf( stderr, fmt, args );
   va_end( args );
}

void gsgFree( void * ptr ) {
   free( ptr );
}

inline void gsgErr( GLenum value ) {
   gsgError = value;
}

inline void gsgOk() {
   gsgError = GL_NO_ERROR;
}

void * gsgDl( const char * name ) {
   if ( ! gsgGLlibp ) {
      char * libname = getenv( "GSG_GLESLIB" );
      if (NULL == libname)
         libname = gsgGLlib;
      if ( ! (gsgGLlibp = dlopen( libname, RTLD_LAZY | RTLD_LOCAL )))
         gsgDie("Could not load library: %s\n", dlerror());
   }
   gsgDebug("gsgDl: %s\n", name );
   void * ret;
   if ( ! (ret = dlsym( gsgGLlibp, name )))
      gsgDie("Could not find symbol: %s\n", dlerror());
   return ret;
}

inline float * gsgToMatrixf( const GLdouble * md ) {
   static GLfloat mf[16];
   GLfloat * at = mf;
   int n;
   for (n=16; 0<n; --n)
      *(at++) = *(md++);
   return mf;
}   
   
extern void glMultMatrixd( const GLdouble * md ) {
   glMultMatrixf( gsgToMatrixf( md ) );
}

extern void glTranslated( GLdouble x, GLdouble y, GLdouble z ) {
   glTranslatef( x, y, z );
}

extern void glNormal3fv( const GLfloat * v ) {
   glNormal3f( v[0], v[1], v[2] );
}

extern void glVertex3fv( const GLfloat * v ) {
   glVertex3f( v[0], v[1], v[2] );
}

typedef void (*gsg_)();
typedef GLenum (*gsg_e)();
typedef void (*gsgb)( GLboolean );
typedef void (*gsgbbbb)(GLboolean, GLboolean, GLboolean, GLboolean );
typedef void (*gsge)( GLenum );
typedef const GLubyte * (*gsge_uvc)( GLenum );
typedef void (*gsgee)( GLenum, GLenum );
typedef void (*gsgeee)( GLenum, GLenum, GLenum );
typedef void (*gsgef)( GLenum, GLfloat );
typedef void (*gsgefvc)( GLenum, const GLfloat * );
typedef void (*gsgeef)( GLenum, GLenum, GLfloat );
typedef void (*gsgeei)( GLenum, GLenum, GLint );
typedef void (*gsgeefvc)( GLenum, GLenum, const GLfloat * );
typedef void (*gsgeeivc)( GLenum, GLenum, const GLint * );
typedef void (*gsgei)( GLenum, GLint );
typedef void (*gsgeiissieeovc)(GLenum, GLint, GLint, GLsizei, GLsizei,
   GLint, GLenum, GLenum, const GLvoid * );
typedef void (*gsgeiiisseeovc)(GLenum, GLint, GLint, GLint, GLsizei, 
   GLsizei, GLenum, GLenum, const GLvoid * );
typedef void (*gsgeis)( GLenum, GLint, GLsizei );
typedef void (*gsgeiu)( GLenum, GLint, GLuint );
typedef void (*gsgeiv)( GLenum, GLint * );
typedef void (*gsgespc)( GLenum, GLsizei, const GLvoid * );
typedef void (*gsgeu)( GLenum, GLuint );
typedef void (*gsgff)( GLfloat, GLfloat );
typedef void (*gsgfff)( GLfloat, GLfloat, GLfloat );
typedef void (*gsgffff)( GLfloat, GLfloat, GLfloat, GLfloat );
typedef void (*gsgffffff)( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
typedef void (*gsgfvc)( const GLfloat * );
typedef void (*gsgiespc)( GLint, GLenum, GLsizei, const GLvoid * );
typedef void (*gsgiiss)(GLint, GLint, GLsizei, GLsizei );
typedef void (*gsgm)( GLbitfield );
typedef void (*gsgsuv)(GLsizei, GLuint * );
typedef void (*gsgsuvc)(GLsizei, const GLuint * );
typedef GLboolean (*gsgu_b)( GLuint );
   
#define FORWARD( sign, name, ... )      \
   gsgOk();                             \
   static gsg##sign gsgfw = NULL;       \
   if (NULL == gsgfw)                   \
      gsgfw = (gsg##sign)gsgDl( name ); \
   return (*gsgfw)( __VA_ARGS__ );


extern void glMultMatrixf( const GLfloat * m ) {
   LIST( fvc, GLMULTMATRIXF, m );
   FORWARD( fvc, "glMultMatrixf", m );
}   

extern void glTranslatef( GLfloat x, GLfloat y, GLfloat z ) {
   LIST( fff, GLTRANSLATEF, x, y, z );
   FORWARD( fff, "glTranslatef", x, y, z );
}

extern void glVertexPointer( GLint size, GLenum type, GLsizei stride,
   const GLvoid * pointer )
{
   FORWARD( iespc, "glVertexPointer", 
      size, type, stride, pointer );
}

extern void glNormalPointer( GLenum type, GLsizei stride, 
   const GLvoid * pointer)
{
   FORWARD( espc, "glNormalPointer", type, stride, pointer );
}

extern void glDrawArrays( GLenum mode, GLint first, GLsizei count) {
   LIST( iii, GLDRAWARRAYS, mode, first, count );
   FORWARD( eis, "glDrawArrays", mode, first, count );
}

extern void glEnableClientState( GLenum cap ) {
   FORWARD( e, "glEnableClientState", cap );
}

extern void glDisableClientState( GLenum cap ) {
   FORWARD( e, "glDisableClientState", cap );
}

GLenum gsgGetError() {
   FORWARD( _e, "glGetError" );
}   

extern GLenum glGetError() {
   GLenum ret = gsgGetError();
   if (GL_NO_ERROR != gsgError)
      ret = gsgError;
   gsgError = GL_NO_ERROR;   
   return ret;
}

extern void glLightfv( GLenum light, GLenum pname, 
   const GLfloat * params )
{
   LIST( iifvc, GLLIGHTFV, light, pname, params );
   FORWARD( eefvc, "glLightfv", light, pname, params );
}

extern void glEnable( GLenum cap ) {
   LIST( i, GLENABLE, cap )
   FORWARD( e, "glEnable", cap );
}

extern void glDisable( GLenum cap ) {
   LIST( i, GLDISABLE, cap );
   FORWARD( e, "glDisable", cap );
}

extern void glMatrixMode( GLenum mode ) {
   LIST( i, GLMATRIXMODE, mode );
   FORWARD( e, "glMatrixMode", mode );
}

extern void glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z ) {
   LIST( ffff, GLROTATEF, angle, x, y, z );
   FORWARD( ffff, "glRotatef", angle, x, y, z );
}

extern void glViewport( GLint x, GLint y, GLsizei width, GLsizei height ) {
   LIST( iiii, GLVIEWPORT, x, y, width, height );
   FORWARD( iiss, "glViewport", x, y, width, height );
}

extern void glClear( GLbitfield mask ) {
   LIST( i, GLCLEAR, mask );
   FORWARD( m, "glClear", mask );
}

extern void glFlush() {
   FORWARD( _, "glFlush" );
}


extern void glMaterialfv( GLenum face, GLenum pname, 
   const GLfloat * params) 
{
   LIST( iifvc, GLMATERIALFV, face, pname, params );
   FORWARD( eefvc, "glMaterialfv", GL_FRONT_AND_BACK, pname, params );
}

extern void glNormal3f( GLfloat x, GLfloat y, GLfloat z ) {
   LIST( fff, GLNORMAL3F, x, y, z );
   GROUP( fff, Normal, x, y, z );
   FORWARD( fff, "glNormal3f", x, y, z );
}

extern void glVertex3f( GLfloat x, GLfloat y, GLfloat z ) {
   LIST( fff, GLVERTEX3F, x, y, z );
   GROUP( fff, Vertex, x, y, z );
   gsgDie( "glVertex3f outside group\n" );
}

extern void glShadeModel( GLenum mode ) {
   LIST( i, GLSHADEMODEL, mode );
   FORWARD( e, "glShadeModel", mode );
}

extern void glLoadIdentity() {
   LIST( _, GLLOADIDENTITY );
   FORWARD( _, "glLoadIdentity" );
}

void glFrustumf( GLfloat left, GLfloat right, GLfloat bottom,
   GLfloat top, GLfloat nearval, GLfloat farval )
{
   LIST( ffffff, GLFRUSTUMF, left, right, bottom, top, nearval, farval );
   FORWARD( ffffff, "glFrustumf", left, right, bottom, top, nearval, farval );
}

extern void glFrustum( GLdouble left, GLdouble right, GLdouble bottom,
   GLdouble top, GLdouble nearval, GLdouble farval )
{
   glFrustumf( left, right, bottom, top, nearval, farval );
}

extern void glPushMatrix() {
   LIST( _, GLPUSHMATRIX );
   FORWARD( _, "glPushMatrix" );
}
 
extern void glPopMatrix() {
   LIST( _, GLPOPMATRIX );
   FORWARD( _, "glPopMatrix" );
}

extern void glClearColor( GLclampf red, GLclampf green, 
   GLclampf blue, GLclampf alpha )
{
   LIST( ffff, GLCLEARCOLOR, red, green, blue, alpha );
   FORWARD( ffff, "glClearColor", red, green, blue, alpha );
}

void glOrthof( GLfloat left, GLfloat right, GLfloat bottom,
   GLfloat top, GLfloat nearval, GLfloat farval )
{
   LIST( ffffff, GLORTHOF, left, right, bottom, top, nearval, farval );
   FORWARD( ffffff, "glOrthof", left, right, bottom, top, nearval, farval );
}

extern void glOrtho( GLdouble left, GLdouble right, GLdouble bottom,
   GLdouble top, GLdouble nearval, GLdouble farval )
{
   glOrthof( left, right, bottom, top, nearval, farval );
}

extern void glColor4f( GLfloat red, GLfloat green, GLfloat blue, 
   GLfloat alpha )
{
   LIST( ffff, GLCOLOR4F, red, green, blue, alpha );
   GROUP( ffff, Color, red, green, blue, alpha );
   FORWARD( ffff, "glColor4f", red, green, blue, alpha );
}

extern void glColor4fv( const GLfloat * v ) {
   glColor4f( v[0], v[1], v[2], v[3] );
}

extern void glColor3f( GLfloat red, GLfloat green, GLfloat blue ) {
   glColor4f( red, green, blue, 1.0f );
}

extern void glColor3fv( const GLfloat * v ) {
   glColor4f( v[0], v[1], v[2], 1.0f );
}

extern void glColorPointer( GLint size, GLenum type, GLsizei stride,
   const GLvoid * pointer )
{
   FORWARD( iespc, "glColorPointer",  size, type, stride, pointer );
}

extern void glVertex2f( GLfloat x, GLfloat y ) {
   glVertex3f( x, y, 0.0 );
}

extern void glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) {
   glBegin( GL_QUADS );
   glVertex2f( x1, y1 );
   glVertex2f( x2, y1 );
   glVertex2f( x2, y2 );
   glVertex2f( x1, y2 );
   glEnd();
}

extern const GLubyte * glGetString( GLenum name ) {
   FORWARD( e_uvc, "glGetString", name );
}

extern void glBindTexture( GLenum target, GLuint texture ) {
   LIST( ii, GLBINDTEXTURE, target, texture );
   FORWARD( eu, "glBindTexture", target, texture );
}

extern void glBlendFunc( GLenum sfactor, GLenum dfactor ) {
   LIST( ii, GLBLENDFUNC, sfactor, dfactor );
   FORWARD( eu, "glBlendFunc", sfactor, dfactor );
}

extern void glGenTextures( GLsizei n, GLuint * textures ) {
   FORWARD( suv, "glGenTextures", n, textures );
}

extern void glPixelStorei( GLenum pname, GLint param ) {
   FORWARD( ei, "glPixelStorei", pname, param );
}

extern void glPushAttrib( GLbitfield mask ) {
   LIST( i, GLPUSHATTRIB, mask );
   gsgUnsupp( "glPushAttrib %x\n", mask );
}

extern void glPopAttrib() {
   LIST( _, GLPOPATTRIB );
   gsgUnsupp( "glPopAttrib\n" );
}

extern void glPushClientAttrib( GLbitfield mask ) {
   FORWARD( m, "glPushClientAttrib", mask );
}

extern void glPopClientAttrib() {
   FORWARD( _, "glPopClientAttrib" );
}

extern void glTexCoord2fv( const GLfloat * v ) {
   glTexCoord2f( v[0], v[1] );
}

extern void glTexCoord2f( GLfloat s, GLfloat t ) {
   GROUP( fff, Tex, s, t, 0.0f );
   LIST( ff, GLTEXCOORD2F, s, t );
   gsgDie( "glTexCoord2f withuot glBegin" );
}

extern void glTexCoordPointer( GLint size, GLenum type, GLsizei stride,
   const GLvoid * pointer )
{
   FORWARD( iespc, "glTexCoordPointer", 
      size, type, stride, pointer );
}

extern void glTexEnvf( GLenum target, GLenum pname, GLfloat param ) {
   LIST( iif, GLTEXENVF, target, pname, param );
   FORWARD( eef, "glTexEnvf", target, pname, param );
}

extern void glTexImage2D( GLenum target, GLint level,
   GLint internalFormat, GLsizei width, GLsizei height, GLint border,
   GLenum format, GLenum type, const GLvoid * data )
{
   if ( GL_TEXTURE_2D != target )
      gsgDie("only gl_texture_2d supported\n" );
   if (gsglInList())
      gsgDie("in list teximage2d not supported\n" );
   FORWARD( eiissieeovc, "glTexImage2D", target, level, internalFormat,
      width, height, border, format, type, data );
}
 
extern void glTexSubImage2D( GLenum target, GLint level,
   GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, 
   GLenum format, GLenum type, const GLvoid * data )
{
   if ( GL_TEXTURE_2D != target )
      gsgDie("only gl_texture_2d supported\n" );
   if (gsglInList())
      gsgDie("in list texSubImage2d not supported\n" );
   FORWARD( eiiisseeovc, "glTexSubImage2D", target, level, xoffset,
      yoffset, width, height, format, type, data );
}
 
extern void glTexParameteri( GLenum target, GLenum pname, GLint param ) {
   LIST( iii, GLTEXPARAMETERI, target, pname, param );
   FORWARD( eei, "glTexParameteri", target, pname, param ); 
}

extern void glVertex2i( GLint x, GLint y ) {
   glVertex2f( x, y );
}

extern void glLightModeli( GLenum pname, GLint value ) {
   if (GL_LIGHT_MODEL_TWO_SIDE == pname)
      return glLightModelf( pname, value );
   gsgErr( GL_INVALID_ENUM );
}

extern void glLightModelf( GLenum pname, GLfloat value ) {
   LIST( if, GLLIGHTMODELF, pname, value );
   FORWARD( ef, "glLightModelf", pname, value );
}

extern void glDepthFunc( GLenum func ) {
   LIST( i, GLDEPTHFUNC, func );
   FORWARD( e, "glDepthFunc", func );
}

extern void glReadBuffer( GLenum mode ) {
   gsgDebug("glReadBuffer %x\n", mode );
   gsgErr( GL_INVALID_ENUM );
}

extern void glGetIntegerv( GLenum pname, GLint * params ) {
   FORWARD( eiv, "glGetIntegerv", pname, params );
}

extern void glTexGeni( GLenum coord, GLenum pname, GLint param ) {
   gsgDebug("glTexGeni %x %x %i\n", coord, pname, param );
   gsgErr( GL_INVALID_ENUM );
}

extern void glPolygonOffset( GLfloat factor, GLfloat units ) {
   LIST( ff, GLPOLYGONOFFSET, factor, units );
   FORWARD( ff, "glPolygonOffset", factor, units );
}

extern GLboolean glIsTexture( GLuint texture ) {
   FORWARD( u_b, "glIsTexture", texture );
}

extern void glStencilFunc( GLenum func, GLint ref, GLuint mask ) {
   LIST( iii, GLSTENCILFUNC, func, ref, mask );
   FORWARD( eiu, "glStencilFunc", func, ref, mask );
}

extern void glStencilOp( GLenum fail, GLenum zfail, GLenum zpass ) {
   LIST( iii, GLSTENCILOP, fail, zfail, zpass );
   FORWARD( eee, "glStencilOp", fail, zfail, zpass );
}

extern void glColorMask( GLboolean red, GLboolean green, GLboolean blue,
   GLboolean alpha )
{
   LIST( iiii, GLCOLORMASK, red, green, blue, alpha );
   FORWARD(  bbbb, "glColorMask", red, green, blue, alpha );
}

extern void glDepthMask( GLboolean flag ) {
   LIST( i, GLDEPTHMASK, flag );
   FORWARD( b, "glDepthMask", flag );
}

extern void glFrontFace( GLenum mode ) {
   LIST( i, GLFRONTFACE, mode );
   FORWARD( e, "glFrontFace", mode );
}

extern void glScalef( GLfloat x, GLfloat y, GLfloat z ) {
   LIST( fff, GLSCALEF, x, y, z );
   FORWARD( fff, "glScalef", x, y, z );
}

extern void glClipPlanef( GLenum plane, const GLfloat * eq ) {
   LIST( ifvc, GLCLIPPLANE, plane, eq );
   FORWARD( efvc, "glClipPlanef", plane, eq );
}

extern void glClipPlane( GLenum plane, const GLdouble * eq ) {
   GLfloat v [] = { eq[0], eq[1], eq[2], eq[3] };
   glClipPlanef( plane, v );
}

extern void glCullFace( GLenum mode ) {
   LIST( i, GLCULLFACE, mode );
   FORWARD( e, "glCullFace", mode );
}


extern void glDeleteTextures( GLsizei n, const GLuint * textures ) {
   FORWARD( suvc, "glDeleteTextures", n, textures );
}
