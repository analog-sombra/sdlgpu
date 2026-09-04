#include "engine/gpu_utils.hpp"

namespace Engine
{

    SDL_GPUBuffer *createGpuBufferInfo(
        SDL_GPUDevice *device,
        SDL_GPUBufferUsageFlags type,
        Uint32 size)
    {
        SDL_GPUBufferCreateInfo bufferInfo{};
        bufferInfo.size = size;
        bufferInfo.usage = type;
        return SDL_CreateGPUBuffer(device, &bufferInfo);
    }

    SDL_GPUTransferBuffer *createGpuTransferBufferInfo(
        SDL_GPUDevice *device,
        const void *data,
        Uint32 size)
    {
        SDL_GPUTransferBufferCreateInfo transferBufferInfo{};
        transferBufferInfo.size = size;
        transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferInfo);

        // upload vertex data
        void *mapped = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
        SDL_memcpy(mapped, data, size);
        SDL_UnmapGPUTransferBuffer(device, transferBuffer);
        return transferBuffer;
    }

    void uploadBufferData(
        SDL_GPUCopyPass *copyPass,
        SDL_GPUBuffer *buffer,
        SDL_GPUTransferBuffer *transferBuffer,
        Uint32 size)
    {
        // upload vertex buffer
        SDL_GPUTransferBufferLocation vertexLocation{};
        vertexLocation.transfer_buffer = transferBuffer;
        vertexLocation.offset = 0;

        SDL_GPUBufferRegion vertexRegion{};
        vertexRegion.buffer = buffer;
        vertexRegion.size = size;
        vertexRegion.offset = 0;

        SDL_UploadToGPUBuffer(copyPass, &vertexLocation, &vertexRegion, false);
    }

    SDL_GPUColorTargetDescription createColorTargetDescription(
        SDL_GPUDevice *device,
        SDL_Window *window)
    {
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
        return colorTargetDescriptions[0];
    }

    SDL_GPUVertexBufferDescription createVertexBufferDescription(Uint32 size)
    {
        // describe the vertex buffers
        SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1];
        vertexBufferDesctiptions[0].slot = 0;
        vertexBufferDesctiptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
        vertexBufferDesctiptions[0].instance_step_rate = 0;
        vertexBufferDesctiptions[0].pitch = size;
        return vertexBufferDesctiptions[0];
    }

    std::vector<SDL_GPUVertexAttribute> createVertexAttribute(const std::vector<CreateVertexAttribute> &attributes)
    {
        std::vector<SDL_GPUVertexAttribute> vertexAttributes;
        int i = 0;
        for (const auto &attr : attributes)
        {
            SDL_GPUVertexAttribute vertexAttribute{};
            vertexAttribute.format = attr.format;
            vertexAttribute.offset = attr.offset;
            vertexAttribute.buffer_slot = 0;
            vertexAttribute.location = i++;
            vertexAttributes.push_back(vertexAttribute);
        }
        return vertexAttributes;
    }

    SDL_GPUGraphicsPipeline *createGraphicsPipeline(
        SDL_GPUDevice *device,
        SDL_GPUShader *vertexShader,
        SDL_GPUShader *fragmentShader,
        const std::vector<SDL_GPUVertexBufferDescription> &vertexBufferDescriptions,
        const std::vector<SDL_GPUVertexAttribute> &vertexAttributes,
        SDL_GPUColorTargetDescription colorTargetDescription)
    {
        SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

        // bind shaders
        pipelineInfo.vertex_shader = vertexShader;
        pipelineInfo.fragment_shader = fragmentShader;

        // draw triangles
        pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

        pipelineInfo.vertex_input_state.num_vertex_buffers = vertexBufferDescriptions.size();
        pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDescriptions.data();

        pipelineInfo.vertex_input_state.num_vertex_attributes = vertexAttributes.size();
        pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes.data();

        pipelineInfo.target_info.num_color_targets = 1;
        pipelineInfo.target_info.color_target_descriptions = &colorTargetDescription;

        SDL_GPUDepthStencilState depthState{};

        depthState.enable_depth_test = true;
        depthState.enable_depth_write = true;
        depthState.compare_op = SDL_GPU_COMPAREOP_LESS;

        
        pipelineInfo.depth_stencil_state = depthState;

        // Also need to set depth format:
        pipelineInfo.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;

        return SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
    }
}