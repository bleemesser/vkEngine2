#pragma once

#include "libs.hpp"
#include "memory.hpp"

struct FinalizationChunk {
    vk::Device device;
    vk::PhysicalDevice physicalDevice;
    vk::Queue queue;
    vk::CommandBuffer commandBuffer;
};

class MeshWrapper {
    public:
        MeshWrapper();
        ~MeshWrapper();
        void consume(meshTypes type, std::vector<float> vertices);
        void finalize(FinalizationChunk chunk);
        Buffer m_vertexBuffer;
        std::unordered_map<meshTypes, int> m_offsets;
        std::unordered_map<meshTypes, int> m_sizes;

    private:
        vk::Device m_device;
        int m_offset;
        std::vector<float> m_lump;
};