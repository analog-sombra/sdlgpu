#include "engine/camera.hpp"
#include <spdlog/spdlog.h>

using namespace Engine;

Camera::Camera(glm::vec3 startPos, glm::vec3 startFront, glm::vec3 startUp, float moveSpeed, float startZoom)
    : position(startPos),
      front(glm::normalize(startFront)),
      up(glm::normalize(startUp)),
      speed(moveSpeed),
      zoom(startZoom)
{
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

void Camera::HandleInput(const SDL_Event &event)
{
    // Handle mouse wheel for zoom
    if (event.type == SDL_EVENT_MOUSE_WHEEL)
    {
        spdlog::debug("Camera: Zoom");
        ProcessMouseScroll(static_cast<float>(event.wheel.y));
    }

    // Handle mouse movement
    if (event.type == SDL_EVENT_MOUSE_MOTION)
    {
        // Check if right mouse button is held down
        uint32_t mouseState = SDL_GetMouseState(nullptr, nullptr);
        if (mouseState & SDL_BUTTON_RMASK)
        {
            ProcessMouseMovement(event.motion.x, event.motion.y);
        }
    }
}

void Camera::UpdateMovement()
{
    // Check keyboard state for continuous movement (no OS delay)
    int numKeys;
    const bool *keys = SDL_GetKeyboardState(&numKeys);

    if (keys[SDL_SCANCODE_W])
    {
        position += speed * front;
    }
    if (keys[SDL_SCANCODE_S])
    {
        position -= speed * front;
    }
    if (keys[SDL_SCANCODE_A])
    {
        position -= glm::normalize(glm::cross(front, up)) * speed;
    }
    if (keys[SDL_SCANCODE_D])
    {
        position += glm::normalize(glm::cross(front, up)) * speed;
    }
}

void Camera::ProcessMouseScroll(float yoffset)
{
    zoom -= yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void Camera::UpdateFrontFromYawPitch()
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}

void Camera::ProcessMouseMovement(double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = static_cast<float>(xpos - lastX);
    float yoffset = static_cast<float>(lastY - ypos);
    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Clamp pitch to avoid gimbal lock
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Update front vector based on new yaw/pitch
    UpdateFrontFromYawPitch();
}

void Camera::Reset()
{
    position = glm::vec3(0.0f, 0.0f, 3.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    speed = 0.05f;
    zoom = 45.0f;
    yaw = -90.0f;
    pitch = 0.0f;
    firstMouse = true;
}
