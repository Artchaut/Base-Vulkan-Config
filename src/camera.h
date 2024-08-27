#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#ifndef CAMERA_H

#define CAMERA_H

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;

class Camera {
private:
    glm::vec3 position;

    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float pitch;
    float yaw;

    float movementSpeed;
    float mouseSensitivity;

    auto updateCamera() -> void;

public:
    Camera(glm::vec3 position = glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f), float yaw = YAW, float pitch = PITCH)
        : front(glm::vec3(0.0f, 0.0f, 0.0f))
        , movementSpeed(SPEED)
        , mouseSensitivity(SENSITIVITY)
    {
        position = position;
        worldUp = up;
        yaw = yaw;
        pitch = pitch;

        updateCamera();
    }

    auto getViewMatrix() -> glm::mat4;

    auto processKeyboard(CameraMovement direction, float deltatime) -> void;

    auto processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) -> void;

};

#endif // !CAMERA_H
