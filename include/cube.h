#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Cube {
public:
    unsigned int VAO;
    unsigned int VBO;
    int vertexCount;
    Cube() {
        float vertices[] = {
            // back face            // normal               // texture
            -1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,    1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,    0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,    1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,    1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,    1.0f,  0.0f,  0.0f,     1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,    1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,    1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,    1.0f,  0.0f,  0.0f,     0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,     0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,     1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,     1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,     1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,     0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,     0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,     0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,    0.0f,  1.0f,  0.0f,     1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,     1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,     1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,     0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,     0.0f, 0.0f  // bottom-left        
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        vertexCount = 36;
    }
    ~Cube() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void Draw() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);
    }
};
