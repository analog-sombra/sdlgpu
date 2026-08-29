#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3_shadercross/SDL_shadercross.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include "shader.hpp"

// the vertex input layout
struct Vertex
{
    float x, y, z;    // vec3 position
    float r, g, b, a; // vec4 color
};

// a list of vertices
static Vertex vertices[]{
    
    // // triangle 1
    // {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},   // top vertex
    // {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}, // bottom left vertex
    // {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},   // bottom right vertex

    // // triangle 2
    // {0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},  // bottom vertex of triangle 2
    // {-0.5f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}, // bottom left vertex of triangle 2
    // {0.5f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f}   // bottom right vertex of triangle 2

    {-0.2f, 0.8f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    {-0.8f, 0.8f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    {-0.5f, 0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},

    {0.2f, -0.8f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    {0.8f, -0.8f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    {0.5f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},


};

int main()
{
#pragma region Initialization
    /* 1. Initialize SDL */
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    /* 2. Create a window */
    SDL_Window *window = SDL_CreateWindow(
        "My App",  /* title */
        1280, 720, /* width, height */
        SDL_WINDOW_RESIZABLE);

    SDL_GPUDevice *device = SDL_CreateGPUDevice(SDL_ShaderCross_GetSPIRVShaderFormats(),
                                                true, /* debug_mode */
                                                NULL  /* auto-select backend */
    );

    if (!device)
    {
        SDL_Log("SDL_CreateGPUDevice failed: %s", SDL_GetError());
        return 1;
    }

    /* 4. Claim the window — links the window's swapchain to the device */
    if (!SDL_ClaimWindowForGPUDevice(device, window))
    {
        SDL_Log("SDL_ClaimWindowForGPUDevice failed: %s", SDL_GetError());
        return 1;
    }

    /* Query what backend was chosen: */
    SDL_Log("GPU driver: %s", SDL_GetGPUDeviceDriver(device));
    // get all available GPU backends
    int numDrivers = SDL_GetNumRenderDrivers();
    for (int i = 0; i < numDrivers; ++i)
    {
        SDL_Log(fmt::format("Render Driver {}: {}", i, SDL_GetRenderDriver(i)).c_str());
    }

    SDL_GPUShaderFormat formats = SDL_GetGPUShaderFormats(device);
    if (formats & SDL_GPU_SHADERFORMAT_SPIRV)
        SDL_Log("Supports SPIR-V");

    if (formats & SDL_GPU_SHADERFORMAT_DXIL)
        SDL_Log("Supports DXIL");

    if (formats & SDL_GPU_SHADERFORMAT_DXBC)
        SDL_Log("Supports DXBC");

    if (formats & SDL_GPU_SHADERFORMAT_MSL)
        SDL_Log("Supports MSL");

    if (formats & SDL_GPU_SHADERFORMAT_METALLIB)
        SDL_Log("Supports MetalLib");

#pragma endregion

#pragma region triangle
    // create the vertex buffer
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = sizeof(vertices);
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    SDL_GPUBuffer *vertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

    // create a transfer buffer to upload to the vertex buffer
    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = sizeof(vertices);
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

    // map the transfer buffer to a pointer
    Vertex *data = (Vertex *)SDL_MapGPUTransferBuffer(device, transferBuffer, false);

    // data[0] = vertices[0];
    // data[1] = vertices[1];
    // data[2] = vertices[2];

    // or you can copy them all in one operation
    SDL_memcpy(data, vertices, sizeof(vertices));

    // unmap the pointer when you are done updating the transfer buffer
    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    // start a copy pass
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    // where is the data
    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = transferBuffer;
    location.offset = 0; // start from the beginning

    // where to upload the data
    SDL_GPUBufferRegion region{};
    region.buffer = vertexBuffer;
    region.size = sizeof(vertices); // size of the data in bytes
    region.offset = 0;              // begin writing from the first vertex

    // upload the data
    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    // end the copy pass
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);

    SDL_GPUShader *vertexShader = CreateShader(device, VERTEX_SHADER, "shaders/vertex.spv");
    SDL_GPUShader *fragmentShader = CreateShader(device, FRAGMENT_SHADER, "shaders/fragment.spv");

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

    // bind shaders
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;

    // draw triangles
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    // describe the vertex buffers
    SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1];
    vertexBufferDesctiptions[0].slot = 0;
    vertexBufferDesctiptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesctiptions[0].instance_step_rate = 0;
    vertexBufferDesctiptions[0].pitch = sizeof(Vertex);

    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDesctiptions;

    // describe the vertex attribute
    SDL_GPUVertexAttribute vertexAttributes[2];

    // a_position
    vertexAttributes[0].buffer_slot = 0;                             // fetch data from the buffer at slot 0
    vertexAttributes[0].location = 0;                                // layout (location = 0) in shader
    vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; // vec3
    vertexAttributes[0].offset = 0;                                  // start from the first byte from current buffer position

    // a_color
    vertexAttributes[1].buffer_slot = 0;                             // use buffer at slot 0
    vertexAttributes[1].location = 1;                                // layout (location = 1) in shader
    vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4; // vec4
    vertexAttributes[1].offset = sizeof(float) * 3;                  // 4th float from current buffer position

    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;

    // describe the color target
    SDL_GPUColorTargetDescription colorTargetDescriptions[1];
    colorTargetDescriptions[0] = {};
    colorTargetDescriptions[0].blend_state.enable_blend = true;
    colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, window);

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

    // create the pipeline
    SDL_GPUGraphicsPipeline *graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

    // we don't need to store the shaders after creating the pipeline
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);

#pragma endregion

#pragma region loop
    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        // acquire the command buffer
        SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);
        if (commandBuffer == NULL)
        {
            SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
            return -1;
        }

        SDL_GPUTexture *swapchainTexture;
        Uint32 width, height;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, &width, &height))
        {
            SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
            return -1;
        }

        // create the color target
        SDL_GPUColorTargetInfo colorTargetInfo{};
        colorTargetInfo.clear_color = {240 / 255.0f, 240 / 255.0f, 240 / 255.0f, 255 / 255.0f};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        colorTargetInfo.texture = swapchainTexture;

        // begin a render pass
        SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

        // bind the graphics pipeline
        SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

        // bind the vertex buffer
        SDL_GPUBufferBinding bufferBindings[1];
        bufferBindings[0].buffer = vertexBuffer; // index 0 is slot 0 in this example
        bufferBindings[0].offset = 0;            // start from the first byte

        SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1); // bind one buffer starting from slot 0

        // issue a draw call
        SDL_DrawGPUPrimitives(renderPass, 6, 1, 0, 0);

        // end the render pass
        SDL_EndGPURenderPass(renderPass);

        // submit the command buffer
        SDL_SubmitGPUCommandBuffer(commandBuffer);
    }
#pragma endregion

#pragma region Cleanup

    /* Always wait for the GPU to finish before destroying things */
    SDL_WaitForGPUIdle(device);

    SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);

    // release buffers
    SDL_ReleaseGPUBuffer(device, vertexBuffer);
    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

    SDL_ReleaseWindowFromGPUDevice(device, window);
    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);
    SDL_Quit();
#pragma endregion

    return EXIT_SUCCESS;
}