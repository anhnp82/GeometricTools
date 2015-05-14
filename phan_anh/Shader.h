////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Computer Graphics Group RWTH Aachen University         //
// All rights reserved.                                                       //
////////////////////////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include <string>


class Shader
{
public:
    Shader( const std::string &fileName, GLenum type ) : mObjectName(0), mType(type)
    {
        mObjectName = glCreateShader(mType);
        setFromFile(fileName);
    }

	Shader(){}

    ~Shader()
    {
        glDeleteShader(mObjectName);
    }

    inline GLuint getObjectName() const { return mObjectName; }

    bool setFromFile (const std::string &fileName) { return setSource( getFileContent(fileName) ); }
    bool setSource   (const std::string &source  );

private:
    std::string getFileContent( const std::string &fileName);

    GLuint mObjectName;
    GLenum mType;
};
