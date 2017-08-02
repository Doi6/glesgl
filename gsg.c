#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "gsg.h"
#include "gsga.h"
#include "gsgg.h"
#include "gsgl.h"

// name of the library to load
#define GSG_GLESLIB "GSG_GLESLIB"

#define GSG_SHOWUNSUPP 1

char * gsgGLlib  = "libGLESv1_CM.so";
void * gsgGLlibp = NULL;
GLenum gsgError = GL_NO_ERROR;

gsggVect gsgRasterPos = {0.0f, 0.0f, 0.0f };

ARRDEF( int );
ARR( int ) * gsgAttrs;
ARR( int ) * gsgClientAttrs;

void gsgDie( const char * fmt, ... ) {
   va_list args;
   va_start( args, fmt );
   vfprintf( stderr, fmt, args );
   va_end( args );
   exit(1);
}

extern void gsgUnsupp( const char * fmt, ... ) {
   if ( GSG_SHOWUNSUPP ) {
      va_list args;
      va_start( args, fmt );
      vfprintf( stderr, fmt, args );
      va_end( args );
   }
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
      char * libname = getenv( GSG_GLESLIB );
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

void gsgAttrEnab( GLenum cap ) {
   if ( glIsEnabled( cap ))
      glEnable( cap );
      else glDisable( cap );
}

GLint gsgGeti( GLenum pname ) {
   GLint ret;
   glGetIntegerv( pname, &ret );
   return ret;
}

void gsgPushAttrib( GLenum mask ) {
   if (0 == mask)
      return;
   if ( NULL == gsgAttrs )
      ARRINIT( gsgAttrs, int, 8 );
   int a = glGenLists(1);
   ARRADD( gsgAttrs, int, a );
   glNewList(a,GL_COMPILE);
   if ( GL_LIGHTING_BIT & mask ) {
      gsgAttrEnab( GL_COLOR_MATERIAL );
      gsgAttrEnab( GL_LIGHTING );
      glShadeModel( gsgGeti( GL_SHADE_MODEL ) );
   }
   if ( GL_ENABLE_BIT & mask ) {
      gsgAttrEnab( GL_ALPHA_TEST );
      gsgAttrEnab( GL_AUTO_NORMAL );
      gsgAttrEnab( GL_BLEND );
      gsgAttrEnab( GL_COLOR_MATERIAL );
      gsgAttrEnab( GL_CULL_FACE );
      gsgAttrEnab( GL_DEPTH_TEST );
      gsgAttrEnab( GL_DITHER );
      gsgAttrEnab( GL_FOG );
      gsgAttrEnab( GL_LIGHTING );
      gsgAttrEnab( GL_LINE_SMOOTH );
      gsgAttrEnab( GL_LINE_STIPPLE );
      gsgAttrEnab( GL_COLOR_LOGIC_OP );
      gsgAttrEnab( GL_INDEX_LOGIC_OP );
      gsgAttrEnab( GL_MULTISAMPLE );
      gsgAttrEnab( GL_NORMALIZE );
      gsgAttrEnab( GL_SCISSOR_TEST );
      gsgAttrEnab( GL_STENCIL_TEST );
   }
   if ( ! ((GL_LIGHTING_BIT | GL_ENABLE_BIT) & mask) )
      gsgUnsupp( "gsgPushAttrib %x\n", mask );
   glEndList();
}
   
void gsgPushClientAttrib( GLenum mask ) {
   if (0 == mask)
      return;
   if ( NULL == gsgClientAttrs )
      ARRINIT( gsgClientAttrs, int, 8 );
   int a = glGenLists(1);
   ARRADD( gsgClientAttrs, int, a );
   glNewList(a,GL_COMPILE);
   gsgUnsupp( "gsgPushClientAttrib %x\n", mask );
   glEndList();
}
   
void gsgPopAttrib() {
   if ( NULL == gsgAttrs ) return;
   int n = gsgAttrs->count;
   if (0 == n) return;
   int idx = gsgAttrs->items[ n-1 ];
   glCallList( idx );
   glDeleteLists( idx, 1 );
   -- gsgAttrs->count;
}
   
void gsgPopClientAttrib() {
   if (NULL == gsgClientAttrs ) return;
   int n = gsgClientAttrs->count;
   if ( 0 == n ) return;
   int idx = gsgClientAttrs->items[ n-1 ];
   glCallList( idx );
   glDeleteLists( idx, 1 );
   -- gsgClientAttrs->count;
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

extern void glNormal3dv( const GLdouble * v ) {
   glNormal3f( v[0], v[1], v[2] );
}

extern void glVertex3fv( const GLfloat * v ) {
   glVertex3f( v[0], v[1], v[2] );
}

extern void glVertex3dv( const GLdouble * v ) {
   glVertex3f( v[0], v[1], v[2] );
}

typedef void (*gsg_)();
typedef GLenum (*gsg_e)();
typedef void (*gsgb)( GLboolean );
typedef void (*gsgbbbb)(GLboolean, GLboolean, GLboolean, GLboolean );
typedef void (*gsge)( GLenum );
typedef GLboolean (*gsge_b)( GLenum );
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
typedef void (*gsgeiiiiiss)( GLenum, GLint, GLint, GLint, GLint, GLint,
   GLsizei, GLsizei );
typedef void (*gsgeiissieeovc)(GLenum, GLint, GLint, GLsizei, GLsizei,
   GLint, GLenum, GLenum, const GLvoid * );
typedef void (*gsgeiiisseeovc)(GLenum, GLint, GLint, GLint, GLsizei, 
   GLsizei, GLenum, GLenum, const GLvoid * );
typedef void (*gsgeis)( GLenum, GLint, GLsizei );
typedef void (*gsgeiu)( GLenum, GLint, GLuint );
typedef void (*gsgeiv)( GLenum, GLint * );
typedef void (*gsgesepc)( GLenum, GLsizei, GLenum, const GLvoid * );
typedef void (*gsgespc)( GLenum, GLsizei, const GLvoid * );
typedef void (*gsgeu)( GLenum, GLuint );
typedef void (*gsgf)( GLfloat );
typedef void (*gsgff)( GLfloat, GLfloat );
typedef void (*gsgfff)( GLfloat, GLfloat, GLfloat );
typedef void (*gsgffff)( GLfloat, GLfloat, GLfloat, GLfloat );
typedef void (*gsgffffff)( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
typedef void (*gsgfvc)( const GLfloat * );
typedef void (*gsgi)( GLint );
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

extern void glLightf( GLenum light, GLenum pname, GLfloat param ) {
   glLightfv( light, pname, &param );
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

extern void glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z ) {
   glRotatef( angle, x, y, z );
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


extern void glMaterialf( GLenum face, GLenum pname, GLfloat param) {
   LIST( iif, GLMATERIALF, face, pname, param );
   FORWARD( eef, "glMaterialf", GL_FRONT_AND_BACK, pname, param );
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
 
extern void glNormal3d( GLdouble x, GLdouble y, GLdouble z ) {
   glNormal3f( x, y, z );
}
 
extern void glVertex3f( GLfloat x, GLfloat y, GLfloat z ) {
   LIST( fff, GLVERTEX3F, x, y, z );
   GROUP( fff, Vertex, x, y, z );
}

extern void glVertex3d( GLdouble x, GLdouble y, GLdouble z ) {
   glVertex3f( x, y, z );
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

extern void glClearDepthf( GLfloat depth ) {
   LIST( f, GLCLEARDEPTHF, depth );
   FORWARD( f, "glClearDepthf", depth );
}

extern void glClearDepth( GLdouble depth ) {
   glClearDepthf( depth );
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

extern void glColor4dv( const GLdouble * v ) {
   glColor4f( v[0], v[1], v[2], v[3] );
}

extern void glColor3dv( const GLdouble * v ) {
   glColor4f( v[0], v[1], v[2], 1.0f );
}

extern void glColor3f( GLfloat red, GLfloat green, GLfloat blue ) {
   glColor4f( red, green, blue, 1.0f );
}

extern void glColor3fv( const GLfloat * v ) {
   glColor4f( v[0], v[1], v[2], 1.0f );
}

extern void glColor4ub( GLubyte red, GLubyte green, GLubyte blue,
   GLubyte alpha )
{
   glColor4f( red/255.0f, green/255.0f, blue/255.0f, alpha/255.0f );
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

const GLubyte * gsgGetString( GLenum name ) {
   FORWARD( e_uvc, "glGetString", name );
}

extern const GLubyte * glGetString( GLenum name ) {
   const GLubyte * ret  = gsgGetString( name );
   if (NULL == ret) {
      gsgErr( GL_INVALID_ENUM );
      return (const GLubyte *)"";
   }
   return ret;
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
   gsgPushAttrib( mask );
}

extern void glPopAttrib() {
   LIST( _, GLPOPATTRIB );
   gsgPopAttrib();
}

extern void glPushClientAttrib( GLbitfield mask ) {
   LIST( i, GLPUSHCLIENTATTRIB, mask );
   gsgPushClientAttrib( mask );
}

extern void glPopClientAttrib() {
   LIST( _, GLPOPCLIENTATTRIB );
   gsgPopClientAttrib();
}

extern void glTexCoord2fv( const GLfloat * v ) {
   glTexCoord2f( v[0], v[1] );
}

extern void glTexCoord2f( GLfloat s, GLfloat t ) {
   GROUP( fff, Tex, s, t, 0.0f );
   LIST( ff, GLTEXCOORD2F, s, t );
   gsgDie( "glTexCoord2f withuot glBegin" );
}

extern void glTexCoord2d( GLdouble s, GLdouble t ) {
   glTexCoord2f( s, t );
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

extern void glTexParameterf( GLenum target, GLenum pname, GLfloat param ) {
   LIST( iif, GLTEXPARAMETERF, target, pname, param );
   FORWARD( eef, "glTexParameterf", target, pname, param );
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

extern void glLightModelfv( GLenum pname, const GLfloat * value ) {
   LIST( ifvc, GLLIGHTMODELFV, pname, value );
   FORWARD( efvc, "glLightModelfv", pname, value );
}

extern void glDepthFunc( GLenum func ) {
   LIST( i, GLDEPTHFUNC, func );
   FORWARD( e, "glDepthFunc", func );
}

extern void glReadBuffer( GLenum mode ) {
   gsgDebug("glReadBuffer %x\n", mode );
   gsgErr( GL_INVALID_ENUM );
}

Bool gsgGetIntegerv( GLenum pname, GLint * params ) {
   switch (pname) {
      case GL_PACK_ROW_LENGTH:
      case GL_PACK_SKIP_ROWS:
      case GL_PACK_SKIP_PIXELS:
      case GL_UNPACK_ROW_LENGTH: 
      case GL_UNPACK_SKIP_ROWS:
      case GL_UNPACK_SKIP_PIXELS:
         (*params) = 0; 
      break;
      case GL_PACK_LSB_FIRST:
      case GL_PACK_SWAP_BYTES:
      case GL_UNPACK_LSB_FIRST:
      case GL_UNPACK_SWAP_BYTES:
         (*params) = GL_FALSE;
      break;
      default:
         return False;
   }
   return True;
}

extern void glGetIntegerv( GLenum pname, GLint * params ) {
   if ( gsgGetIntegerv( pname, params ) )
      return;
   FORWARD( eiv, "glGetIntegerv", pname, params );
}

extern void glTexGeni( GLenum coord, GLenum pname, GLint param ) {
//   gsgDebug("glTexGeni %x %x %i\n", coord, pname, param );
   gsgErr( GL_INVALID_ENUM );
}

extern void glTexGenfv( GLenum coord, GLenum pname,
   const GLfloat * params )
{
   gsgErr( GL_INVALID_ENUM );
}

extern void glPolygonOffset( GLfloat factor, GLfloat units ) {
   LIST( ff, GLPOLYGONOFFSET, factor, units );
   FORWARD( ff, "glPolygonOffset", factor, units );
}

extern GLboolean glIsTexture( GLuint texture ) {
   FORWARD( u_b, "glIsTexture", texture );
}

extern GLboolean glIsEnabled( GLenum cap ) {
   FORWARD( e_b, "glIsEnabled", cap );
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

extern void glScaled( GLdouble x, GLdouble y, GLdouble z ) {
   glScalef( x, y, z );
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

extern void glFinish() {
   FORWARD( _, "glFinish" );
}

extern void glScissor( GLint x, GLint y, GLsizei width, GLsizei height ) {
   LIST( iiii, GLSCISSOR, x, y, width, height );
   FORWARD( iiss, "glScissor", x, y, width, height );
}

extern void glFogi( GLenum pname, GLint param ) {
   glFogf( pname, param );
}

extern void glFogf( GLenum pname, GLfloat param ) {
   LIST( if, GLFOGF, pname, param );
   FORWARD( ef, "glFogf", pname, param );
}

extern void glFogfv( GLenum pname, const GLfloat * params ) {
   LIST( ifvc, GLFOGFV, pname, params );
   FORWARD( efvc, "glFogfv", pname, params );
}

extern void glHint( GLenum target, GLenum mode ) {
   LIST( ii, GLHINT, target, mode );
   FORWARD( ee, "glHint", target, mode );
}

extern void glTexEnvi( GLenum target, GLenum pname, GLint param ) {
   LIST( iii, GLTEXENVI, target, pname, param );
   FORWARD( eei, "glTexEnvi", target, pname, param );
}

extern void glPolygonMode( GLenum face, GLenum mode ) {
   gsgUnsupp( "glPolygonmode unsupported\n" );
}

extern void glCopyTexSubImage2D( GLenum target, GLint level, 
   GLint xoffset, GLint yoffset, GLint x, GLint y,
   GLsizei width, GLsizei height )
{
   LIST( iiiiiiii, GLCOPYTEXSUBIMAGE2D, target, level, xoffset, yoffset,
      x, y, width, height );
   FORWARD( eiiiiiss, "glCopyTexSubImage2D", target, level, xoffset, 
      yoffset, x, y, width, height );
}

extern void glVertex2fv( const GLfloat * v ) {
   glVertex3f( v[0], v[1], 0.0f );
}

extern void glPolygonStipple( const GLubyte * mask ) {
   gsgUnsupp( "glPolygonstipple\n" );
}

extern void glDrawBuffer( GLenum buf ) {
   gsgUnsupp( "glDrawBuffer %x\n", buf );
}

extern void glLineWidth( GLfloat width ) {
   LIST( f, GLLINEWIDTH, width );
   FORWARD( f, "glLineWidth", width ); 
}

extern void glRasterPos2f( GLfloat x, GLfloat y ) {
   glRasterPos3f( x, y, 0.0f );
}

extern void glRasterPos3f( GLfloat x, GLfloat y, GLfloat z ) {
   gsggVect v = { x, y, z };
   gsgRasterPos = v;
}

extern void glClearStencil( GLint s ) {
   LIST( i, GLCLEARSTENCIL, s );
   FORWARD( i, "glClearStencil", s );
}

extern void glDrawElements( GLenum mode, GLsizei count, GLenum type,
   const GLvoid * indices )
{
   LIST( iiipc, GLDRAWELEMENTS, mode, count, type, indices );
   FORWARD( esepc, "glDrawElements", mode, count, type, indices );
}

extern void glAlphaFunc( GLenum func, GLclampf ref ) {
   LIST( if, GLALPHAFUNC, func, ref );
   FORWARD( ef, "glAlphaFunc", func, ref );
}

extern void glIndexi( GLint c ) {
   glIndexf( c );
}

extern void glIndexf( GLfloat c ) {
   LIST( f, GLINDEXF, c );
   GROUP( f, Index, c );
}

