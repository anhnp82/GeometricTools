////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Computer Graphics Group RWTH Aachen University         //
// All rights reserved.                                                       //
////////////////////////////////////////////////////////////////////////////////

#include "Shader.h"
#include <iostream>
#include <fstream>

#include <conio.h>


bool Shader::setSource(const std::string &source  )
{
    const char *source_c = source.c_str();
    glShaderSource(mObjectName, 1, &source_c, NULL);
    glCompileShader(mObjectName);

    // check for shader compile errors:
    GLint shaderError;
    glGetShaderiv(mObjectName, GL_COMPILE_STATUS, &shaderError);
    if (shaderError != GL_TRUE)
    {
        // yes, errors
        std::cerr << "Shader compile error: " << std::endl;

		_cprintf("Shader compile error: \n");
    }

    // a log gets always printed (could be warnings)
    GLsizei length = 0;
    glGetShaderiv(mObjectName, GL_INFO_LOG_LENGTH, &length);
    if (length > 1)
    {
        // a compile log can get produced even if there were no errors, but warnings!
        GLchar* pInfo = new char[length + 1];
        glGetShaderInfoLog(mObjectName,  length, &length, pInfo);
        std::cerr << "Compile log: " << std::string(pInfo) << std::endl;

		_cprintf("Compile log: %s \n", std::string(pInfo).c_str());

        delete[] pInfo;
    }
    return (shaderError == GL_TRUE);
}

std::string Shader::getFileContent( const std::string &fileName)
{
    std::string line = "";
    std::string fileContent = "";

    std::ifstream fileStream(fileName.c_str(), std::ifstream::in);

    if (fileStream.is_open())
    {
        while (fileStream.good())
        {
            std::getline(fileStream,line);
            fileContent += line + "\n";
        }
        fileStream.close();
    }
    else
    {
        std::cout << "Failed to open file: " << fileName << std::endl;

		_cprintf("Failed to open file: \n");

        return "";
    }
    return fileContent;
}
