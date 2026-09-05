#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL3/SDL.h>

namespace Engine
{
    class Camera
    {
    private:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        float speed;
        float zoom;

        // Mouse look variables
        float yaw = -90.0f;
        float pitch = 0.0f;
        double lastX = 0.0;
        double lastY = 0.0;
        bool firstMouse = true;
        float mouseSensitivity = 0.1f;

        // Helper to update front vector from yaw/pitch
        void UpdateFrontFromYawPitch();

    public:
        Camera(
            glm::vec3 startPos = glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3 startFront = glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3 startUp = glm::vec3(0.0f, 1.0f, 0.0f),
            float moveSpeed = 0.05f,
            float startZoom = 45.0f);

        ~Camera() = default;

        // Get view matrix
        glm::mat4 GetViewMatrix() const;

        // Handle keyboard input
        void HandleInput(const SDL_Event &event);

        // Update continuous input (call every frame for smooth movement)
        void UpdateMovement();

        void ProcessMouseScroll(float yoffset);

        // Handle mouse movement for camera look
        void ProcessMouseMovement(double xpos, double ypos);

        // Getters
        glm::vec3 GetPosition() const
        {
            return position;
        }
        glm::vec3 GetFront() const { return front; }
        glm::vec3 GetUp() const { return up; }
        float GetZoom() const { return zoom; }

        // Setters
        void SetPosition(const glm::vec3 &newPos) { position = newPos; }
        void SetFront(const glm::vec3 &newFront) { front = glm::normalize(newFront); }
        void SetUp(const glm::vec3 &newUp) { up = glm::normalize(newUp); }
        void SetSpeed(float newSpeed) { speed = newSpeed; }
        void SetMouseSensitivity(float sens) { mouseSensitivity = sens; }

        float GetYaw() const { return yaw; }
        float GetPitch() const { return pitch; }

        // Reset to defaults
        void Reset();
    };
}

#endif // __CAMERA_HPP__
