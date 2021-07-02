//
//  shader.h
//  GL_Anim
//
//  Created by 朱俊凯 on 2019/5/16.
//  Copyright © 2019年 朱俊凯. All rights reserved.
//

#ifndef shader_h
#define shader_h

#include <string>

#include <glad4.3/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


// General purpsoe shader object. Compiles from file, generates
// compile/link-time error messages and hosts several utility
// functions for easy management.
class Shader
{
    public:
    // State
    GLuint ID;
    // Constructor
    Shader() { }
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath,
		const char* varyings[], int count);
    // Sets the current shader as active
    void use();
    Shader  &Use();
    // Compiles the shader from given source code
    void    Compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr); // Note: geometry source code is optional
    // Utility functions
	void    SetBool(const GLchar* name, GLboolean value);
    void    SetFloat    (const GLchar *name, GLfloat value, GLboolean useShader = false);
    void    SetInteger  (const GLchar *name, GLint value, GLboolean useShader = false);
    void    SetVector2f (const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
    void    SetVector2f (const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
    void    SetVector3f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
    void    SetVector3f (const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
    void    SetVector4f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
    void    SetVector4f (const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
    void    SetMatrix4  (const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);
    private:
    // Checks if compilation or linking failed and if so, print the error logs
    void    checkCompileErrors(GLuint object, std::string type);
};

#endif /* shader_h */
