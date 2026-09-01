// #include <iostream>
// #include <SDL3/SDL.h>
// #include <SDL3/SDL_gpu.h>
// #include <SDL3/SDL_init.h>
// #include <SDL3/SDL_video.h>
// #include <SDL3_shadercross/SDL_shadercross.h>
// #include <fmt/core.h>
// #include <fmt/format.h>
// #include "shader.hpp"
// #include <SDL3_image/SDL_image.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>

// struct Transform
// {
//     glm::vec3 position{0.0f, 0.0f, 0.0f};
//     glm::vec3 rotation{0.0f, 0.0f, 0.0f};
//     glm::vec3 scale{1.0f, 1.0f, 1.0f};
// };

// // the vertex input layout
// struct Vertex
// {
//     float x, y, z; // vec3 position
//     float u, v;    // texture coordinates
// };

// // a list of vertices
// static Vertex vertices[]{
//     // 4 vertives for rect
//     {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f}, // bottom left
//     {0.5f, -0.5f, 0.0f, 1.0f, 0.0f},  // bottom right
//     {0.5f, 0.5f, 0.0f, 1.0f, 1.0f},   // top right
//     {-0.5f, 0.5f, 0.0f, 0.0f, 1.0f}   // top left
// };

// static uint32_t indices[] = {
//     0, 1, 2,
//     3, 0, 2};

// int main()
// {
// #pragma region Initialization
//     /* 1. Initialize SDL */
//     if (!SDL_Init(SDL_INIT_VIDEO))
//     {
//         SDL_Log("SDL_Init failed: %s", SDL_GetError());
//         return 1;
//     }

//     /* 2. Create a window */
//     SDL_Window *window = SDL_CreateWindow(
//         "My App",  /* title */
//         1280, 720, /* width, height */
//         SDL_WINDOW_RESIZABLE);

//     SDL_GPUDevice *device = SDL_CreateGPUDevice(SDL_ShaderCross_GetSPIRVShaderFormats(),
//                                                 true, /* debug_mode */
//                                                 NULL  /* auto-select backend */
//     );

//     if (!device)
//     {
//         SDL_Log("SDL_CreateGPUDevice failed: %s", SDL_GetError());
//         return 1;
//     }

//     /* 4. Claim the window — links the window's swapchain to the device */
//     if (!SDL_ClaimWindowForGPUDevice(device, window))
//     {
//         SDL_Log("SDL_ClaimWindowForGPUDevice failed: %s", SDL_GetError());
//         return 1;
//     }

//     /* Query what backend was chosen: */
//     SDL_Log("GPU driver: %s", SDL_GetGPUDeviceDriver(device));
//     // get all available GPU backends
//     int numDrivers = SDL_GetNumRenderDrivers();
//     for (int i = 0; i < numDrivers; ++i)
//     {
//         SDL_Log(fmt::format("Render Driver {}: {}", i, SDL_GetRenderDriver(i)).c_str());
//     }

//     SDL_GPUShaderFormat formats = SDL_GetGPUShaderFormats(device);
//     if (formats & SDL_GPU_SHADERFORMAT_SPIRV)
//         SDL_Log("Supports SPIR-V");

//     if (formats & SDL_GPU_SHADERFORMAT_DXIL)
//         SDL_Log("Supports DXIL");

//     if (formats & SDL_GPU_SHADERFORMAT_DXBC)
//         SDL_Log("Supports DXBC");

//     if (formats & SDL_GPU_SHADERFORMAT_MSL)
//         SDL_Log("Supports MSL");

//     if (formats & SDL_GPU_SHADERFORMAT_METALLIB)
//         SDL_Log("Supports MetalLib");

// #pragma endregion

// #pragma region triangle

//     struct TransformUniform
//     {
//         glm::mat4 model;
//     };

//     Transform transform;

//     transform.position = {0.0f, 0.0f, 0.0f};
//     transform.rotation = {0.0f, 0.0f, 0.0f};
//     transform.scale = {0.6f, 1.0f, 1.0f};

