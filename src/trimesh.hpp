#pragma once

#include "libs.hpp"
#include "memory.hpp"

class TriangleMesh {
    public:
        TriangleMesh(vk::Device device, vk::PhysicalDevice physicalDevice);
        ~TriangleMesh();
        Buffer m_vertexBuffer;
        
    private:
        vk::Device m_device;
};