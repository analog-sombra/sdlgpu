#ifndef __GAME_SETUP_HPP__
#define __GAME_SETUP_HPP__

#include <memory>
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include "engine/seane/seane_manager.hpp"

using namespace Engine;

class GameSetup
{
public:
    static void InitializeSeanes(
        std::shared_ptr<SeaneManager> seaneManager,
        SDL_GPUDevice *device,
        SDL_Window *window);
};

#endif // __GAME_SETUP_HPP__
