#include "Tools.h"
#include <conio.h>

GLenum openGLError_( const char *_fileName, const unsigned long _lineNumber )
{
    GLenum currentError = glGetError();
    GLenum lastError    = currentError;

    // OpenGL does not forbit the implementation to stack up more than one error code
    // so we have to check those in a loop:
    while ( currentError != GL_NO_ERROR ) {
        std::cerr << "GL error in file: " << _fileName << " : " << _lineNumber << " - " << openGLErrorString( currentError ) << std::endl;

		_cprintf("GL error in file: %s : %d - %s \n"
			, std::string(_fileName).c_str(), _lineNumber
			, std::string(openGLErrorString( currentError )).c_str() );

        lastError    = currentError;
        currentError = glGetError();
    }

    return lastError; // returns the last real error (in case there was at least one!)
}

// a gluErrorString alternative:
const char* openGLErrorString( GLenum _errorCode )
{
    // Only 3.2+ Core and ES 2.0+ errors, no deprecated strings like stack underflow etc.
    if      (_errorCode == GL_INVALID_ENUM)                  { return "GL_INVALID_ENUM"; }
    else if (_errorCode == GL_INVALID_VALUE)                 { return "GL_INVALID_VALUE"; }
    else if (_errorCode == GL_INVALID_OPERATION)             { return "GL_INVALID_OPERATION"; }
    else if (_errorCode == GL_INVALID_FRAMEBUFFER_OPERATION) { return "GL_INVALID_FRAMEBUFFER_OPERATION"; }
    else if (_errorCode == GL_OUT_OF_MEMORY)                 { return "GL_OUT_OF_MEMORY"; }
    else if (_errorCode == GL_NO_ERROR)                      { return "GL_NO_ERROR"; }
    else {
        return "unknown error";
    }
}

