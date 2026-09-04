#include "engine/game.hpp"
using namespace Engine;

void SDLCALL MyLogCallback(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    if (priority >= SDL_LOG_PRIORITY_ERROR)
    {
        // Breakpoint here in your IDE to catch any SDL error instantly
        spdlog::error("SDL Error: {}", message);
    }
}

Game::Game(std::string title)
{

    // Enable verbose error logging globally
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
    SDL_SetLogOutputFunction(MyLogCallback, NULL);

    SDL_Init(SDL_INIT_VIDEO);

    window.reset(SDL_CreateWindow(title.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE));
    device.reset(SDL_CreateGPUDevice(SDL_ShaderCross_GetSPIRVShaderFormats(), true, NULL));
    SDL_ClaimWindowForGPUDevice(device.get(), window.get());

    // Initialize RectElement with device and window pointers
    // rectElement.Initialize(device.get(), window.get());
    rectElement = std::make_unique<RectElement>(device.get(), window.get());

    spdlog::info("GPU driver: {}", SDL_GetGPUDeviceDriver(device.get()));

    bool issup = SDL_WindowSupportsGPUPresentMode(
        device.get(),
        window.get(),
        SDL_GPU_PRESENTMODE_IMMEDIATE);
    if (issup)
    {
        spdlog::info("Immediate present mode is supported");
    }
    else
    {
        spdlog::warn("Immediate present mode is not supported");
    }

    if (SDL_WindowSupportsGPUPresentMode(
            device.get(),
            window.get(),
            SDL_GPU_PRESENTMODE_IMMEDIATE))
    {
        SDL_SetGPUSwapchainParameters(
            device.get(),
            window.get(),
            SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
            SDL_GPU_PRESENTMODE_IMMEDIATE);
    }
    else
    {
        spdlog::info("Immediate present mode is not supported");
    }

    SDL_PropertiesID props = SDL_GetGPUDeviceProperties(device.get());

    const char *gpuName = SDL_GetStringProperty(
        props,
        SDL_PROP_GPU_DEVICE_NAME_STRING,
        "Unknown");

    const char *driverName = SDL_GetStringProperty(
        props,
        SDL_PROP_GPU_DEVICE_DRIVER_NAME_STRING,
        "Unknown");

    spdlog::info("GPU: {}", gpuName);
    spdlog::info("Driver: {}", driverName);
    spdlog::info("Backend: {}", SDL_GetGPUDeviceDriver(device.get()));
}

Game::~Game()
{
    if (device)
        SDL_WaitForGPUIdle(device.get());

    if (device && window)
        SDL_ReleaseWindowFromGPUDevice(device.get(), window.get());

    device.reset();
    window.reset();

    SDL_Quit();
}

void Game::Run()
{
    Uint64 fps = 0;
    Uint64 fpsTimer = SDL_GetTicks();

    while (running)
    {
        HandleEvents();
        Update();
        Render();
        fps++;

        Uint64 now = SDL_GetTicks();
        if (now - fpsTimer >= 1000)
        {
            SDL_SetWindowTitle(window.get(), fmt::format("Engine Window - FPS: {}", fps).c_str());
            fps = 0;
            fpsTimer = now;
        }
    }
}

void Game::Render()
{
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device.get());
    SDL_GPUTexture *swapchainTexture;
    Uint32 width, height;
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window.get(), &swapchainTexture, &width, &height);

    // create the color target
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color = {135 / 255.0f, 206 / 255.0f, 235 / 255.0f, 255 / 255.0f};
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture = swapchainTexture;

    // begin a render pass
    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

    rectElement.get()->Render(renderPass, commandBuffer);

    // end the render pass
    SDL_EndGPURenderPass(renderPass);

    // submit the command buffer
    SDL_SubmitGPUCommandBuffer(commandBuffer);
}

void Game::Update()
{
    rectElement.get()->Update();
    // update game state here
}

void Game::HandleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
            running = false;

        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F11)
        {
            SDL_SetWindowFullscreen(window.get(), fullscreen);
            fullscreen = !fullscreen;
        }
    }
}

void Game::GetAllRenderDrivers()
{
    int numDrivers = SDL_GetNumRenderDrivers();
    for (int i = 0; i < numDrivers; ++i)
    {
        spdlog::info("Render Driver {}: {}", i, SDL_GetRenderDriver(i));
    }
}

void Game::GetCurrentRenderDriver()
{
    spdlog::info("GPU driver: {}", SDL_GetGPUDeviceDriver(device.get()));
}

void Game::GetSupportedShaderFormats()
{
    SDL_GPUShaderFormat formats = SDL_GetGPUShaderFormats(device.get());
    if (formats & SDL_GPU_SHADERFORMAT_SPIRV)
        spdlog::info("Supports SPIR-V");
    if (formats & SDL_GPU_SHADERFORMAT_DXIL)
        spdlog::info("Supports DXIL");
    if (formats & SDL_GPU_SHADERFORMAT_DXBC)
        spdlog::info("Supports DXBC");
    if (formats & SDL_GPU_SHADERFORMAT_MSL)
        spdlog::info("Supports MSL");
    if (formats & SDL_GPU_SHADERFORMAT_METALLIB)
        spdlog::info("Supports Metal");
}