//     glm::mat4 model = glm::mat4(1.0f);

//     model = glm::translate(
//         model,
//         transform.position);

//     model = glm::rotate(
//         model,
//         glm::radians(transform.rotation.z),
//         glm::vec3(0.0f, 0.0f, 1.0f));

//     model = glm::scale(
//         model,
//         transform.scale);

//     TransformUniform transformData;
//     transformData.model = model;

//     // create the vertex buffer
//     SDL_GPUBufferCreateInfo bufferInfo{};
//     bufferInfo.size = sizeof(vertices);
//     bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
//     SDL_GPUBuffer *vertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

//     // create the index buffer
//     SDL_GPUBufferCreateInfo indexBufferInfo{};
//     indexBufferInfo.size = sizeof(indices);
//     indexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
//     SDL_GPUBuffer *indexBuffer = SDL_CreateGPUBuffer(device, &indexBufferInfo);

//     // creating surface from image data
//     SDL_Surface *imageData = IMG_Load("./assets/bg.jpg");
//     if (imageData == NULL)
//     {
//         SDL_Log("Could not load image data: %s", SDL_GetError());
//     }
//     SDL_FlipSurface(imageData, SDL_FLIP_VERTICAL);

//     SDL_Surface *rgbaSurface =
//         SDL_ConvertSurface(imageData, SDL_PIXELFORMAT_RGBA32);

//     if (rgbaSurface == NULL)
//     {
//         SDL_Log("Could not convert image: %s", SDL_GetError());
//     }

//     // create the GPU texture from the image data
//     SDL_GPUTextureCreateInfo textureInfo{};

//     textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
//     textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
//     textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;

//     textureInfo.width = rgbaSurface->w;
//     textureInfo.height = rgbaSurface->h;

//     textureInfo.layer_count_or_depth = 1;
//     textureInfo.num_levels = 1;
//     textureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;

//     SDL_GPUTexture *texture =
//         SDL_CreateGPUTexture(device, &textureInfo);

//     // create transfer buffers to upload to GPU buffers
//     SDL_GPUTransferBufferCreateInfo vertexTransferInfo{};
//     vertexTransferInfo.size = sizeof(vertices);
//     vertexTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
//     SDL_GPUTransferBuffer *vertexTransferBuffer = SDL_CreateGPUTransferBuffer(device, &vertexTransferInfo);

//     SDL_GPUTransferBufferCreateInfo indexTransferInfo{};
//     indexTransferInfo.size = sizeof(indices);
//     indexTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
//     SDL_GPUTransferBuffer *indexTransferBuffer = SDL_CreateGPUTransferBuffer(device, &indexTransferInfo);

//     SDL_GPUTransferBufferCreateInfo transferInfo{};
//     transferInfo.size = static_cast<Uint32>(rgbaSurface->w * rgbaSurface->h * 4);
//     transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
//     SDL_GPUTransferBuffer *textureTransferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

//     // upload vertex data
//     Vertex *vertexData = (Vertex *)SDL_MapGPUTransferBuffer(device, vertexTransferBuffer, false);
//     SDL_memcpy(vertexData, vertices, sizeof(vertices));
//     SDL_UnmapGPUTransferBuffer(device, vertexTransferBuffer);

//     // upload index data
//     uint32_t *indexData = (uint32_t *)SDL_MapGPUTransferBuffer(device, indexTransferBuffer, false);
//     SDL_memcpy(indexData, indices, sizeof(indices));
//     SDL_UnmapGPUTransferBuffer(device, indexTransferBuffer);

//     // upload texture data
//     void *mapped = SDL_MapGPUTransferBuffer(device, textureTransferBuffer, false);
//     SDL_memcpy(mapped, rgbaSurface->pixels, rgbaSurface->h * rgbaSurface->pitch);
//     SDL_UnmapGPUTransferBuffer(device, textureTransferBuffer);

//     // start a copy pass
//     SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);
//     SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

