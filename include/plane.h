#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Plane {
public:
    unsigned int VAO;
    unsigned int VBO;
    int vertexCount;

    Plane() {
        float vertices[] = {
            // positions                // normals          // texcoords
            25.0f, -0.5f,  25.0f,       0.0f, 1.0f, 0.0f,   25.0f,  0.0f,
            -25.0f, -0.5f,  25.0f,      0.0f, 1.0f, 0.0f,    0.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,      0.0f, 1.0f, 0.0f,    0.0f, 25.0f,

            25.0f, -0.5f,  25.0f,       0.0f, 1.0f, 0.0f,   25.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,      0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
            25.0f, -0.5f, -25.0f,       0.0f, 1.0f, 0.0f,   25.0f, 25.0f
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);

        vertexCount = 6;
    }
    ~Plane() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void Draw() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
        glBindVertexArray(0);
    }
};