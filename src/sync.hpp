#pragma once

#include "libs.hpp"

namespace ASHInit {
    vk::Semaphore createSemaphore(vk::Device device) {
        vk::SemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.flags = vk::SemaphoreCreateFlags();

        try {
            return device.createSemaphore(semaphoreInfo);
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to create semaphore");
        }
    }

    vk::Fence createFence(vk::Device device) {
        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

        try {
            return device.createFence(fenceInfo);
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to create fence");
        }
    }
}