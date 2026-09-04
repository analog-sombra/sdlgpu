#include "engine/seane/seane_manager.hpp"
using namespace Engine;

SeaneManager::SeaneManager(SDL_GPUDevice *device, SDL_Window *window)
    : device(device), window(window)
{
}

SeaneManager::~SeaneManager()
{
    ClearAllSeanes();
}

void SeaneManager::AddSeane(const std::string &name, std::unique_ptr<Seane> seane, std::shared_ptr<SeaneManager> manager)
{
    if (seanes.find(name) != seanes.end())
    {
        spdlog::warn("Seane '{}' already exists", name);
        return;
    }

    // Set the manager reference in the seane if provided
    if (manager)
    {
        seane->SetSeaneManager(manager);
    }

    seanes[name] = std::move(seane);
}

void SeaneManager::DeleteSeane(const std::string &name)
{
    auto it = seanes.find(name);
    if (it == seanes.end())
    {
        spdlog::warn("Seane '{}' not found", name);
        return;
    }

    seanes.erase(it);
    spdlog::info("Seane '{}' deleted", name);
}

void SeaneManager::ClearAllSeanes()
{
    seanes.clear();

    // Clear the stack
    while (!seaneStack.empty())
    {
        seaneStack.pop();
    }
}

void SeaneManager::PushSeane(const std::string &name)
{
    auto it = seanes.find(name);
    if (it == seanes.end())
    {
        spdlog::warn("Seane '{}' not found", name);
        return;
    }

    seaneStack.push(name);
    spdlog::info("Seane '{}' pushed to stack (stack size: {})", name, (int)seaneStack.size());
}

void SeaneManager::PopSeane()
{
    if (seaneStack.empty())
    {
        spdlog::warn("Seane stack is empty, cannot pop");
        return;
    }

    std::string poppedSeane = seaneStack.top();
    seaneStack.pop();

    if (seaneStack.empty())
    {
        spdlog::info("Seane '{}' popped. Stack is now empty", poppedSeane);
    }
    else
    {
        std::string topSeane = seaneStack.top();
        spdlog::info("Seane '{}' popped. Returned to seane '{}'", poppedSeane, topSeane);
    }
}

Seane *SeaneManager::PeekSeane() const
{
    if (seaneStack.empty())
    {
        return nullptr;
    }

    std::string topSeane = seaneStack.top();
    auto it = seanes.find(topSeane);
    if (it != seanes.end())
    {
        return it->second.get();
    }

    return nullptr;
}

Seane *SeaneManager::GetSeane(const std::string &name) const
{
    auto it = seanes.find(name);
    if (it != seanes.end())
    {
        return it->second.get();
    }
    return nullptr;
}

int SeaneManager::GetStackSize() const
{
    return seaneStack.size();
}

std::vector<std::string> SeaneManager::GetAllSeanes() const
{
    std::vector<std::string> seaneNames;
    for (const auto &pair : seanes)
    {
        seaneNames.push_back(pair.first);
    }
    return seaneNames;
}

std::vector<std::string> SeaneManager::GetAllStackSeanes() const
{
    std::vector<std::string> stackSeanes;

    // Convert stack to vector (bottom to top)
    std::stack<std::string> tempStack = seaneStack;
    while (!tempStack.empty())
    {
        stackSeanes.insert(stackSeanes.begin(), tempStack.top());
        tempStack.pop();
    }

    return stackSeanes;
}

bool SeaneManager::HasSeane(const std::string &name) const
{
    return seanes.find(name) != seanes.end();
}

bool SeaneManager::IsSeaneInStack(const std::string &name) const
{
    std::stack<std::string> tempStack = seaneStack;
    while (!tempStack.empty())
    {
        if (tempStack.top() == name)
        {
            return true;
        }
        tempStack.pop();
    }
    return false;
}

int SeaneManager::GetSeaneCount() const
{
    return seanes.size();
}

void SeaneManager::Render(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer)
{
    // Render all seanes in the stack (bottom to top) for layering effect
    // This allows pause menu to render on top of gameplay
    std::vector<std::string> stackSeanes = GetAllStackSeanes();
    for (const auto &seaneName : stackSeanes)
    {
        auto it = seanes.find(seaneName);
        if (it != seanes.end())
        {
            it->second->Render(renderPass, commandBuffer);
        }
    }
}

void SeaneManager::Update(float deltaTime)
{

    std::vector<std::string> stackSeanes = GetAllStackSeanes();
    for (const auto &seaneName : stackSeanes)
    {
        auto it = seanes.find(seaneName);
        if (it != seanes.end())
        {
            // Update all seanes in the stack, but only if they are not paused
            if (!it->second->IsPaused())
            {
                it->second->Update(deltaTime);
            }
        }
    }
}

void SeaneManager::HandleEvents(SDL_Event event)
{
    std::vector<std::string> stackSeanes = GetAllStackSeanes();
    for (const auto &seaneName : stackSeanes)
    {
        auto it = seanes.find(seaneName);
        if (it != seanes.end())
        {
            // Handle events for all seanes in the stack, but only if they are not paused
            if (!it->second->IsPaused())
            {
                it->second->HandleEvents(event);
            }
        }
    }
}

void SeaneManager::PauseSeane(const std::string &name)
{
    Seane *seane = GetSeane(name);
    if (seane)
    {
        seane->Pause();

        spdlog::info("Seane '{}' paused", name);
    }
    else
    {
        spdlog::warn("Seane '{}' not found, cannot pause", name);
    }
}

void SeaneManager::ResumeSeane(const std::string &name)
{
    Seane *seane = GetSeane(name);
    if (seane)
    {
        seane->Resume();
        spdlog::info("Seane '{}' resumed", name);
    }
    else
    {
        spdlog::warn("Seane '{}' not found, cannot resume", name);
    }
}

void SeaneManager::ResetSeane(const std::string &name)
{
    Seane *seane = GetSeane(name);
    if (seane)
    {
        seane->Reset();
        spdlog::info("Seane '{}' reset", name);
    }
    else
    {
        spdlog::warn("Seane '{}' not found, cannot reset", name);
    }
}

void SeaneManager::PauseCurrentSeane()
{
    Seane *topSeane = PeekSeane();
    if (topSeane)
    {
        topSeane->Pause();
        spdlog::info("Current seane paused");
    }
}

void SeaneManager::ResumeCurrentSeane()
{
    Seane *topSeane = PeekSeane();
    if (topSeane)
    {
        topSeane->Resume();
        spdlog::info("Current seane resumed");
    }
}

void SeaneManager::ResetCurrentSeane()
{
    Seane *topSeane = PeekSeane();
    if (topSeane)
    {
        topSeane->Reset();
        spdlog::info("Current seane reset");
    }
}
