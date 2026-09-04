#ifndef __GPU_UTILS__
#define __GPU_UTILS__

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <vector>

namespace Engine
{

    // custom deleters
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

    struct GraphicsPipelineDeleter
    {
        SDL_GPUDevice *device = nullptr;
        void operator()(SDL_GPUGraphicsPipeline *pipeline) const
        {
            if (pipeline && device)
                SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        }
    };

    SDL_GPUBuffer *createGpuBufferInfo(
        SDL_GPUDevice *device, SDL_GPUBufferUsageFlags type, Uint32 size);

    SDL_GPUTransferBuffer *createGpuTransferBufferInfo(
        SDL_GPUDevice *device, const void *data, Uint32 size);

    void uploadBufferData(
        SDL_GPUCopyPass *copyPass,
        SDL_GPUBuffer *buffer,
        SDL_GPUTransferBuffer *transferBuffer,
        Uint32 size);

    SDL_GPUColorTargetDescription createColorTargetDescription(
        SDL_GPUDevice *device,
        SDL_Window *window);

    SDL_GPUVertexBufferDescription createVertexBufferDescription(Uint32 size);

    struct CreateVertexAttribute
    {
        SDL_GPUVertexElementFormat format; /**< The size and type of the attribute data. */
        Uint32 offset;
    };

    std::vector<SDL_GPUVertexAttribute> createVertexAttribute(const std::vector<CreateVertexAttribute> &attributes);

    SDL_GPUGraphicsPipeline *createGraphicsPipeline(
        SDL_GPUDevice *device,
        SDL_GPUShader *vertexShader,
        SDL_GPUShader *fragmentShader,
        const std::vector<SDL_GPUVertexBufferDescription> &vertexBufferDescriptions,
        const std::vector<SDL_GPUVertexAttribute> &vertexAttributes,
        SDL_GPUColorTargetDescription colorTargetDescription);

}
#endif // __GPU_UTILS__