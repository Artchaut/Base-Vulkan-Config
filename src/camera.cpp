#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

auto Camera::updateCamera() -> void
{
    glm::vec3 frontVec;

    frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontVec.y = sin(glm::radians(pitch));
    frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(frontVec);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

auto Camera::processKeyboard(CameraMovement direction, float deltatime) -> void
{
    float velocity = movementSpeed * deltatime;
    switch (direction) {
    case FORWARD:
        position += front * velocity;
        break;
    case BACKWARD:
        position -= front * velocity;
    case LEFT:
        position += right * velocity;
        break;
    case RIGHT:
        position -= right * velocity;
        break;
    }
}

auto Camera::processMouseMovement(float xoffset, float yoffset, bool constraintPitch) -> void
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constraintPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCamera();
}

auto Camera::getViewMatrix() -> glm::mat4
{
    return glm::lookAt(position, position + front, up);
}
