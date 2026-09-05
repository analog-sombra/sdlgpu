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
        rectElement->SetCamera(camera.get());  // Pass camera to RectElement
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
    // Update continuous camera movement (WASD)
    if (camera)
        camera->UpdateMovement();

    if (rectElement)
        rectElement->Update();
}

void TestSeane::HandleEvents(SDL_Event event)
{
    // Pass input to camera to control the cube
    if (camera)
    {
        camera->HandleInput(event);

        // Handle mouse movement for camera look
        // if (event.type == SDL_EVENT_MOUSE_MOTION)
        // {
        //     camera->ProcessMouseMovement(event.motion.x, event.motion.y);
        // }
    }
}