#pragma once

#include "libs.hpp"

#include "stb_image.h"

namespace ASHImage {

    struct TextureInput {
        vk::Device device;
        vk::PhysicalDevice physicalDevice;
        const char* path;
        vk::CommandBuffer commandBuffer;
        vk::Queue queue;
        vk::DescriptorSetLayout layout;
        vk::DescriptorPool pool;
    };

    struct ImageInput {
        vk::Device device;
        vk::PhysicalDevice physicalDevice;
        int width, height;
        vk::ImageTiling tiling;
        vk::ImageUsageFlags usage;
        vk::MemoryPropertyFlags properties;
        vk::Format format;
    };

    struct ImageLayoutTransition {
        vk::CommandBuffer commandBuffer;
        vk::Queue queue;
        vk::Image image;
        vk::ImageLayout oldLayout, newLayout;
    };

    struct BufferCopy {
        vk::CommandBuffer commandBuffer;
        vk::Queue queue;
        vk::Buffer srcBuffer;
        vk::Image dstImage;
        int width, height;
    };

    class Texture {
        public:
            Texture(TextureInput input);
            ~Texture();

            void use(vk::CommandBuffer commandBuffer, vk::PipelineLayout layout);


        private:
            int m_width, m_height, m_channels;
            vk::Device m_device;
            vk::PhysicalDevice m_physicalDevice;
            const char* m_path;
            stbi_uc* m_pixels;

            vk::Image m_image;
            vk::DeviceMemory m_imageMemory;
            vk::ImageView m_imageView;
            vk::Sampler m_sampler;

            vk::DescriptorSetLayout m_layout;
            vk::DescriptorSet m_descriptorSet;
            vk::DescriptorPool m_descriptorPool;

            vk::CommandBuffer m_commandBuffer;
            vk::Queue m_queue;

            void load(); 

            void populate(); 

            void createView(); 

            void createSampler(); 

            void createDescriptorSet(); 
    };

    vk::Image createImage(ImageInput input);

    vk::DeviceMemory createImageMemory(ImageInput input, vk::Image image);

    void transitionImageLayout(ImageLayoutTransition input);

    void copyBufferToImage(BufferCopy input);

    vk::ImageView createImageView(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);

    vk::Format getSupportedFormat(vk::PhysicalDevice physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
}