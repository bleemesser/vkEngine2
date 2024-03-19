#include "meshwrapper.hpp"

MeshWrapper::MeshWrapper() {
    m_offset = 0;
}

void MeshWrapper::consume(meshTypes type, std::vector<float> vertices) {
    for (float attribute : vertices) {
        m_lump.push_back(attribute);
    }
    int vertexCount = static_cast<int>(vertices.size() / 7);

    m_offsets.insert(std::make_pair(type, m_offset));
    m_sizes.insert(std::make_pair(type, vertexCount));

    m_offset += vertexCount;
}

void MeshWrapper::finalize(FinalizationChunk chunk) {
    m_device = chunk.device;

    BufferInput input;
    input.device = m_device;
    input.physicalDevice = chunk.physicalDevice;
    input.size = sizeof(m_lump[0]) * m_lump.size();
    input.usage = vk::BufferUsageFlagBits::eTransferSrc;
    input.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    Buffer stagingBuffer = ASHUtil::createBuffer(input);

    void* memoryLoc = m_device.mapMemory(stagingBuffer.memory, 0, input.size);
    memcpy(memoryLoc, m_lump.data(), input.size);
    m_device.unmapMemory(stagingBuffer.memory);

    input.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
    input.properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    m_vertexBuffer = ASHUtil::createBuffer(input);

    ASHUtil::copyBuffer(stagingBuffer, m_vertexBuffer, input.size, chunk.queue, chunk.commandBuffer);

    m_device.destroyBuffer(stagingBuffer.buffer);
    m_device.freeMemory(stagingBuffer.memory);
}

MeshWrapper::~MeshWrapper() {
    m_device.destroyBuffer(m_vertexBuffer.buffer);
    m_device.freeMemory(m_vertexBuffer.memory);
}