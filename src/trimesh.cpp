#include "trimesh.hpp"

TriangleMesh::TriangleMesh(vk::Device device, vk::PhysicalDevice physicalDevice) {
    m_device = device;

    std::vector<float> vertices = { {
        0.0f, -0.05f, 0.0f, 1.0f, 0.0f,
        0.05f, 0.05f, 0.0f, 1.0f, 0.0f,
        -0.05f, 0.05f, 0.0f, 1.0f, 0.0f
    }};
    
    BufferInput input;
    input.device = device;
    input.physicalDevice = physicalDevice;
    input.size = sizeof(vertices[0]) * vertices.size();
    input.usage = vk::BufferUsageFlagBits::eVertexBuffer;

    m_vertexBuffer = ASHUtil::createBuffer(input);

    void* memoryLoc = device.mapMemory(m_vertexBuffer.memory, 0, input.size);
    memcpy(memoryLoc, vertices.data(), input.size);
    device.unmapMemory(m_vertexBuffer.memory);
}

TriangleMesh::~TriangleMesh() {
    m_device.destroyBuffer(m_vertexBuffer.buffer);
    m_device.freeMemory(m_vertexBuffer.memory);
}