#include "game/seane/test.hpp"

using namespace Engine;
TestSeane::TestSeane(SDL_GPUDevice *device, SDL_Window *window) : Seane(device, window)
{
    Initialize();
}

void TestSeane::Initialize()
{
    if (!rectElement)
        rectElement = std::make_unique<RectElement>(GetDevice(), GetWindow());
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
    if (rectElement)
        rectElement->Update();
}

void TestSeane::HandleEvents(SDL_Event event)
{
    if (rectElement)
        rectElement->HandleEvents(event);
}