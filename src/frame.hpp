#pragma once

#include "libs.hpp"
#include "memory.hpp"

namespace ASHUtil {
    struct UBO {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjection;
    };

    class SwapChainFrame { // TODO: add m_ prefix to member variables
        public:
            vk::Device device;
            vk::PhysicalDevice physicalDevice;

            vk::Image image;
            vk::ImageView imageView;
            vk::Framebuffer framebuffer;
            vk::Image depthBuffer;
            vk::DeviceMemory depthBufferMemory;
            vk::ImageView depthBufferView;
            vk::Format depthFormat;
            int width, height;

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

            void createDescriptorResources();
            void createDepthResources();
            void writeDescriptorSet();
            void destroy();
    };
}