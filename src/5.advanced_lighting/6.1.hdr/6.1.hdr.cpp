#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "shader.h"
#include "camera.h"
#include "quad.h"
#include "cube.h"

void framebufferCallback(GLFWwindow* window, int width, int height);
void cursorPositionCallback(GLFWwindow* window, double xposIn, double yposIn);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

unsigned int loadTexture(const std::string& file, bool gammaCorrection);

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

bool firstMouse = true;
float lastX = SCR_WIDTH * 0.5f;
float lastY = SCR_HEIGHT * 0.5f;

float lastFrame = 0.0f;
float deltaTime = 0.0f;

bool hdr = true;
bool hdrKeyPressed = false;
float exposure = 1.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

class HDRBuffer {
public:
    unsigned int RBO;
    unsigned int FBO;
    unsigned int textureID;
    
    unsigned int width;
    unsigned int height;

    HDRBuffer(unsigned int widthIn, unsigned int heightIn) 
        : width(widthIn), height(heightIn) {
        glGenFramebuffers(1, &FBO);
	    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL); // 使用 16 bit 浮点帧缓冲

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    ~HDRBuffer() {
        glDeleteTextures(1, &textureID);
        glDeleteRenderbuffers(1, &RBO);
        glDeleteFramebuffers(1, &FBO);
    }
};

int main() {
    std::cout << "Hello World! hdr..." << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello World", nullptr, nullptr);
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
    
    Shader hdrShader("6.1.hdr.vs", "6.1.hdr.fs");
    Shader shader("6.1.lighting.vs", "6.1.lighting.fs");

    HDRBuffer hdrBuffer(SCR_WIDTH, SCR_HEIGHT);
    Quad quad;
    Cube cube;

    unsigned int woodTexture = loadTexture("resources/textures/wood.png", true);

    std::vector<glm::vec3> lightPositions;
    lightPositions.push_back(glm::vec3( 0.0f,  0.0f, 49.5f)); // back light
    lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
    lightPositions.push_back(glm::vec3( 0.0f, -1.8f, 4.0f));
    lightPositions.push_back(glm::vec3( 0.8f, -1.7f, 6.0f));

    std::vector<glm::vec3> lightColors;
    lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
    lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
    lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
    lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, hdrBuffer.FBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
        model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f)); // 长隧道

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);

        shader.use();
        shader.setMat4("u_Model", model);
        shader.setMat4("u_Projection", projection);
        shader.setMat4("u_View", view);

        for (unsigned int i = 0; i < lightPositions.size(); i++)         {
            shader.setVec3("u_Lights[" + std::to_string(i) + "].Position", lightPositions[i]);
            shader.setVec3("u_Lights[" + std::to_string(i) + "].Color", lightColors[i]);
        }
        shader.setVec3("u_ViewPos", camera.Position);
        shader.setInt("u_InverseNormals", true);
        shader.setInt("u_DiffuseTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        cube.Draw();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.use();
        hdrShader.setInt("u_HDR", hdr);
        hdrShader.setFloat("u_Exposure", exposure);
        hdrShader.setInt("u_HDRBuffer", 0);

        std::cout << "hdr: " << (hdr ? "on" : "off") << "| exposure: " << exposure << std::endl;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrBuffer.textureID);
        quad.Draw();

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

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !hdrKeyPressed) {
        hdr = !hdr;
        hdrKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        hdrKeyPressed = false;
    }
}

unsigned int loadTexture(const std::string& file, bool gammaCorrection = false) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, channels;
    unsigned char* data = stbi_load(file.c_str(), &width, &height, &channels, 0);
    if (data) {
        GLenum internalFormat;
        GLenum dataFormat;

        if (channels == 1) {
            internalFormat = dataFormat = GL_RED;
        } else if (channels == 3) {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        } else if (channels == 4) {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cout << "Failed to load texture: " << file << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}
