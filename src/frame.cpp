#include "frame.hpp"
#include "memory.hpp"
#include "image.hpp"


void ASHUtil::SwapChainFrame::createDescriptorResources() {
    BufferInput input;
    input.device = device;
    input.physicalDevice = physicalDevice;
    input.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    input.size = sizeof(UBO);
    input.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    cameraDataBuffer = createBuffer(input);

    cameraDataWritePtr = device.mapMemory(cameraDataBuffer.memory, 0, sizeof(UBO));

    int maxModelMatrices = 1024;
    input.size = maxModelMatrices * sizeof(glm::mat4); // 1024 model matrices limit
    input.usage = vk::BufferUsageFlagBits::eStorageBuffer;
    modelMatrixBuffer = createBuffer(input);

    modelMatrixWritePtr = device.mapMemory(modelMatrixBuffer.memory, 0, maxModelMatrices * sizeof(glm::mat4));

    modelMatrices.reserve(maxModelMatrices);

    for (int i = 0; i < maxModelMatrices; ++i) {
        modelMatrices.push_back(glm::mat4(1.0f));
    }


    uboDescriptor.buffer = cameraDataBuffer.buffer;
    uboDescriptor.offset = 0;
    uboDescriptor.range = sizeof(UBO);

    modelMatrixDescriptor.buffer = modelMatrixBuffer.buffer;
    modelMatrixDescriptor.offset = 0;
    modelMatrixDescriptor.range = maxModelMatrices * sizeof(glm::mat4);


}

void ASHUtil::SwapChainFrame::createDepthResources() {
    depthFormat = ASHImage::getSupportedFormat(physicalDevice, {vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint}, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);

    ASHImage::ImageInput input;
    input.device = device;
    input.physicalDevice = physicalDevice;
    input.tiling = vk::ImageTiling::eOptimal;
    input.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    input.properties = vk::MemoryPropertyFlagBits::eDeviceLocal;    
    input.width = width;
    input.height = height;
    input.format = depthFormat;

    depthBuffer = ASHImage::createImage(input);
    depthBufferMemory = ASHImage::createImageMemory(input, depthBuffer);
    depthBufferView = ASHImage::createImageView(device, depthBuffer, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

void ASHUtil::SwapChainFrame::writeDescriptorSet() {
    vk::WriteDescriptorSet writeInfo;
    writeInfo.dstSet = descriptorSet;
    writeInfo.dstBinding = 0;
    writeInfo.dstArrayElement = 0;
    writeInfo.descriptorCount = 1;
    writeInfo.descriptorType = vk::DescriptorType::eUniformBuffer;
    writeInfo.pBufferInfo = &uboDescriptor;

    device.updateDescriptorSets(writeInfo, nullptr);

    vk::WriteDescriptorSet modelMatrixWriteInfo;
    modelMatrixWriteInfo.dstSet = descriptorSet;
    modelMatrixWriteInfo.dstBinding = 1;
    modelMatrixWriteInfo.dstArrayElement = 0;
    modelMatrixWriteInfo.descriptorCount = 1;
    modelMatrixWriteInfo.descriptorType = vk::DescriptorType::eStorageBuffer;
    modelMatrixWriteInfo.pBufferInfo = &modelMatrixDescriptor;

    device.updateDescriptorSets(modelMatrixWriteInfo, nullptr);
}

void ASHUtil::SwapChainFrame::destroy() {
    device.unmapMemory(cameraDataBuffer.memory);
    device.unmapMemory(modelMatrixBuffer.memory);

    device.freeMemory(cameraDataBuffer.memory);
    device.freeMemory(modelMatrixBuffer.memory);

    device.destroyBuffer(cameraDataBuffer.buffer);
    device.destroyBuffer(modelMatrixBuffer.buffer);

    device.destroyImage(depthBuffer);

    device.freeMemory(depthBufferMemory);

    device.destroyImageView(depthBufferView);
    device.destroyImageView(imageView);

    device.destroyFramebuffer(framebuffer);

    device.destroyFence(inFlightFence);

    device.destroySemaphore(imageAvailableSemaphore);
    device.destroySemaphore(renderFinishedSemaphore);
}

