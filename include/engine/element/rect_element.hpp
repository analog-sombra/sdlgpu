#ifndef __RECT_ELEMENT_HPP__
#define __RECT_ELEMENT_HPP__

#include <memory>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include "shader.hpp"
#include "engine/gpu_utils.hpp"
#include "engine/camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
    struct TransformUniform
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };
    struct Vertex
    {
        float x, y, z;    // vec3 position
        float r, g, b, a; // vec4 color
    };

    class RectElement
    {
    private:
        SDL_GPUDevice *device;
        SDL_Window *window;

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;

        std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter> vertexBuffer;
        std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter> indexBuffer;
        std::unique_ptr<SDL_GPUGraphicsPipeline, GraphicsPipelineDeleter> graphicsPipeline;

    public:
        RectElement(SDL_GPUDevice *gpuDevice, SDL_Window *window);
        ~RectElement();

        void Update();
        void Render(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer);
        void HandleEvents(const SDL_Event &event);

        void getModelMatrix(glm::mat4 &outModel) const { outModel = model; }
        void getViewMatrix(glm::mat4 &outView) const { outView = view; }
        void setModelMatrix(const glm::mat4 &inModel) { model = inModel; }
        void setViewMatrix(const glm::mat4 &inView) { view = inView; }
    };
}

#endif // __RECT_ELEMENT_HPP__