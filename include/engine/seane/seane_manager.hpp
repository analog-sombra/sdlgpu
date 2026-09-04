#ifndef __SEANE_MANAGER_HPP__
#define __SEANE_MANAGER_HPP__

#include "seane.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <map>
#include <vector>
#include <stack>
#include <algorithm>
#include <spdlog/spdlog.h>
namespace Engine
{
    class SeaneManager
    {
    public:
        SeaneManager(SDL_GPUDevice *device, SDL_Window *window);
        ~SeaneManager();

        // Add and manage seanes
        void AddSeane(const std::string &name, std::unique_ptr<Seane> seane, std::shared_ptr<SeaneManager> manager = nullptr);
        void DeleteSeane(const std::string &name);
        void ClearAllSeanes();

        // Stack-based seane management (for layering like pause menu)
        void PushSeane(const std::string &name);
        void PopSeane();
        Seane *PeekSeane() const;                       // Get top seane without popping
        Seane *GetSeane(const std::string &name) const; // Get a seane by name
        int GetStackSize() const;

        // Utility methods
        std::vector<std::string> GetAllSeanes() const;      // Return all registered seanes
        std::vector<std::string> GetAllStackSeanes() const; // Return all seanes in stack (bottom to top)
        bool HasSeane(const std::string &name) const;
        int GetSeaneCount() const;
        bool IsSeaneInStack(const std::string &name) const;

        void PauseSeane(const std::string &name);
        void ResumeSeane(const std::string &name);
        void ResetSeane(const std::string &name);
        void PauseCurrentSeane();
        void ResumeCurrentSeane();
        void ResetCurrentSeane();

        // Main update/render/handle events
        // Renders ALL seanes in stack (bottom to top)
        void Render(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer);
        // Only updates top seane in stack
        void Update(float deltaTime);
        // Only top seane handles events
        void HandleEvents(SDL_Event event);

    private:
        SDL_GPUDevice *device;
        SDL_Window *window;
        std::map<std::string, std::unique_ptr<Seane>> seanes;
        std::stack<std::string> seaneStack;
    };
}

#endif // __SEANE_MANAGER_HPP__