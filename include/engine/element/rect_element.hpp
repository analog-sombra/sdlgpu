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
        glm::mat4 projection;
    };

    struct Transform
    {
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::vec3 rotation{0.0f, 0.0f, 0.0f};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
    };

    struct Vertex
    {
        float x, y, z;    // vec3 position
        float r, g, b, a; // vec4 color
    };

    class RectElement
    {
    private:
        SDL_GPUDevice *device; // Not owned - borrowed from Game
        SDL_Window *window;    // Not owned - borrowed from Game

        glm::mat4 model;
        glm::mat4 projection;
        Transform transform;

        std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter> vertexBuffer;
        std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter> indexBuffer;
        std::unique_ptr<SDL_GPUTransferBuffer, GPUTransferBufferDeleter> vertexTransferBuffer;
        std::unique_ptr<SDL_GPUTransferBuffer, GPUTransferBufferDeleter> indexTransferBuffer;
        std::unique_ptr<SDL_GPUGraphicsPipeline, GraphicsPipelineDeleter> graphicsPipeline;

    public:
        RectElement(SDL_GPUDevice *gpuDevice, SDL_Window *sdlWindow);
        ~RectElement();

        void Update();
        void Render(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer);
    };
}

#endif // __RECT_ELEMENT_HPP__