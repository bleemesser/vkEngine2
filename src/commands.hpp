#pragma once

#include "libs.hpp"
#include "queues.hpp"
#include "frame.hpp"

namespace ASHInit {

    struct CommandBufferInput {
        vk::Device device;
        vk::CommandPool commandPool;
        std::vector<ASHUtil::SwapChainFrame>& frames;
    };

    vk::CommandPool createCommandPool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {
        ASHUtil::QueueFamilyIndices queueFamilyIndices = ASHUtil::findQueueFamilies(physicalDevice, surface);

        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        try {
            return device.createCommandPool(poolInfo);
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to create command pool");
        }
    }

    vk::CommandBuffer createCommandBuffer(CommandBufferInput input) {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = input.commandPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;

        try {
            vk::CommandBuffer primaryBuffer = input.device.allocateCommandBuffers(allocInfo)[0];
            #ifdef DEBUG
            std::cout << "Primary command buffer created" << std::endl;
            #endif
            return primaryBuffer;
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to allocate primary command buffer");
        }
    }

    void createFrameCommandBuffers(CommandBufferInput input) {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = input.commandPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;

        for (int i = 0; i < input.frames.size(); ++i) {
            try {
                input.frames[i].commandBuffer = input.device.allocateCommandBuffers(allocInfo)[0];
                #ifdef DEBUG
                std::cout << "Command buffer " << i << " created" << std::endl;
                #endif
            } catch (vk::SystemError err) {
                throw std::runtime_error("Failed to allocate command buffer");
            }
        }
    }
}