

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp> 

#include <glm/glm.hpp>
#include <stb_image.h>

#include "shader.h"
#include "camera.h"

void framebufferCallback(GLFWwindow* window, int width, int height);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

unsigned int loadTexture(char const * path);
// unsigned int createVAO();

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

float lastFrame = 0.0;
float deltaTime = 0.0;

bool firstMouse = true;
float lastX = SCR_WIDTH * 0.5;
float lastY = SCR_HEIGHT * 0.5;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool blinn = false;
bool blinnKeyPressed = false;

class Plane {
public:
    unsigned int VAO;
    unsigned int VBO;
    int vertexCount;

    Plane() {
        float planeVertices[] = {
            // positions            // normals         // texcoords
            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
            10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
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
};

int main() {
    std::cout << "Hello World! Advanced Lighting" << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Advanced Lighting", NULL, NULL);
    if (!window) {
        std::cout << "faild to create window..." << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Plane plane;
    unsigned int floorTexture = loadTexture("resources/textures/wood.png");
    
    Shader shader("1.advanced_lighting.vs", "1.advanced_lighting.fs"); 

    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));

        shader.setMat4("u_Model", model);
        shader.setMat4("u_Projection", projection);
        shader.setMat4("u_View", view);
        shader.setMat3("u_NormalMatrix", normalMatrix);

        // set light uniforms
        shader.setInt("u_FloorTexture", 0);
        shader.setVec3("u_ViewPos", camera.Position);
        shader.setVec3("u_LightPos", lightPos);
        shader.setInt("u_Blinn", blinn);

        // floor
        glBindVertexArray(plane.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, plane.vertexCount);

        std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

void framebufferCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void cursorPosCallback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        std::cout << "Close Window.." << std::endl;
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)  {
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)  {
        blinnKeyPressed = false;
    }
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}