#include "engine/element/rect_element.hpp"

using namespace Engine;

RectElement::RectElement(SDL_GPUDevice *gpuDevice, SDL_Window *sdlWindow)
    : device(gpuDevice), window(sdlWindow)
{
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

    TransformUniform transformData;
    transformData.model = model;
    transformData.projection = projection;

    // create the vertex buffer
    auto vbuf = createGpuBufferInfo(device, SDL_GPU_BUFFERUSAGE_VERTEX, sizeof(vertices));
    vertexBuffer = std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter>(vbuf, {device});

    // create the index buffer
    auto ibuf = createGpuBufferInfo(device, SDL_GPU_BUFFERUSAGE_INDEX, sizeof(indices));
    indexBuffer = std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter>(ibuf, {device});

    // create transfer buffers to upload to GPU buffers
    auto vxbuf = createGpuTransferBufferInfo(device, sizeof(vertices));
    vertexTransferBuffer = std::unique_ptr<SDL_GPUTransferBuffer, GPUTransferBufferDeleter>(vxbuf, {device});

    auto ixbuf = createGpuTransferBufferInfo(device, sizeof(indices));
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
    uploadBufferData(copyPass, vertexBuffer.get(), vertexTransferBuffer.get(), sizeof(vertices));

    // upload index buffer
    uploadBufferData(copyPass, indexBuffer.get(), indexTransferBuffer.get(), sizeof(indices));

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
    SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1] = {createVertexBufferDescription(sizeof(Vertex))};

    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDesctiptions;
    
    auto vertexAttributes = createVertexAttribute({{SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 0},
                                                    {SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, sizeof(float) * 3}});
    pipelineInfo.vertex_input_state.num_vertex_attributes = vertexAttributes.size();
    pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes.data();

    // describe the color target
    SDL_GPUColorTargetDescription colorTargetDescriptions = createColorTargetDescription(device, window);

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDescriptions;

    // create the pipeline
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
    transform.rotation.y += 0.05f;
    transform.rotation.x += 0.05f;
    transform.rotation.z += 0.05f;

    // recalculate the model matrix based on updated transform
    model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
    model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, transform.scale);

    float aspect = 1280.0f / 720.0f; // width / height
    // projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
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

// 225