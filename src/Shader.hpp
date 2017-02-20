/*
 * @file
 * @author Vsevolod (Seva) Ivanov
 * @brief Adapted from http://learnopengl.com
*/

#pragma once

#include <string>
#include <fstream>
#include <sstream>

#include <stdio.h>
  
#include <GL/glew.h>

class Shader
{
    public:
        GLuint ProgramId;
        
        Shader(const GLchar* vertexPath,
               const GLchar* fragmentPath);
        
        void use();
};
