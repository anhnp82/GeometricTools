
#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "Shader.h"

// OpenGL Math:
#pragma warning (disable: 4201)
#include "glm-0.9.3.4/glm/glm.hpp"
#include "glm-0.9.3.4/glm/ext.hpp"
#pragma warning (default: 4201)

class ShaderProgram
{
public:
	ShaderProgram( const std::string &vertexShaderName, const std::string &fragmentShaderName )
		:   mObjectName(0),
		mVertexShader(   vertexShaderName,   GL_VERTEX_SHADER   ),
		mFragmentShader( fragmentShaderName, GL_FRAGMENT_SHADER )
	{
		mObjectName = glCreateProgram();
		glAttachShader( mObjectName, mVertexShader.getObjectName() );
		glAttachShader( mObjectName, mFragmentShader.getObjectName() );
		link();
	}

	///*
	ShaderProgram( const std::string &vertexShaderName, 
		const std::string &fragmentShaderName, 
		const std::string &geometryShaderName)
		:   mObjectName(0),
		mVertexShader(   vertexShaderName,   GL_VERTEX_SHADER   ),
		mFragmentShader( fragmentShaderName, GL_FRAGMENT_SHADER ),
		m_geometry_shader( geometryShaderName, GL_GEOMETRY_SHADER )
	{
		mObjectName = glCreateProgram();
		glAttachShader( mObjectName, mVertexShader.getObjectName() );
		glAttachShader( mObjectName, mFragmentShader.getObjectName() );
		glAttachShader( mObjectName, m_geometry_shader.getObjectName() );
		link();
	}
	//*/

    ~ShaderProgram()
    {
        glDeleteProgram(mObjectName);
    }

    inline GLint getUniformLocation      (const std::string& _nameInShader) const { return glGetUniformLocation (mObjectName, _nameInShader.c_str()); }
    inline GLint getAttributeLocation    (const std::string& _nameInShader) const { return glGetAttribLocation  (mObjectName, _nameInShader.c_str()); }
    inline void bindAttributeLocation    (const std::string& _nameInShader, GLuint _location) const { glBindAttribLocation   (mObjectName, _location, _nameInShader.c_str()); }
    inline void use() const { glUseProgram(mObjectName); }

    bool link();

    // int by location
    inline void setUniform (GLint _location, GLint _v)             const { glUniform1i (_location, _v); }
    inline void setUniform (GLint _location, const glm::ivec2& _v) const { glUniform2iv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::ivec3& _v) const { glUniform3iv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::ivec4& _v) const { glUniform4iv(_location, 1, glm::value_ptr(_v)); }

    // unsigned int by location
    inline void setUniform (GLint _location, GLuint _v)            const { glUniform1ui (_location, _v); }
    inline void setUniform (GLint _location, const glm::uvec2& _v) const { glUniform2uiv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::uvec3& _v) const { glUniform3uiv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::uvec4& _v) const { glUniform4uiv(_location, 1, glm::value_ptr(_v)); }

    // float by location
    inline void setUniform (GLint _location, GLfloat _v)          const { glUniform1f (_location, _v); }
    inline void setUniform (GLint _location, const glm::vec2& _v) const { glUniform2fv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::vec3& _v) const { glUniform3fv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::vec4& _v) const { glUniform4fv(_location, 1, glm::value_ptr(_v)); }

    // float matrix by location
    inline void setUniform (GLint _location, const glm::mat2&   _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat2x3& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2x3fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat2x4& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2x4fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat3x2& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3x2fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat3&   _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3fv  (_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat3x4& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3x4fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat4x2& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4x2fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat4x3& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4x3fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat4&   _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4fv  (_location, 1, _transpose, glm::value_ptr(_v)); }

    // normal:
    template <typename T>
    inline void setUniform (const std::string& _nameInShader, T _v) const
    {
        setUniform( getUniformLocation(_nameInShader), _v);
    }

    // normal for matrices with additional transpose parameter
    template <typename T>
    inline void setUniform (const std::string& _nameInShader, T _v, GLboolean _transpose) const
    {
        setUniform( getUniformLocation(_nameInShader), _v, _transpose);
    }

	GLuint getName()
	{
		return mObjectName;
	}

private:
    GLuint mObjectName;
    Shader mVertexShader;
    Shader mFragmentShader;
	Shader m_geometry_shader;
};

#endif