//     // upload vertex buffer
//     SDL_GPUTransferBufferLocation vertexLocation{};
//     vertexLocation.transfer_buffer = vertexTransferBuffer;
//     vertexLocation.offset = 0;

//     SDL_GPUBufferRegion vertexRegion{};
//     vertexRegion.buffer = vertexBuffer;
//     vertexRegion.size = sizeof(vertices);
//     vertexRegion.offset = 0;

//     SDL_UploadToGPUBuffer(copyPass, &vertexLocation, &vertexRegion, false);

//     // upload index buffer
//     SDL_GPUTransferBufferLocation indexLocation{};
//     indexLocation.transfer_buffer = indexTransferBuffer;
//     indexLocation.offset = 0;

//     SDL_GPUBufferRegion indexRegion{};
//     indexRegion.buffer = indexBuffer;
//     indexRegion.size = sizeof(indices);
//     indexRegion.offset = 0;

//     SDL_UploadToGPUBuffer(copyPass, &indexLocation, &indexRegion, true);

//     SDL_GPUTextureTransferInfo source{};
//     source.transfer_buffer = textureTransferBuffer;
//     source.offset = 0;
//     source.pixels_per_row = rgbaSurface->w;
//     source.rows_per_layer = rgbaSurface->h;

//     SDL_GPUTextureRegion destination{};
//     destination.texture = texture;
//     destination.mip_level = 0;
//     destination.layer = 0;

//     destination.x = 0;
//     destination.y = 0;
//     destination.z = 0;

//     destination.w = rgbaSurface->w;
//     destination.h = rgbaSurface->h;
//     destination.d = 1;
//     SDL_UploadToGPUTexture(copyPass, &source, &destination, false);

//     // end the copy pass
//     SDL_EndGPUCopyPass(copyPass);
//     SDL_SubmitGPUCommandBuffer(commandBuffer);

//     SDL_GPUShader *vertexShader = CreateShader(device, VERTEX_SHADER, "shaders/texturevertex.spv");
//     SDL_GPUShader *fragmentShader = CreateShader(device, FRAGMENT_SHADER, "shaders/texturefragment.spv");

//     // creating the GPU sampler
//     SDL_GPUSamplerCreateInfo samplerInfo{};
//     samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
//     samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;

//     samplerInfo.mipmap_mode =
//         SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;

//     samplerInfo.address_mode_u =
//         SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

//     samplerInfo.address_mode_v =
//         SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

//     samplerInfo.address_mode_w =
//         SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

//     SDL_GPUSampler *sampler =
//         SDL_CreateGPUSampler(device, &samplerInfo);

//     SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

//     // bind shaders
//     pipelineInfo.vertex_shader = vertexShader;
//     pipelineInfo.fragment_shader = fragmentShader;

//     // draw triangles
//     pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

//     // describe the vertex buffers
//     SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1];
//     vertexBufferDesctiptions[0].slot = 0;
//     vertexBufferDesctiptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
//     vertexBufferDesctiptions[0].instance_step_rate = 0;
//     vertexBufferDesctiptions[0].pitch = sizeof(Vertex);

//     pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
//     pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDesctiptions;

//     // describe the vertex attribute
//     SDL_GPUVertexAttribute vertexAttributes[2];

//     // a_position
//     vertexAttributes[0].buffer_slot = 0;                             // fetch data from the buffer at slot 0
//     vertexAttributes[0].location = 0;                                // layout (location = 0) in shader
//     vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; // vec3
//     vertexAttributes[0].offset = 0;                                  // start from the first byte from current buffer position

//     // a_color
//     vertexAttributes[1].buffer_slot = 0;                             // use buffer at slot 0
//     vertexAttributes[1].location = 1;                                // layout (location = 1) in shader
//     vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2; // vec2
//     vertexAttributes[1].offset = sizeof(float) * 3;                  // 4th float from current buffer position

//     pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
//     pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;

