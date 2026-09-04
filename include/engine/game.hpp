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
#include "engine/element/rect_element.hpp"
// #include "engine/element/image_element.hpp"

#include "const.hpp"

namespace Engine
{
    class Game
    {

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

        std::unique_ptr<SDL_Window, SDLWindowDeleter> window;
        std::unique_ptr<SDL_GPUDevice, SDLGPUDeviceDeleter> device;

        bool running = true;
        bool fullscreen = false;
        std::unique_ptr<RectElement> rectElement;

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