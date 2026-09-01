#include "engine/element/rect_element.hpp"

using namespace Engine;

RectElement::RectElement(SDL_GPUDevice *gpuDevice, SDL_Window *sdlWindow)
    : device(gpuDevice), window(sdlWindow)
{
    Initialize(gpuDevice, sdlWindow);
}

void RectElement::Initialize(SDL_GPUDevice *gpuDevice, SDL_Window *sdlWindow)
{
    device = gpuDevice;
    window = sdlWindow;
    Vertex vertices[]{
        // 4 vertives for rect
        {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}, // bottom left
        {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},  // bottom right
        {0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},   // top right
        {-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f}   // top left
    };

    uint32_t indices[] = {
        0, 1, 2,
        3, 0, 2};

    // transform.position = {0.0f, 0.0f, 0.0f};
    // transform.rotation = {0.0f, 0.0f, 0.0f};
    // transform.scale = {1.0f, 1.0f, 1.0f};

    // model matrix will be calculated in Update()

    TransformUniform transformData;
    transformData.model = model;
    transformData.projection = projection;

    // create the vertex buffer
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = sizeof(vertices);
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    // SDL_GPUBuffer *vertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);
    auto vbuf = SDL_CreateGPUBuffer(device, &bufferInfo);
    vertexBuffer = std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter>(vbuf, {device});

    // create the index buffer
    SDL_GPUBufferCreateInfo indexBufferInfo{};
    indexBufferInfo.size = sizeof(indices);
    indexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    // SDL_GPUBuffer *indexBuffer = SDL_CreateGPUBuffer(device, &indexBufferInfo);
    auto ibuf = SDL_CreateGPUBuffer(device, &indexBufferInfo);
    indexBuffer = std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter>(ibuf, {device});

    // create transfer buffers to upload to GPU buffers
    SDL_GPUTransferBufferCreateInfo vertexTransferInfo{};
    vertexTransferInfo.size = sizeof(vertices);
    vertexTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    // SDL_GPUTransferBuffer *vertexTransferBuffer = SDL_CreateGPUTransferBuffer(device, &vertexTransferInfo);
    auto vxbuf = SDL_CreateGPUTransferBuffer(device, &vertexTransferInfo);
    vertexTransferBuffer = std::unique_ptr<SDL_GPUTransferBuffer, GPUTransferBufferDeleter>(vxbuf, {device});

    SDL_GPUTransferBufferCreateInfo indexTransferInfo{};
    indexTransferInfo.size = sizeof(indices);
    indexTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    // SDL_GPUTransferBuffer *indexTransferBuffer = SDL_CreateGPUTransferBuffer(device, &indexTransferInfo);
    auto ixbuf = SDL_CreateGPUTransferBuffer(device, &indexTransferInfo);
    indexTransferBuffer = std::unique_ptr<SDL_GPUTransferBuffer, GPUTransferBufferDeleter>(ixbuf, {device});

    // upload vertex data
    Vertex *vertexData = (Vertex *)SDL_MapGPUTransferBuffer(device, vertexTransferBuffer.get(), false);
    SDL_memcpy(vertexData, vertices, sizeof(vertices));
    SDL_UnmapGPUTransferBuffer(device, vertexTransferBuffer.get());

    // upload index data
    uint32_t *indexData = (uint32_t *)SDL_MapGPUTransferBuffer(device, indexTransferBuffer.get(), false);
    SDL_memcpy(indexData, indices, sizeof(indices));
    SDL_UnmapGPUTransferBuffer(device, indexTransferBuffer.get());

    // start a copy pass
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    // upload vertex buffer
    SDL_GPUTransferBufferLocation vertexLocation{};
    vertexLocation.transfer_buffer = vertexTransferBuffer.get();
    vertexLocation.offset = 0;

    SDL_GPUBufferRegion vertexRegion{};
    vertexRegion.buffer = vertexBuffer.get();
    vertexRegion.size = sizeof(vertices);
    vertexRegion.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &vertexLocation, &vertexRegion, false);

    // upload index buffer
    SDL_GPUTransferBufferLocation indexLocation{};
    indexLocation.transfer_buffer = indexTransferBuffer.get();
    indexLocation.offset = 0;

    SDL_GPUBufferRegion indexRegion{};
    indexRegion.buffer = indexBuffer.get();
    indexRegion.size = sizeof(indices);
    indexRegion.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &indexLocation, &indexRegion, true);

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
    // SDL_GPUGraphicsPipeline *graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
    auto pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
    graphicsPipeline = std::unique_ptr<SDL_GPUGraphicsPipeline, GraphicsPipelineDeleter>(pipeline, {device});
    // we don't need to store the shaders after creating the pipeline
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);
}

RectElement::~RectElement()
{
    graphicsPipeline.reset();
    indexTransferBuffer.reset();
    vertexTransferBuffer.reset();
    indexBuffer.reset();
    vertexBuffer.reset();
}

void RectElement::Update()
{
    // rotate the rectangle over time
    transform.rotation.z += 0.05f;

    // recalculate the model matrix based on updated transform
    model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, transform.scale);

    float aspect = 1280.0f / 720.0f; // width / height
    projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
}

void RectElement::Render(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer)
{
    // bind the graphics pipeline
    SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline.get());

    // bind the vertex buffer
    SDL_GPUBufferBinding bufferBindings[1];
    bufferBindings[0].buffer = vertexBuffer.get(); // index 0 is slot 0 in this example
    bufferBindings[0].offset = 0;                  // start from the first byte

    SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1); // bind one buffer starting from slot 0

    // bind the index buffer
    SDL_GPUBufferBinding indexBufferBinding{};
    indexBufferBinding.buffer = indexBuffer.get();
    indexBufferBinding.offset = 0;

    SDL_BindGPUIndexBuffer(renderPass, &indexBufferBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    // Create uniform data struct with both model and projection matrices
    TransformUniform uniformData;
    uniformData.model = model;
    uniformData.projection = projection;

    // Push both matrices in ONE call to match the shader's uniform block
    SDL_PushGPUVertexUniformData(
        commandBuffer,
        0,
        &uniformData,
        sizeof(TransformUniform));

    // issue an indexed draw call (6 indices = 2 triangles for a rectangle)
    SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);
}
