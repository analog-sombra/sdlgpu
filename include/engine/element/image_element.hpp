#ifndef __IMAGE_ELEMENT_HPP__
#define __IMAGE_ELEMENT_HPP__

#include <memory>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include "shader.hpp"

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

    // Custom deleters for SDL objects that require device pointer
    struct GraphicsPipelineDeleter
    {
        SDL_GPUDevice *device = nullptr;
        void operator()(SDL_GPUGraphicsPipeline *pipeline) const
        {
            if (pipeline && device)
                SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        }
    };

    struct GPUBufferDeleter
    {
        SDL_GPUDevice *device = nullptr;
        void operator()(SDL_GPUBuffer *buffer) const
        {
            if (buffer && device)
                SDL_ReleaseGPUBuffer(device, buffer);
        }
    };

    struct GPUTransferBufferDeleter
    {
        SDL_GPUDevice *device = nullptr;
        void operator()(SDL_GPUTransferBuffer *buffer) const
        {
            if (buffer && device)
                SDL_ReleaseGPUTransferBuffer(device, buffer);
        }
    };

    struct GPUTextureDeleter
    {
        SDL_GPUDevice *device = nullptr;
        void operator()(SDL_GPUTexture *texture) const
        {
            if (texture && device)
                SDL_ReleaseGPUTexture(device, texture);
        }
    };

    struct GPUSamplerDeleter
    {
        SDL_GPUDevice *device = nullptr;
        void operator()(SDL_GPUSampler *sampler) const
        {
            if (sampler && device)
                SDL_ReleaseGPUSampler(device, sampler);
        }
    };

    struct Vertex
    {
        float x, y, z; // vec3 position
        float u, v;    // texture coordinates
    };

    class ImageElement
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
        std::unique_ptr<SDL_GPUTransferBuffer, GPUTransferBufferDeleter> textureTransferBuffer;
        std::unique_ptr<SDL_GPUGraphicsPipeline, GraphicsPipelineDeleter> graphicsPipeline;
        std::unique_ptr<SDL_GPUTexture, GPUTextureDeleter> texture;
        std::unique_ptr<SDL_GPUSampler, GPUSamplerDeleter> sampler;

    public:
        ImageElement(SDL_GPUDevice *gpuDevice, SDL_Window *sdlWindow);
        ~ImageElement();

        void Update();
        void Render(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer);
    };
}
#endif // __IMAGE_ELEMENT_HPP__