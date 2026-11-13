
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "shader.h"
#include "camera.h"
#include "cube.h"
#include "quad.h"
#include "plane.h"

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

bool bloom = false;
bool bloomKeyPressed = false;
float exposure = 1.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

class BloomBuffer {
public:
    unsigned int width;
    unsigned int height;
    
    unsigned int FBO;
    unsigned int RBO;
    unsigned int textureIDs[2]; // 0: 渲染数据 1：发光区域

    BloomBuffer(unsigned int widthIn, unsigned int heightIn) : width(widthIn), height(heightIn) {
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glGenTextures(2, textureIDs);
        for (int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureIDs[i], 0);
        }
        
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
        
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    ~BloomBuffer() {
        glDeleteRenderbuffers(1, &RBO);
        glDeleteTextures(2, textureIDs);
        glDeleteFramebuffers(1, &FBO);
    }
};

class BlurPingpongBuffer {
public:
    unsigned int width;
    unsigned int height;
    
    unsigned int FBO[2];
    unsigned int textureIDs[2];

    BlurPingpongBuffer(unsigned int widthIn, unsigned int heightIn) : width(widthIn), height(heightIn) {
        glGenFramebuffers(2, FBO);
        glGenTextures(2, textureIDs);

        for (int i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO[i]);
            glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIDs[i], 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Framebuffer not complete!" << std::endl;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    ~BlurPingpongBuffer() {
        glDeleteTextures(2, textureIDs);
        glDeleteFramebuffers(2, FBO);
    }
};

class Scene {
public:
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;

    Scene() { 
        lightPositions = {
            glm::vec3( 0.0f, 0.5f,  1.5f),
            glm::vec3(-4.0f, 0.5f, -3.0f),
            glm::vec3( 3.0f, 0.5f,  1.0f),
            glm::vec3(-.8f,  2.4f, -1.0f)
        };
        lightColors = {
            glm::vec3(5.0f,   5.0f,  5.0f),
            glm::vec3(10.0f,  0.0f,  0.0f),
            glm::vec3(0.0f,   0.0f,  15.0f),
            glm::vec3(0.0f,   5.0f,  0.0f)
        };
    }
    ~Scene() {}

    void DrawCubes(Shader& shader, unsigned int woodTexture, unsigned int containerTexture) {
        shader.use();

        shader.setInt("u_RenderLight", false);
        shader.setInt("u_Texture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
        model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
        shader.setMat4("u_Model", model);
        cube_.Draw();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, containerTexture);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        shader.setMat4("u_Model", model);
        cube_.Draw();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
        model = glm::scale(model, glm::vec3(0.5f));
        shader.setMat4("u_Model", model);
        cube_.Draw();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
        model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        shader.setMat4("u_Model", model);
        cube_.Draw();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
        model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model = glm::scale(model, glm::vec3(1.25));
        shader.setMat4("u_Model", model);
        cube_.Draw();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
        model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        shader.setMat4("u_Model", model);
        cube_.Draw();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        shader.setMat4("u_Model", model);
        cube_.Draw();

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void DrawLights(Shader& shader) {
        shader.setInt("u_RenderLight", true);

        for (unsigned int i = 0; i < lightPositions.size(); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(lightPositions[i]));
            model = glm::scale(model, glm::vec3(0.25f));
            shader.setMat4("u_Model", model);
            shader.setVec3("u_LightColor", lightColors[i]);
            cube_.Draw();
        }

    }

private:
    Cube cube_;
};

int main() {
    std::cout << "Hello World! bloom.." << std::endl;

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

    Shader shader("7.1.bloom.vs", "7.1.bloom.fs");
    Shader extractHDRBoomShader("7.1.extract_hdr_bloom.vs", "7.1.extract_hdr_bloom.fs");
    Shader blurShader("7.1.blur.vs", "7.1.blur.fs");

    BloomBuffer bloomBuffer(SCR_WIDTH, SCR_HEIGHT);
    BlurPingpongBuffer blurPingpongBuffer(SCR_WIDTH, SCR_HEIGHT);

    Scene scene;
    Quad quad;

    unsigned int woodTexture      = loadTexture("resources/textures/wood.png", true);
    unsigned int containerTexture = loadTexture("resources/textures/container2.png", true);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);

        // 1. extract bloom regin
        glBindFramebuffer(GL_FRAMEBUFFER, bloomBuffer.FBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        extractHDRBoomShader.use();
        extractHDRBoomShader.setMat4("u_Projection", projection);
        extractHDRBoomShader.setMat4("u_View", view);
        extractHDRBoomShader.setMat4("u_Model", model);
        extractHDRBoomShader.setVec3("u_ViewPos", camera.Position);

        for (int i = 0; i < scene.lightColors.size(); i++) {
            extractHDRBoomShader.setVec3("u_Lights[" + std::to_string(i) + "].Position", scene.lightPositions[i]);
            extractHDRBoomShader.setVec3("u_Lights[" + std::to_string(i) + "].Ambient", glm::vec3(0.0f));
            extractHDRBoomShader.setVec3("u_Lights[" + std::to_string(i) + "].Diffuse", scene.lightColors[i]);
            extractHDRBoomShader.setVec3("u_Lights[" + std::to_string(i) + "].Specular", glm::vec3(0.0f));
        }

        scene.DrawCubes(extractHDRBoomShader, woodTexture, containerTexture);
        scene.DrawLights(extractHDRBoomShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. blur
        bool horizontal = true;
        bool firstIteration = true;
        unsigned int amount = 5;

        blurShader.use();
        blurShader.setInt("u_Image", 0);
        for (int i = 0; i < amount; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, blurPingpongBuffer.FBO[horizontal]); // 输出图像
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, firstIteration ? bloomBuffer.textureIDs[1] : blurPingpongBuffer.textureIDs[!horizontal]); // 输入图像

            blurShader.setInt("u_Horizontal", horizontal);

            quad.Draw();

            horizontal = !horizontal;

            if (firstIteration) {
                firstIteration = false;
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. combine
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setInt("u_DiffuseMap", 0);
        shader.setInt("u_BloomMap", 1);
        shader.setFloat("u_Exposure", exposure);
        shader.setBool("u_Bloom", bloom);

        std::cout << "bloom: " << bloom << std::endl;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bloomBuffer.textureIDs[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, blurPingpongBuffer.textureIDs[!horizontal]);
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

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !bloomKeyPressed) {
        bloom = !bloom;
        bloomKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        bloomKeyPressed = false;
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
