#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>
#include <filesystem>
#include <cstdint>
#include <utility>

#include "Bindable.hpp"

class Shader
{
public:
    static inline std::array SHADER_TYPES = {
        std::make_pair(".vert", GL_VERTEX_SHADER),
        std::make_pair(".vs", GL_VERTEX_SHADER),
        std::make_pair(".frag", GL_FRAGMENT_SHADER),
        std::make_pair(".fs", GL_FRAGMENT_SHADER),
        std::make_pair(".tesc", GL_TESS_CONTROL_SHADER),
        std::make_pair(".tsc", GL_TESS_CONTROL_SHADER),
        std::make_pair(".tese", GL_TESS_EVALUATION_SHADER),
        std::make_pair(".tes", GL_TESS_EVALUATION_SHADER),
        std::make_pair(".comp", GL_COMPUTE_SHADER),
        std::make_pair(".cs", GL_COMPUTE_SHADER),
        std::make_pair(".geom", GL_GEOMETRY_SHADER),
        std::make_pair(".gs", GL_GEOMETRY_SHADER),
    };

    template<typename ... Args>
    Shader(Args &&... args)
    {
        std::array unpack = {compileShader(std::forward<Args>(args))...};
        mId = GLCall(glCreateProgram());
        for(auto i : unpack)
            GLCall(glAttachShader(mId, i));
        GLCall(glLinkProgram(mId));
        for(auto i : unpack)
            GLCall(glDeleteShader(i));
    }

    std::uint32_t compileShader(const std::filesystem::path &path)
    {
        std::uint32_t shader = 0;

        auto type = getShaderType(path);
        if(type == 0)
            return 0;

        std::ifstream shaderCode;
        std::string shaderString;
        shaderCode.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            shaderCode.open(path);
            std::stringstream shaderStream;
            shaderStream << shaderCode.rdbuf();
            shaderString = shaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << '\n';
            return 0;
        }

        const char *shaderCString = shaderString.c_str();
        shader = GLCall(glCreateShader(type));
        GLCall(glShaderSource(shader, 1, &shaderCString, nullptr));
        GLCall(glCompileShader(shader));
        checkCompileErrors(shader, "");


        return shader;
    }

    GLenum getShaderType(const std::filesystem::path &path)
    {
        std::string extension = path.extension().generic_string();
        for(const auto &s : SHADER_TYPES)
            if(extension == s.first)
                return s.second;
        return 0;
    }

    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        glUseProgram(mId); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void set(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(mId, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void set(const std::string &name, std::int32_t value) const
    { 
        glUniform1i(glGetUniformLocation(mId, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void set(const std::string &name, std::uint32_t value) const
    { 
        glUniform1ui(glGetUniformLocation(mId, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void set(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(mId, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void set(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(mId, name.c_str()), 1, &value[0]); 
    }
    void set(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(mId, name.c_str()), x, y); 
    }
    // ------------------------------------------------------------------------
    void set(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(mId, name.c_str()), 1, &value[0]); 
    }
    void set(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(mId, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void set(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(mId, name.c_str()), 1, &value[0]); 
    }
    void set(const std::string &name, float x, float y, float z, float w) 
    { 
        glUniform4f(glGetUniformLocation(mId, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void set(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(mId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void set(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(mId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void set(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(mId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    std::uint32_t mId;
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                throw std::invalid_argument("Could not create shaders.");
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                throw std::invalid_argument("Could not create shaders.");
            }
        }

    }
};
#endif
