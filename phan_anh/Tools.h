////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Computer Graphics Group RWTH Aachen University         //
// All rights reserved.                                                       //
////////////////////////////////////////////////////////////////////////////////

#ifndef TOOLS_HH
#define TOOLS_HH

#include <GL/glew.h>
#include <iostream>

// OpenGL error checks:
const char* openGLErrorString( GLenum _errorCode );

#define openGLError() openGLError_( __FILE__, __LINE__ )
GLenum openGLError_( const char *_fileName, const unsigned long _lineNumber );

//inline GLint getGLTypeSize ( GLenum _type );
inline GLint getGLTypeSize ( GLenum _type )
{
    switch(_type)
    {
        case GL_BYTE:           return sizeof(GLubyte);
        case GL_UNSIGNED_BYTE:  return sizeof(GLbyte);
        case GL_SHORT:          return sizeof(GLshort);
        case GL_UNSIGNED_SHORT: return sizeof(GLushort);
        case GL_INT:            return sizeof(GLint);
        case GL_UNSIGNED_INT:   return sizeof(GLuint);
        case GL_FLOAT:          return sizeof(GLfloat);
        case GL_DOUBLE:         return sizeof(GLdouble);
    }
    return 0;
}



#endif
