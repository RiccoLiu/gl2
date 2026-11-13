
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "shader.h"
#include "camera.h"
#include "cube.h"

class Cube;

void framebufferCallback(GLFWwindow* window, int width, int height);
void cursorPositionCallback(GLFWwindow* window, double xposIn, double yposIn);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const * path);
void renderScene(const Shader &shader, const Cube& cube);

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

bool firstMouse = true;
float lastX = SCR_WIDTH * 0.5f;
float lastY = SCR_HEIGHT * 0.5f;

float lastFrame = 0.0f;
float deltaTime = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

class DepthMap {
public:
    unsigned int width;
    unsigned int height;

    unsigned int FBO;
    unsigned int cubeMap;

    DepthMap(int widthIn = 1024, int heightIn = 1024) : width(widthIn), height(heightIn) {
        glGenTextures(1, &cubeMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
        for (int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    ~DepthMap() {
        glDeleteTextures(1, &cubeMap);
        glDeleteFramebuffers(1, &FBO);
    }
};

int main() {
    std::cout << "Hello World! point shadows\n";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "point shadows", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create window..." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD..." << std::endl;
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);

    Shader shader("3.2.point_shadows.vs", "3.2.point_shadows.fs");
    Shader depthShader("3.2.point_shadows_depth.vs", "3.2.point_shadows_depth.fs", "3.2.point_shadows_depth.gs");

    DepthMap depthMap;
    Cube cube;

    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

    unsigned int woodTexture = loadTexture("resources/textures/wood.png");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightPos.z = static_cast<float>(sin(glfwGetTime() * 0.5) * 3.0);

        float nearPlane = 1.0f;
        float farPlane  = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)depthMap.width / (float)depthMap.height, nearPlane, farPlane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));

        glViewport(0, 0, depthMap.width, depthMap.height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMap.FBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        depthShader.use();
        for (unsigned int i = 0; i < 6; ++i)
            depthShader.setMat4("u_ShadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        depthShader.setFloat("u_FarPlane", farPlane);
        depthShader.setVec3("u_LightPos", lightPos);
        renderScene(depthShader, cube);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        shader.use();
        shader.setInt("u_WoodTexture", 0);
        shader.setInt("u_DepthMap", 1); 

        shader.setMat4("u_View", view);
        shader.setMat4("u_Projection", projection);
        shader.setVec3("u_ViewPos", camera.Position);
        shader.setFloat("u_FarPlane", farPlane);

        shader.setVec3("u_Light.position", lightPos);
        shader.setVec3("u_Light.ambient", glm::vec3(0.3f));
        shader.setVec3("u_Light.diffuse", glm::vec3(0.3f));
        shader.setVec3("u_Light.specular", glm::vec3(0.3f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap.cubeMap);

        renderScene(shader, cube);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}

void framebufferCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void cursorPositionCallback(GLFWwindow* window, double xposIn, double yposIn) {
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

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        std::cout << "Close Window..." << std::endl; 
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
    }
}

unsigned int loadTexture(char const * path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        int format = GL_RGB;
        if (nrChannels == 1) {
            format = GL_RED;
        } else if (nrChannels == 3) {
            format = GL_RGB;
        } else if (nrChannels == 4) {
            format = GL_RGBA;
        } 
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

void renderScene(const Shader &shader, const Cube& cube) {
    // room cube
    glm::mat4 model = glm::mat4(1.0f);
#if 1
    model = glm::scale(model, glm::vec3(5.0f));
    shader.setMat4("u_Model", model);
    // glDisable(GL_CULL_FACE); 
    shader.setInt("u_ReverseNormals", 1);

    glBindVertexArray(cube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, cube.vertexCount);
    glBindVertexArray(0);

    shader.setInt("u_ReverseNormals", 0); // and of course disable it
#endif
    // glEnable(GL_CULL_FACE);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("u_Model", model);
    
    glBindVertexArray(cube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, cube.vertexCount);
    glBindVertexArray(0);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.75f));
    shader.setMat4("u_Model", model);

    glBindVertexArray(cube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, cube.vertexCount);
    glBindVertexArray(0);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("u_Model", model);

    glBindVertexArray(cube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, cube.vertexCount);
    glBindVertexArray(0);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("u_Model", model);

    glBindVertexArray(cube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, cube.vertexCount);
    glBindVertexArray(0);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.75f));
    shader.setMat4("u_Model", model);

    glBindVertexArray(cube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, cube.vertexCount);
    glBindVertexArray(0);
}

