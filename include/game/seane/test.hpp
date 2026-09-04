#ifndef __TEST_SEANE_HPP__
#define __TEST_SEANE_HPP__

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include "engine/seane/seane.hpp"
#include "const.hpp"
#include "engine/seane/seane_manager.hpp"
#include <iostream>
#include "engine/element/rect_element.hpp"
#include "engine/camera.hpp"

using namespace Engine;
class TestSeane : public Seane
{
private:
    std::unique_ptr<RectElement> rectElement;
    std::unique_ptr<Camera> camera;

public:
    TestSeane(SDL_GPUDevice *device, SDL_Window *window);
    ~TestSeane();
    void Initialize() override;
    void Render(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer) override;
    void Update(float deltaTime) override;
    void HandleEvents(SDL_Event event) override;

    Camera *GetCamera() const { return camera.get(); }
};

#endif // __TEST_SEANE_HPP__
