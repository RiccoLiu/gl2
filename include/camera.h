#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <utility>

// 注意：保持为普通 enum（非 enum class），旧示例代码使用非限定名 FORWARD 等。
enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

inline constexpr float YAW         = -90.0f;
inline constexpr float PITCH       = 0.0f;
inline constexpr float SPEED       = 1.5f;
inline constexpr float SENSITIVITY = 0.1f;
inline constexpr float ZOOM        = 45.0f;

// 第一人称相机：欧拉角 + WASD/鼠标/滚轮输入。
class Camera
{
public:
    glm::vec3 Position{0.0f, 0.0f, 0.0f};
    glm::vec3 Front{0.0f, 0.0f, -1.0f};
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw              = YAW;
    float Pitch            = PITCH;
    float MovementSpeed    = SPEED;
    float MouseSensitivity = SENSITIVITY;
    float Zoom             = ZOOM;

    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : Position(std::move(position)), WorldUp(std::move(up)), Yaw(yaw), Pitch(pitch)
    {
        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Position(posX, posY, posZ), WorldUp(upX, upY, upZ), Yaw(yaw), Pitch(pitch)
    {
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(CameraMovement direction, float deltaTime)
    {
        const float velocity = MovementSpeed * deltaTime;
        switch (direction)
        {
        case FORWARD: Position += Front * velocity; break;
        case BACKWARD: Position -= Front * velocity; break;
        case LEFT: Position -= Right * velocity; break;
        case RIGHT: Position += Right * velocity; break;
        }
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // 限制俯仰角，避免视线翻转。
        if (constrainPitch)
            Pitch = std::clamp(Pitch, -89.0f, 89.0f);

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        Zoom = std::clamp(Zoom - yoffset, 1.0f, 150.0f);
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = std::cos(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
        front.y = std::sin(glm::radians(Pitch));
        front.z = std::sin(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
