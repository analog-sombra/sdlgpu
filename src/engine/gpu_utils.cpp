#include "engine/gpu_utils.hpp"

namespace Engine
{

    SDL_GPUBuffer *createGpuBufferInfo(
        SDL_GPUDevice *device,
        SDL_GPUBufferUsageFlags type,
        Uint32 size)
    {
        SDL_GPUBufferCreateInfo bufferInfo{};
        bufferInfo.size = size;
        bufferInfo.usage = type;
        return SDL_CreateGPUBuffer(device, &bufferInfo);
    }

    SDL_GPUTransferBuffer *createGpuTransferBufferInfo(
        SDL_GPUDevice *device,
        Uint32 size)
    {
        SDL_GPUTransferBufferCreateInfo transferBufferInfo{};
        transferBufferInfo.size = size;
        transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        return SDL_CreateGPUTransferBuffer(device, &transferBufferInfo);
    }

    void uploadBufferData(
        SDL_GPUCopyPass *copyPass,
        SDL_GPUBuffer *buffer,
        SDL_GPUTransferBuffer *transferBuffer,
        Uint32 size)
    {
        // upload vertex buffer
        SDL_GPUTransferBufferLocation vertexLocation{};
        vertexLocation.transfer_buffer = transferBuffer;
        vertexLocation.offset = 0;

        SDL_GPUBufferRegion vertexRegion{};
        vertexRegion.buffer = buffer;
        vertexRegion.size = size;
        vertexRegion.offset = 0;

        SDL_UploadToGPUBuffer(copyPass, &vertexLocation, &vertexRegion, false);
    }

}