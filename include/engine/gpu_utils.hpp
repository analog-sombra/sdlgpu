#ifndef __GPU_UTILS__
#define __GPU_UTILS__

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

namespace Engine
{

    SDL_GPUBuffer *createGpuBufferInfo(
        SDL_GPUDevice *device, SDL_GPUBufferUsageFlags type, Uint32 size);

    SDL_GPUTransferBuffer *createGpuTransferBufferInfo(
        SDL_GPUDevice *device, Uint32 size);

    void updateTransferBuffer(
        SDL_GPUDevice *device,
        SDL_GPUTransferBuffer *transferBuffer,
        const void *data,
        Uint32 size);

    void uploadBufferData(
        SDL_GPUCopyPass *copyPass,
        SDL_GPUBuffer *buffer,
        SDL_GPUTransferBuffer *transferBuffer,
        Uint32 size);

}
#endif // __GPU_UTILS__