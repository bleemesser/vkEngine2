#include "meshwrapper.hpp"

MeshWrapper::MeshWrapper() {
    m_indexOffset = 0;
}

void MeshWrapper::consume(meshTypes type, std::vector<float>& vertices, std::vector<uint32_t>& indices) {
    
    int vertexCount = static_cast<int>(vertices.size() / 8);
    int indexCount = static_cast<int>(indices.size());
    int lastIndex = static_cast<int>(m_indexLump.size());

    m_firstIndices.insert(std::make_pair(type, lastIndex));
    m_indexCounts.insert(std::make_pair(type, indexCount));

    for (float attribute : vertices) {
        m_vertexLump.push_back(attribute);
    }

    for (uint32_t index : indices) {
        m_indexLump.push_back(index + m_indexOffset);
    }

    m_indexOffset += vertexCount;
}

void MeshWrapper::finalize(FinalizationChunk chunk) {
    m_device = chunk.device;
    // Vertex buffer
    BufferInput input;
    input.device = m_device;
    input.physicalDevice = chunk.physicalDevice;
    input.size = sizeof(m_vertexLump[0]) * m_vertexLump.size();
    input.usage = vk::BufferUsageFlagBits::eTransferSrc;
    input.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    Buffer stagingBuffer = ASHUtil::createBuffer(input);

    void* memoryLoc = m_device.mapMemory(stagingBuffer.memory, 0, input.size);
    memcpy(memoryLoc, m_vertexLump.data(), input.size);
    m_device.unmapMemory(stagingBuffer.memory);

    input.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
    input.properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    m_vertexBuffer = ASHUtil::createBuffer(input);

    ASHUtil::copyBuffer(stagingBuffer, m_vertexBuffer, input.size, chunk.queue, chunk.commandBuffer);

    m_device.destroyBuffer(stagingBuffer.buffer);
    m_device.freeMemory(stagingBuffer.memory);

    // Index buffer
    input.size = sizeof(m_indexLump[0]) * m_indexLump.size();
    input.usage = vk::BufferUsageFlagBits::eTransferSrc;
    input.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    stagingBuffer = ASHUtil::createBuffer(input);

    memoryLoc = m_device.mapMemory(stagingBuffer.memory, 0, input.size);
    memcpy(memoryLoc, m_indexLump.data(), input.size);
    m_device.unmapMemory(stagingBuffer.memory);

    input.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
    input.properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    m_indexBuffer = ASHUtil::createBuffer(input);

    ASHUtil::copyBuffer(stagingBuffer, m_indexBuffer, input.size, chunk.queue, chunk.commandBuffer);

    m_device.destroyBuffer(stagingBuffer.buffer);
    m_device.freeMemory(stagingBuffer.memory);

    m_indexLump.clear();


}

MeshWrapper::~MeshWrapper() {
    m_device.destroyBuffer(m_vertexBuffer.buffer);
    m_device.freeMemory(m_vertexBuffer.memory);

    m_device.destroyBuffer(m_indexBuffer.buffer);
    m_device.freeMemory(m_indexBuffer.memory);
}