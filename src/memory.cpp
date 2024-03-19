#include "memory.hpp"
#include "onetimecommands.hpp"

Buffer ASHUtil::createBuffer(BufferInput input) {
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.flags = vk::BufferCreateFlags();
    bufferInfo.size = input.size;
    bufferInfo.usage = input.usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    Buffer buffer{};
    buffer.buffer = input.device.createBuffer(bufferInfo);

    allocateBufferMemory(buffer, input);

    return buffer;
}

uint32_t ASHUtil::findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties) {
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        bool supported = static_cast<bool>(supportedMemoryIndices & (1 << i));
        bool hasRequestedProperties = (memProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties;

        if (supported && hasRequestedProperties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type");
}

void ASHUtil::allocateBufferMemory(Buffer& buffer, const BufferInput& input) {
    vk::MemoryRequirements memRequirements = input.device.getBufferMemoryRequirements(buffer.buffer);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryTypeIndex(input.physicalDevice, memRequirements.memoryTypeBits, input.properties);

    buffer.memory = input.device.allocateMemory(allocInfo);
    input.device.bindBufferMemory(buffer.buffer, buffer.memory, 0);
}

void ASHUtil::copyBuffer(Buffer& src, Buffer& dst, vk::DeviceSize size, vk::Queue queue, vk::CommandBuffer commandBuffer) {
    startJob(commandBuffer);

    vk::BufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    commandBuffer.copyBuffer(src.buffer, dst.buffer, 1, &copyRegion);

    endJob(commandBuffer, queue);
}