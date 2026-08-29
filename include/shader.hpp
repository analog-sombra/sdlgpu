#ifndef __SHADER_HPP__
#define __SHADER_HPP__
#include <string>
#include <iostream>
#include <SDL3/SDL_gpu.h>
#include <SDL3_shadercross/SDL_shadercross.h>

enum ShaderType
{
    VERTEX_SHADER,
    FRAGMENT_SHADER,
};
SDL_GPUShader *CreateShader(SDL_GPUDevice *device, ShaderType type, const std::string &source);

#endif // __SHADER_HPP__
