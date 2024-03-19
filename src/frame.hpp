#pragma once

#include "libs.hpp"
#include "memory.hpp"

namespace ASHUtil {
    struct UBO {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjection;
    };

    struct SwapChainFrame {
        vk::Image image;
        vk::ImageView imageView;
        vk::Framebuffer framebuffer;

        vk::CommandBuffer commandBuffer;

        vk::Semaphore imageAvailableSemaphore, renderFinishedSemaphore;
        vk::Fence inFlightFence;

        UBO cameraData;
        Buffer cameraDataBuffer;
        void* cameraDataWritePtr;

        std::vector<glm::mat4> modelMatrices;
        Buffer modelMatrixBuffer;
        void* modelMatrixWritePtr;

        vk::DescriptorBufferInfo uboDescriptor;
        vk::DescriptorBufferInfo modelMatrixDescriptor;
        vk::DescriptorSet descriptorSet;

        void createDescriptorResources(vk::Device device, vk::PhysicalDevice physicalDevice) {
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

        void writeDescriptorSet(vk::Device device) {
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
    };
}