#include "engine/element/rect_element.hpp"

using namespace Engine;

RectElement::RectElement(SDL_GPUDevice *gpuDevice, SDL_Window *window)
    : device(gpuDevice), window(window)
{
    Vertex vertices[]{
        // 4 vertives for rect
        // {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}, // bottom left
        // {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},  // bottom right
        // {0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},   // top right
        // {-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f}   // top left

        -0.5f,
        -0.5f,
        -0.5f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        0.5f,
        0.5f,
        -0.5f,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        0.5f,
        0.5f,
        -0.5f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        -0.5f,
        0.5f,
        -0.5f,
        1.0f,
        1.0f,
        0.0f,
        1.0f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        1.0f,
        1.0f,
        1.0f,

        -0.5f,
        -0.5f,
        0.5f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.5f,
        -0.5f,
        0.5f,
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        0.5f,
        0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        0.5f,
        0.5f,
        0.5f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        -0.5f,
        0.5f,
        0.5f,
        1.0f,
        1.0f,
        0.0f,
        1.0f,
        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        1.0f,
        1.0f,
        1.0f,

        -0.5f,
        0.5f,
        0.5f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        -0.5f,
        0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        -0.5f,
        -0.5f,
        -0.5f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        -0.5f,
        -0.5f,
        0.5f,
        1.0f,
        1.0f,
        0.0f,
        1.0f,
        -0.5f,
        0.5f,
        0.5f,
        0.0f,
        1.0f,
        1.0f,
        1.0f,

        0.5f,
        0.5f,
        0.5f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.5f,
        0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        0.5f,
        -0.5f,
        -0.5f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        0.5f,
        -0.5f,
        0.5f,
        1.0f,
        1.0f,
        0.0f,
        1.0f,
        0.5f,
        0.5f,
        0.5f,
        0.0f,
        1.0f,
        1.0f,
        1.0f,

        -0.5f,
        -0.5f,
        -0.5f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        0.5f,
        -0.5f,
        0.5f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        -0.5f,
        -0.5f,
        0.5f,
        1.0f,
        1.0f,
        0.0f,
        1.0f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        1.0f,
        1.0f,
        1.0f,

        -0.5f,
        0.5f,
        -0.5f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.5f,
        0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        0.5f,
        0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        0.5f,
        0.5f,
        0.5f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        -0.5f,
        0.5f,
        0.5f,
        1.0f,
        1.0f,
        0.0f,
        1.0f,
        -0.5f,
        0.5f,
        -0.5f,
        0.0f,
        1.0f,
        1.0f,
        1.0f,
    };

    // uint32_t indices[] = {
    //     0, 1, 2,
    //     3, 0, 2};

    TransformUniform transformData;
    transformData.model = model;
    transformData.view = view;
    transformData.projection = projection;

    // create the vertex buffer
    auto vbuf = createGpuBufferInfo(device, SDL_GPU_BUFFERUSAGE_VERTEX, sizeof(vertices));
    vertexBuffer = std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter>(vbuf, {device});

    // create the index buffer
    // auto ibuf = createGpuBufferInfo(device, SDL_GPU_BUFFERUSAGE_INDEX, sizeof(indices));
    // indexBuffer = std::unique_ptr<SDL_GPUBuffer, GPUBufferDeleter>(ibuf, {device});

    // create transfer buffers to upload to GPU buffers
    SDL_GPUTransferBuffer *vertexTransferBuffer = createGpuTransferBufferInfo(device, vertices, sizeof(vertices));
    // SDL_GPUTransferBuffer *indexTransferBuffer = createGpuTransferBufferInfo(device, indices, sizeof(indices));

    // start a copy pass
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    // upload vertex buffer
    uploadBufferData(copyPass, vertexBuffer.get(), vertexTransferBuffer, sizeof(vertices));

    // upload index buffer
    // uploadBufferData(copyPass, indexBuffer.get(), indexTransferBuffer, sizeof(indices));

    // end the copy pass
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);

    SDL_GPUShader *vertexShader = CreateShader(device, VERTEX_SHADER, "shaders/vertex.spv");
    SDL_GPUShader *fragmentShader = CreateShader(device, FRAGMENT_SHADER, "shaders/fragment.spv");

    // describe the vertex buffers
    std::vector<SDL_GPUVertexBufferDescription> vertexBufferDescriptions = {createVertexBufferDescription(sizeof(Vertex))};

    std::vector<SDL_GPUVertexAttribute> vertexAttributes = createVertexAttribute({{SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 0},
                                                                                  {SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, sizeof(float) * 3}});

    // describe the color target
    SDL_GPUColorTargetDescription colorTargetDescriptions = createColorTargetDescription(device, window);

    // create the pipeline
    auto pipeline = createGraphicsPipeline(device, vertexShader, fragmentShader, vertexBufferDescriptions, vertexAttributes, colorTargetDescriptions);
    graphicsPipeline = std::unique_ptr<SDL_GPUGraphicsPipeline, GraphicsPipelineDeleter>(pipeline, {device});
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);
    SDL_ReleaseGPUTransferBuffer(device, vertexTransferBuffer);
    // SDL_ReleaseGPUTransferBuffer(device, indexTransferBuffer);

    this->cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    this->cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

RectElement::~RectElement()
{
    graphicsPipeline.reset();
    indexBuffer.reset();
    vertexBuffer.reset();
}

void RectElement::Update()
{

    float aspect = 1280.0f / 720.0f; // width / height
    // projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    // view = glm::mat4(1.0f);
    // // note that we're translating the scene in the reverse direction of where we want to move
    // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    // view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(0.1f, 0.0f, 0.0f));
    // rotate every fram
    // model = glm::rotate(model,glm::translate, glm::vec3(0.0f, 1.0f, 0.0f));
    // model = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
}

void RectElement::HandleEvents(const SDL_Event &event)
{

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    const float cameraSpeed = 0.05f; // adjust accordingly
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_W)
    {
        SDL_Log("W key pressed");

        cameraPos += cameraSpeed * cameraFront;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_S)
    {

        cameraPos -= cameraSpeed * cameraFront;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_A)
    {

        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_D)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }
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
    // SDL_GPUBufferBinding indexBufferBinding{};
    // indexBufferBinding.buffer = indexBuffer.get();
    // indexBufferBinding.offset = 0;

    // SDL_BindGPUIndexBuffer(renderPass, &indexBufferBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    // Create uniform data struct with both model and projection matrices
    TransformUniform uniformData;
    uniformData.model = model;
    uniformData.projection = projection;
    uniformData.view = view;

    // Push both matrices in ONE call to match the shader's uniform block
    SDL_PushGPUVertexUniformData(
        commandBuffer,
        0,
        &uniformData,
        sizeof(TransformUniform));

    // issue an indexed draw call (6 indices = 2 triangles for a rectangle)
    // SDL_DrawGPUIndexedPrimitives(renderPass, 36, 1, 0, 0, 0);
    SDL_DrawGPUPrimitives(renderPass, 36, 1, 0, 0);
}
