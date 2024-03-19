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
        void consume(meshTypes type, std::vector<float> vertices, std::vector<uint32_t> indices);
        void finalize(FinalizationChunk chunk);
        Buffer m_vertexBuffer, m_indexBuffer;
        std::unordered_map<meshTypes, int> m_firstIndices;
        std::unordered_map<meshTypes, int> m_indexCounts;

    private:
        vk::Device m_device;
        int m_indexOffset;
        std::vector<float> m_vertexLump;
        std::vector<uint32_t> m_indexLump;
};