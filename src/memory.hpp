#pragma once

#include "libs.hpp"

namespace ASHUtil {
    Buffer createBuffer(BufferInput input);
    uint32_t findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties);
    void allocateBufferMemory(Buffer& buffer, const BufferInput& input);
    void copyBuffer(Buffer& src, Buffer& dst, vk::DeviceSize size, vk::Queue queue, vk::CommandBuffer commandBuffer);
}