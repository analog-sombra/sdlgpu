#ifndef __SEANE_HPP__
#define __SEANE_HPP__

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <map>
#include <memory>
namespace Engine
{
    class SeaneManager;

    class Seane
    {
    private:
        SDL_GPUDevice *device;
        SDL_Window *window;
        std::weak_ptr<SeaneManager> seaneManager;
        bool isPaused = false;

    public:
        Seane(SDL_GPUDevice *device, SDL_Window *window) : device(device), window(window), isPaused(false)
        {
        }

        virtual ~Seane() {};

        virtual void Initialize() = 0; // Setup game logic
        virtual void Reset()
        {
            Initialize(); // Reuse same init
        }

        void SetSeaneManager(std::weak_ptr<SeaneManager> manager)
        {
            seaneManager = manager;
        }

        std::shared_ptr<SeaneManager> GetSeaneManager() const
        {
            return seaneManager.lock();
        }

        SDL_GPUDevice *GetDevice() const { return device; }
        SDL_Window *GetWindow() const { return window; }

        virtual void Render(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer) = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void HandleEvents(SDL_Event event) = 0;

        bool IsPaused() const { return isPaused; }
        void Pause() { isPaused = true; }
        void Resume() { isPaused = false; }
    };
}

#endif // __SEANE_HPP__