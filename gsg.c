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
typedef const GLubyte * (*gsge_uvc)( GLenum );
typedef void (*gsgb)( GLbitfield );
typedef void (*gsge)( GLenum );
typedef void (*gsgeefvc)( GLenum, GLenum, const GLfloat * );
typedef void (*gsgeeivc)( GLenum, GLenum, const GLint * );
typedef void (*gsgeis)( GLenum, GLint, GLsizei );
typedef void (*gsgespc)( GLenum, GLsizei, const GLvoid * );
typedef void (*gsgfff)( GLfloat, GLfloat, GLfloat );
typedef void (*gsgffff)( GLfloat, GLfloat, GLfloat, GLfloat );
typedef void (*gsgffffff)( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
typedef void (*gsgfvc)( const GLfloat * );
typedef void (*gsgiespc)( GLint, GLenum, GLsizei, const GLvoid * );
typedef void (*gsgiiss)(GLint, GLint, GLsizei, GLsizei );

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
   gsgDebug("glVertexPointer (%x)\n", glGetError() );
   FORWARD( iespc, "glVertexPointer", 
      size, type, stride, pointer );
}

extern void glNormalPointer( GLenum type, GLsizei stride, 
   const GLvoid * pointer)
{
   FORWARD( espc, "glNormalPointer", type, stride, pointer );
}

extern void glDrawArrays( GLenum mode, GLint first, GLsizei count) {
   gsgDebug("glDrawArrays %x %d %d (%d)\n", mode, first, count, glGetError() );   
   LIST( eis, GLDRAWARRAYS, mode, first, count );
   FORWARD( eis, "glDrawArrays", mode, first, count );
}

extern void glEnableClientState( GLenum cap ) {
   gsgDebug("glEnableClientState (%x)\n", glGetError() );
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
   LIST( eefvc, GLLIGHTFV, light, pname, params );
   FORWARD( eefvc, "glLightfv", light, pname, params );
}

extern void glEnable( GLenum cap ) {
   LIST( e, GLENABLE, cap )
   FORWARD( e, "glEnable", cap );
}

extern void glMatrixMode( GLenum mode ) {
   LIST( e, GLMATRIXMODE, mode );
   FORWARD( e, "glMatrixMode", mode );
}

extern void glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z ) {
   LIST( ffff, GLROTATEF, angle, x, y, z );
   FORWARD( ffff, "glRotatef", angle, x, y, z );
}

extern void glViewport( GLint x, GLint y, GLsizei width, GLsizei height ) {
   LIST( iiss, GLVIEWPORT, x, y, width, height );
   FORWARD( iiss, "glViewport", x, y, width, height );
}

extern void glClear( GLbitfield mask ) {
   LIST( b, GLCLEAR, mask );
   FORWARD( b, "glClear", mask );
}

extern void glFlush() {
   FORWARD( _, "glFlush" );
}


extern void glMaterialfv( GLenum face, GLenum pname, 
   const GLfloat * params) 
{
   gsgDebug("glMaterialfv %x, %x (%x)\n", face, pname, glGetError() );
   LIST( eefvc, GLMATERIALFV, face, pname, params );
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
   LIST( e, GLSHADEMODEL, mode );
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

extern void glColor3f( GLfloat red, GLfloat green, GLfloat blue ) {
   glColor4f( red, green, blue, 1.0f );
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
