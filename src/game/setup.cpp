#include "game/setup.hpp"
#include "game/seane/test.hpp"

using namespace Engine;
void GameSetup::InitializeSeanes(std::shared_ptr<SeaneManager> seaneManager,
                                 SDL_GPUDevice *device,
                                 SDL_Window *window)
{
    // Create and add all seanes here
    seaneManager->AddSeane(
        "TestSeane",
        std::make_unique<TestSeane>(device, window),
        seaneManager);

     // Push the initial seane to start the game
    seaneManager->PushSeane("TestSeane");
}