//     // describe the color target
//     SDL_GPUColorTargetDescription colorTargetDescriptions[1];
//     colorTargetDescriptions[0] = {};
//     colorTargetDescriptions[0].blend_state.enable_blend = true;
//     colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
//     colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
//     colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
//     colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
//     colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
//     colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
//     colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, window);

//     pipelineInfo.target_info.num_color_targets = 1;
//     pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

//     // create the pipeline
//     SDL_GPUGraphicsPipeline *graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

//     // we don't need to store the shaders after creating the pipeline
//     SDL_ReleaseGPUShader(device, vertexShader);
//     SDL_ReleaseGPUShader(device, fragmentShader);

// #pragma endregion

// #pragma region loop
//     bool running = true;
//     while (running)
//     {
//         SDL_Event event;
//         while (SDL_PollEvent(&event))
//         {
//             if (event.type == SDL_EVENT_QUIT)
//             {
//                 running = false;
//             }
//         }

//         // acquire the command buffer
//         SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);
//         if (commandBuffer == NULL)
//         {
//             SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
//             return -1;
//         }

//         SDL_GPUTexture *swapchainTexture;
//         Uint32 width, height;
//         if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, &width, &height))
//         {
//             SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
//             return -1;
//         }

//         // create the color target
//         SDL_GPUColorTargetInfo colorTargetInfo{};
//         colorTargetInfo.clear_color = {240 / 255.0f, 240 / 255.0f, 240 / 255.0f, 255 / 255.0f};
//         colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
//         colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
//         colorTargetInfo.texture = swapchainTexture;

//         // begin a render pass
//         SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

//         // bind the graphics pipeline
//         SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

//         // bind the vertex buffer
//         SDL_GPUBufferBinding bufferBindings[1];
//         bufferBindings[0].buffer = vertexBuffer; // index 0 is slot 0 in this example
//         bufferBindings[0].offset = 0;            // start from the first byte

//         SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1); // bind one buffer starting from slot 0

//         // bind the index buffer
//         SDL_GPUBufferBinding indexBufferBinding{};
//         indexBufferBinding.buffer = indexBuffer;
//         indexBufferBinding.offset = 0;

//         SDL_BindGPUIndexBuffer(renderPass, &indexBufferBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

//         // bind the sampler
//         SDL_GPUTextureSamplerBinding textureBinding{};
//         textureBinding.texture = texture;
//         textureBinding.sampler = sampler;
//         SDL_BindGPUFragmentSamplers(renderPass, 0, &textureBinding, 1);

//         SDL_PushGPUVertexUniformData(
//             commandBuffer,
//             0,
//             &model,
//             sizeof(model));

//         // issue an indexed draw call (6 indices = 2 triangles for a rectangle)
//         SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);

//         // end the render pass
//         SDL_EndGPURenderPass(renderPass);

//         // submit the command buffer
//         SDL_SubmitGPUCommandBuffer(commandBuffer);
//     }
// #pragma endregion

// #pragma region Cleanup

//     /* Always wait for the GPU to finish before destroying things */
//     SDL_WaitForGPUIdle(device);

//     SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);

//     // release buffers
//     SDL_ReleaseGPUBuffer(device, vertexBuffer);
//     SDL_ReleaseGPUBuffer(device, indexBuffer);
//     SDL_ReleaseGPUTransferBuffer(device, vertexTransferBuffer);
//     SDL_ReleaseGPUTransferBuffer(device, indexTransferBuffer);

//     // free surface if loaded
//     if (imageData != NULL)
//     {
//         SDL_DestroySurface(imageData);
//     }

//     SDL_ReleaseWindowFromGPUDevice(device, window);
//     SDL_DestroyGPUDevice(device);
//     SDL_DestroyWindow(window);
//     SDL_Quit();
// #pragma endregion

//     return EXIT_SUCCESS;
// }

#include "engine/game.hpp"
#include <iostream>
int main()
{
    Engine::Game game("My App");
    game.Run();
    return EXIT_SUCCESS;
}