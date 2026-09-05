#include "game/seane/test.hpp"


using namespace Engine;
TestSeane::TestSeane(SDL_GPUDevice *device, SDL_Window *window) : Seane(device, window)
{
    Initialize();
}

void TestSeane::Initialize()
{
    if (!camera)
        camera = std::make_unique<Camera>();

    if (!rectElement)
    {
        rectElement = std::make_unique<RectElement>(GetDevice(), GetWindow());
        rectElement->setViewMatrix(camera->GetViewMatrix());
    }
}

TestSeane::~TestSeane()
{
}

void TestSeane::Render(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer)
{
    if (rectElement)
        rectElement->Render(renderPass, commandBuffer);
}

void TestSeane::Update(float deltaTime)
{
    if (camera)
    {
        camera->UpdateMovement();

        // Update all elements with the camera's current view matrix
        if (rectElement)
        {
            rectElement->setViewMatrix(camera->GetViewMatrix());
            // rotate the rectangle element each second using sdl ticks
            float angle = SDL_GetTicks() / 1000.0f; // Get time
            rectElement->setModelMatrix(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)));
            rectElement->Update();
        }
    }
}

void TestSeane::HandleEvents(SDL_Event event)
{
    // Pass input to camera to control the cube
    if (camera)
    {
        camera->HandleInput(event);
    }
}