#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

// GLSL 着色器程序封装。
// - 构造时读取 .vs/.fs(/.gs) 文件并完成编译链接，任何失败都抛出 std::runtime_error。
// - RAII：析构时释放 program 句柄；只可移动，不可拷贝，避免句柄被双重释放。
// - uniform location 按名称缓存，避免每帧重复调用 glGetUniformLocation。
class Shader
{
public:
    explicit Shader(const std::string& vertexPath,
                    const std::string& fragmentPath,
                    const std::string& geometryPath = {})
        : id_(buildProgram(vertexPath, fragmentPath, geometryPath))
    {
    }

    ~Shader()
    {
        if (id_ != 0)
            glDeleteProgram(id_);
    }

    Shader(const Shader&)            = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept : id_(std::exchange(other.id_, 0)) {}

    Shader& operator=(Shader&& other) noexcept
    {
        if (this != &other)
        {
            if (id_ != 0)
                glDeleteProgram(id_);
            id_ = std::exchange(other.id_, 0);
            uniformLocations_.clear();
        }
        return *this;
    }

    GLuint id() const { return id_; }

    void use() const { glUseProgram(id_); }

    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(location(name), static_cast<GLint>(value));
    }
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(location(name), value);
    }
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(location(name), value);
    }
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(location(name), 1, glm::value_ptr(value));
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(location(name), x, y);
    }
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(location(name), 1, glm::value_ptr(value));
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(location(name), x, y, z);
    }
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(location(name), 1, glm::value_ptr(value));
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(location(name), x, y, z, w);
    }
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(location(name), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(location(name), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(mat));
    }

private:
    GLint location(const std::string& name) const
    {
        auto it = uniformLocations_.find(name);
        if (it == uniformLocations_.end())
        {
            it = uniformLocations_.emplace(name, glGetUniformLocation(id_, name.c_str())).first;
        }
        return it->second;
    }

    static std::string readFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file)
            throw std::runtime_error("Shader: failed to open file: " + path);
        std::ostringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }

    static GLuint compile(GLenum type, const std::string& source, const std::string& path)
    {
        GLuint shader = glCreateShader(type);
        const GLchar* code = source.c_str();
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLchar infoLog[1024];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            glDeleteShader(shader);
            throw std::runtime_error("Shader: compile failed [" + path + "]\n" + infoLog);
        }
        return shader;
    }

    static GLuint buildProgram(const std::string& vertexPath,
                               const std::string& fragmentPath,
                               const std::string& geometryPath)
    {
        GLuint vertex = compile(GL_VERTEX_SHADER, readFile(vertexPath), vertexPath);
        GLuint fragment = compile(GL_FRAGMENT_SHADER, readFile(fragmentPath), fragmentPath);
        GLuint geometry = 0;
        if (!geometryPath.empty())
            geometry = compile(GL_GEOMETRY_SHADER, readFile(geometryPath), geometryPath);

        GLuint program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        if (geometry != 0)
            glAttachShader(program, geometry);
        glLinkProgram(program);

        // 着色器对象在链接后即不再需要，无论链接成败都释放。
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometry != 0)
            glDeleteShader(geometry);

        GLint success = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLchar infoLog[1024];
            glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
            glDeleteProgram(program);
            throw std::runtime_error(std::string("Shader: link failed\n") + infoLog);
        }
        return program;
    }

    GLuint id_ = 0;
    mutable std::unordered_map<std::string, GLint> uniformLocations_;
};
