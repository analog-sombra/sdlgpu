#include "shader.hpp"
SDL_GPUShader *CreateShader(SDL_GPUDevice *device, ShaderType type, const std::string &source)
{
    // load the vertex shader code
    size_t codeSize;
    void *code = SDL_LoadFile(source.c_str(), &codeSize);

    // create the vertex shader
    SDL_ShaderCross_SPIRV_Info vertexInfo{};
    vertexInfo.bytecode = (Uint8 *)code;
    vertexInfo.bytecode_size = codeSize;
    vertexInfo.entrypoint = "main";
    if (type == VERTEX_SHADER)
        vertexInfo.shader_stage = SDL_SHADERCROSS_SHADERSTAGE_VERTEX;
    if (type == FRAGMENT_SHADER)
        vertexInfo.shader_stage = SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT;

    // figure out shader metadata
    const SDL_ShaderCross_GraphicsShaderMetadata *metadata = SDL_ShaderCross_ReflectGraphicsSPIRV((Uint8 *)code, codeSize, 0);

    if (!metadata)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Shader reflection failed: %s",
            SDL_GetError());

        SDL_free(code);
        return nullptr;
    }
    // cross compile to the appropriate shaderformat and create a shader object
    SDL_GPUShader *shader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(device, &vertexInfo, &metadata->resource_info, 0);

    if (!shader)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Shader compilation failed: %s",
            SDL_GetError());
    }
    // free the metadata
    SDL_free((void *)metadata);

    // free the file
    SDL_free(code);
    return shader;
}