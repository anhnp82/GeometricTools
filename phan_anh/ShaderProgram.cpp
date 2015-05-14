////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Computer Graphics Group RWTH Aachen University         //
// All rights reserved.                                                       //
////////////////////////////////////////////////////////////////////////////////

#include "ShaderProgram.h"
#include <conio.h>

bool ShaderProgram::link() {
    glLinkProgram(mObjectName);

    // check for program link errors:
    GLint programError;
    glGetProgramiv(mObjectName, GL_LINK_STATUS, &programError);

    if (programError != GL_TRUE)
    {
        // yes, errors :-(
        std::cerr << "Program could not get linked:" << std::endl;

		_cprintf("Program could not get linked: \n");
    }

    GLsizei length = 0;
    glGetProgramiv(mObjectName, GL_INFO_LOG_LENGTH, &length);
    if (length > 1)
    {
        // error log or warnings:
        GLchar* pInfo = new char[length + 1];
        glGetProgramInfoLog(mObjectName,  length, &length, pInfo);
        std::cout << "Linker log: " << std::string(pInfo) << std::endl;

		_cprintf("Linker log: %s \n", std::string(pInfo).c_str());

        delete[] pInfo;
        return (programError == GL_TRUE); // if the log contains only warnings we return true
    }
    return true;
}

