#ifndef EXTRA_H
#define EXTRA_H

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#ifndef M_PI
#define M_PI  3.14159265358979
#endif

// Inline functions to help with drawing
inline void glVertex( const vec3& a )
{
    glVertex3fv( value_ptr(a) );
}

inline void glNormal( const vec3& a ) 
{
    glNormal3fv( value_ptr(a) );
}

inline void glLoadMatrix( const mat4& m )
{
    glLoadMatrixf( value_ptr(m) );
}

inline void glMultMatrix( const mat4& m )
{
    glMultMatrixf( value_ptr(m) );
}

#endif
