#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "shader.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

// 注意：Texture 中的 id 只是对 GPU 纹理对象的引用。
// 纹理生命周期由 Model（textures_loaded 缓存）统一管理，Mesh 不负责释放纹理。
struct Texture
{
    unsigned int id = 0;
    std::string type;
    std::string path;
};

// 网格封装：持有 VAO/VBO/EBO，只可移动，不可拷贝。
class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
        : vertices_(std::move(vertices)), indices_(std::move(indices)), textures_(std::move(textures))
    {
        setupMesh();
    }

    ~Mesh() { release(); }

    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept
        : VAO(std::exchange(other.VAO, 0)),
          VBO(std::exchange(other.VBO, 0)),
          EBO(std::exchange(other.EBO, 0)),
          vertices_(std::move(other.vertices_)),
          indices_(std::move(other.indices_)),
          textures_(std::move(other.textures_))
    {
    }

    Mesh& operator=(Mesh&& other) noexcept
    {
        if (this != &other)
        {
            release();
            VAO = std::exchange(other.VAO, 0);
            VBO = std::exchange(other.VBO, 0);
            EBO = std::exchange(other.EBO, 0);
            vertices_ = std::move(other.vertices_);
            indices_ = std::move(other.indices_);
            textures_ = std::move(other.textures_);
        }
        return *this;
    }

    // shader 仅被读取（查询 uniform 位置），不要求可修改。
    void draw(const Shader& shader) const
    {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;

        for (std::size_t i = 0; i < textures_.size(); ++i)
        {
            const Texture& texture = textures_[i];

            std::string number;
            if (texture.type == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (texture.type == "texture_specular")
                number = std::to_string(specularNr++);
            else
            {
                std::cerr << "Error: unknown texture type: " << texture.type << '\n';
                continue;
            }

            glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
            glUniform1i(glGetUniformLocation(shader.id(), (texture.type + number).c_str()),
                        static_cast<GLint>(i));
            glBindTexture(GL_TEXTURE_2D, texture.id);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

private:
    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // 防止空数据时对 vertices_[0] / indices_[0] 取地址触发未定义行为。
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(vertices_.size() * sizeof(Vertex)),
                     vertices_.empty() ? nullptr : vertices_.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(indices_.size() * sizeof(unsigned int)),
                     indices_.empty() ? nullptr : indices_.data(),
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<const void*>(offsetof(Vertex, position)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<const void*>(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<const void*>(offsetof(Vertex, texCoords)));

        glBindVertexArray(0);
    }

    void release() noexcept
    {
        if (VAO != 0)
            glDeleteVertexArrays(1, &VAO);
        if (VBO != 0)
            glDeleteBuffers(1, &VBO);
        if (EBO != 0)
            glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<Texture> textures_;
};
