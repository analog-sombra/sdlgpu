#ifndef __RECT_ELEMENT_HPP__
#define __RECT_ELEMENT_HPP__

#include <memory>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include "shader.hpp"
#include "engine/gpu_utils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
    struct TransformUniform
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };
    struct Vertex
    {
        float x, y, z;    // vec3 position
        float r, g, b, a; // vec4 color
    };

    // struct Transform
    // {
    //     glm::vec3 position{0.0f, 0.0f, 0.0f};
    //     glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    //     glm::vec3 scale{1.0f, 1.0f, 1.0f};
    // };

    // struct Vertex
    // {
    //     float x, y, z;    // vec3 position
    //     float r, g, b, a; // vec4 color
    // };

    class RectElement
    {
    private:
        SDL_GPUDevice *device;
        SDL_Window *window;

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        // Transform transform;

        std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter> vertexBuffer;
        std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter> indexBuffer;
        std::unique_ptr<SDL_GPUGraphicsPipeline, GraphicsPipelineDeleter> graphicsPipeline;

        glm::vec3 cameraPos;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;

    public:
        RectElement(SDL_GPUDevice *gpuDevice, SDL_Window *window);
        ~RectElement();

        void Update();
        void Render(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer);
        void HandleEvents(const SDL_Event &event);
    };
}

#endif // __RECT_ELEMENT_HPP__