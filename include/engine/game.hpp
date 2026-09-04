#ifndef __GAME_HPP__
#define __GAME_HPP__

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3_shadercross/SDL_shadercross.h>

#include <string>
#include <memory>
#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <engine/seane/seane_manager.hpp>
#include "game/setup.hpp"

// #include "engine/element/rect_element.hpp"
// #include "engine/element/image_element.hpp"

#include "const.hpp"

namespace Engine
{
    class Game
    {

        std::shared_ptr<SeaneManager> seaneManager;

    private:
        // Custom deleters for C++ Smart Pointers
        struct SDLWindowDeleter
        {
            void operator()(SDL_Window *w) const
            {
                if (w)
                    SDL_DestroyWindow(w);
            }
        };

        struct SDLGPUDeviceDeleter
        {
            void operator()(SDL_GPUDevice *d) const
            {
                if (d)
                    SDL_DestroyGPUDevice(d);
            }
        };

        struct SDLGPUTextureDeleter
        {
            SDL_GPUDevice *device = nullptr;
            void operator()(SDL_GPUTexture *texture) const
            {
                if (texture && device)
                    SDL_ReleaseGPUTexture(device, texture);
            }
        };

        std::unique_ptr<SDL_Window, SDLWindowDeleter> window;
        std::unique_ptr<SDL_GPUDevice, SDLGPUDeviceDeleter> device;
        std::unique_ptr<SDL_GPUTexture, SDLGPUTextureDeleter> depthTexture;

        bool running = true;
        bool fullscreen = false;

        // Delta time tracking
        Uint64 lastFrameTime = 0;
        float deltaTime = 0.0f;

    public:
        Game(std::string title);
        ~Game();
        void Run();

        void Render();
        void Update();
        void HandleEvents();

        // driver info
        void GetAllRenderDrivers();
        void GetCurrentRenderDriver();

        // shader info
        void GetSupportedShaderFormats();
    };
}

#endif // __GAME_HPP__