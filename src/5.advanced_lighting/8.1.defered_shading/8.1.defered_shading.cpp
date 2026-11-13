
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include "camera.h"
#include "shader.h"
#include "model.h"
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

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

class GBuffer {
public:
    unsigned int width;
    unsigned int height;

    unsigned int FBO;
    unsigned int RBO;
    unsigned int textureIDs[3]; // Position, Normal, Texture

    GBuffer(unsigned int widthIn, unsigned int heightIn) : width(widthIn), height(heightIn) {
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        
        // Position
        glGenTextures(3, textureIDs);
        glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIDs[0], 0);

        // Normal
        glBindTexture(GL_TEXTURE_2D, textureIDs[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureIDs[1], 0);

        // Texture
        glBindTexture(GL_TEXTURE_2D, textureIDs[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textureIDs[2], 0);

        GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // RBO
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    ~GBuffer() {
        glDeleteTextures(3, textureIDs);
        glDeleteRenderbuffers(1, &RBO);
        glDeleteFramebuffers(1, &FBO);
    }
};

class Scene {
public:
    std::vector<glm::vec3> objectPositions;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    Scene() {
        objectPositions = {
            glm::vec3(-3.0,  -0.5, -3.0),
            glm::vec3( 0.0,  -0.5, -3.0),
            glm::vec3( 3.0,  -0.5, -3.0),
            glm::vec3(-3.0,  -0.5,  0.0),
            glm::vec3( 0.0,  -0.5,  0.0),
            glm::vec3( 3.0,  -0.5,  0.0),
            glm::vec3(-3.0,  -0.5,  3.0),
            glm::vec3( 0.0,  -0.5,  3.0),
            glm::vec3( 3.0,  -0.5,  3.0)
        };

        const unsigned int NR_LIGHTS = 32;
        srand(13);
        for (unsigned int i = 0; i < NR_LIGHTS; i++) {
            float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
            float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
            float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
            lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

            float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            lightColors.push_back(glm::vec3(rColor, gColor, bColor));
        }
    }
};

int main() {
    std::cout << "Hello World! defered shading..." << std::endl;
    
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

    Quad quad;
    Cube cube;
    Scene scene;

    Model backpack("resources/objects/backpack/backpack.obj");

    Shader shader("8.1.defered_shading.vs", "8.1.defered_shading.fs");
    Shader gShader("8.1.g_buffer.vs", "8.1.g_buffer.fs");
    Shader lightBoxShader("8.1.defered_light_box.vs", "8.1.defered_light_box.fs");
    Shader debugShader("8.1.debug.vs", "8.1.debug.fs");

    GBuffer gBuffer(SCR_WIDTH, SCR_HEIGHT);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
   
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBO);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        gShader.use();
        gShader.setMat4("u_View", view);
        gShader.setMat4("u_Projection", projection);
   
        for (size_t i = 0; i < scene.objectPositions.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, scene.objectPositions[i]);
            gShader.setMat4("u_Model", model);
            backpack.draw(gShader);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.setInt("u_PositionMap", 0);
        shader.setInt("u_NormalMap", 1);
        shader.setInt("u_AlbedoSpecMap", 2);
        shader.setVec3("u_ViewPos", camera.Position);

        for (int i = 0; i < scene.lightPositions.size(); i++) {
            glm::vec3 position = scene.lightPositions[i];
            glm::vec3 color = scene.lightColors[i];

            shader.setVec3("u_Lights[" + std::to_string(i) + "].Position", position);
            shader.setVec3("u_Lights[" + std::to_string(i) + "].Ambient", glm::vec3(0.0f));
            shader.setVec3("u_Lights[" + std::to_string(i) + "].Diffuse", color);
            shader.setVec3("u_Lights[" + std::to_string(i) + "].Specular", color);

            const float linear = 0.7f;
            const float quadratic = 1.8f;
            shader.setFloat("u_Lights[" + std::to_string(i) + "].Linear", linear);
            shader.setFloat("u_Lights[" + std::to_string(i) + "].Quadratic", quadratic);

            const float constant = 1.0f;
            const float maxBrightness = std::fmaxf(std::fmaxf(color.r, color.g), color.b);
            float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
            shader.setFloat("u_Lights[" + std::to_string(i) + "].Radius", radius);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffer.textureIDs[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer.textureIDs[1]);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer.textureIDs[2]);

        quad.Draw();
#if 1
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 写到默认的帧缓冲
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif

        lightBoxShader.use();
        lightBoxShader.setMat4("u_View", view);
        lightBoxShader.setMat4("u_Projection", projection);

        for (int i = 0; i < scene.lightPositions.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, scene.lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.125f));
            lightBoxShader.setMat4("u_Model", model);
            lightBoxShader.setVec3("u_LightColor", scene.lightColors[i]);

            cube.Draw();
        }

        // lightBoxShader.setMat4("u_Model", camera.Position);

        // debugShader.use();
        // debugShader.setInt("u_DiffuseMap", 0);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, gBuffer.textureIDs[2]);
        // quad.Draw();

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

