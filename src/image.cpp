#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "memory.hpp"
#include "descriptors.hpp"
#include "onetimecommands.hpp"

ASHImage::Texture::Texture(TextureInput input) {
    m_device = input.device;
    m_physicalDevice = input.physicalDevice;
    m_path = input.path;
    m_commandBuffer = input.commandBuffer;
    m_queue = input.queue;
    m_layout = input.layout;
    m_descriptorPool = input.pool;

    load();

    ImageInput imageInput;
    imageInput.device = m_device;
    imageInput.physicalDevice = m_physicalDevice;
    imageInput.width = m_width;
    imageInput.height = m_height;
    imageInput.tiling = vk::ImageTiling::eOptimal;
    imageInput.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    imageInput.properties = vk::MemoryPropertyFlagBits::eDeviceLocal;

    m_image = createImage(imageInput);
    m_imageMemory = createImageMemory(imageInput, m_image);

    populate();

    free(m_pixels);

    createView();

    createSampler();

    createDescriptorSet();
}

ASHImage::Texture::~Texture() {
    m_device.destroySampler(m_sampler);
    m_device.destroyImageView(m_imageView);
    m_device.destroyImage(m_image);
    m_device.freeMemory(m_imageMemory);
}

void ASHImage::Texture::use(vk::CommandBuffer commandBuffer, vk::PipelineLayout layout) {
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 1, m_descriptorSet, nullptr);
}

void ASHImage::Texture::load() {
    stbi_uc* pixels = stbi_load(m_path, &m_width, &m_height, &m_channels, STBI_rgb_alpha);

    if (!pixels) {
        throw std::runtime_error("Failed to load texture " + std::string(m_path));
    }

    m_pixels = pixels;
}

void ASHImage::Texture::populate() {
    BufferInput input; 
    input.device = m_device;
    input.physicalDevice = m_physicalDevice;
    input.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    input.usage = vk::BufferUsageFlagBits::eTransferSrc;
    input.size = m_width * m_height * 4;

    Buffer stagingBuffer = ASHUtil::createBuffer(input);

    void* memoryLoc = m_device.mapMemory(stagingBuffer.memory, 0, input.size);
    memcpy(memoryLoc, m_pixels, input.size);
    m_device.unmapMemory(stagingBuffer.memory);

    ImageLayoutTransition transition;
    transition.commandBuffer = m_commandBuffer;
    transition.queue = m_queue;
    transition.image = m_image;
    transition.oldLayout = vk::ImageLayout::eUndefined;
    transition.newLayout = vk::ImageLayout::eTransferDstOptimal;
    transitionImageLayout(transition);

    BufferCopy copy;
    copy.commandBuffer = m_commandBuffer;
    copy.queue = m_queue;
    copy.srcBuffer = stagingBuffer.buffer;
    copy.dstImage = m_image;
    copy.width = m_width;
    copy.height = m_height;
    copyBufferToImage(copy);

    transition.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    transition.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    transitionImageLayout(transition);

    m_device.freeMemory(stagingBuffer.memory);
    m_device.destroyBuffer(stagingBuffer.buffer);
}

void ASHImage::Texture::createView() {
    m_imageView = createImageView(m_device, m_image, vk::Format::eR8G8B8A8Unorm);
}

void ASHImage::Texture::createSampler() {
    vk::SamplerCreateInfo samplerInfo;
    samplerInfo.flags = vk::SamplerCreateFlags();
    samplerInfo.minFilter = vk::Filter::eNearest;
    samplerInfo.magFilter = vk::Filter::eLinear;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    try {
        m_sampler = m_device.createSampler(samplerInfo);
    } catch (vk::SystemError err) {
        throw std::runtime_error("Failed to create texture sampler");
    }
}

void ASHImage::Texture::createDescriptorSet() {
    m_descriptorSet = ASHInit::allocateDescriptorSet(m_device, m_descriptorPool, m_layout);

    vk::DescriptorImageInfo imageInfo;
    imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    imageInfo.imageView = m_imageView;
    imageInfo.sampler = m_sampler;

    vk::WriteDescriptorSet descriptorWrite;
    descriptorWrite.dstSet = m_descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    m_device.updateDescriptorSets(descriptorWrite, nullptr);

}

vk::Image ASHImage::createImage(ImageInput input) {
    vk::ImageCreateInfo imageInfo;
    imageInfo.flags = vk::ImageCreateFlags();
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent = vk::Extent3D(input.width, input.height, 1);
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = vk::Format::eR8G8B8A8Unorm;
    imageInfo.tiling = input.tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = input.usage;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.samples = vk::SampleCountFlagBits::e1;

    try {
        return input.device.createImage(imageInfo);
    } catch (vk::SystemError err) {
        throw std::runtime_error("Failed to create image");
    }
}

vk::DeviceMemory ASHImage::createImageMemory(ImageInput input, vk::Image image) {
    vk::MemoryRequirements requirements = input.device.getImageMemoryRequirements(image);

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = requirements.size;
    allocInfo.memoryTypeIndex = ASHUtil::findMemoryTypeIndex(
        input.physicalDevice,
        requirements.memoryTypeBits,
        input.properties
    );

    try {
        vk::DeviceMemory memory = input.device.allocateMemory(allocInfo);
        input.device.bindImageMemory(image, memory, 0);
        return memory;
    } catch (vk::SystemError err) {
        throw std::runtime_error("Failed to allocate image memory");
    }
}

void ASHImage::transitionImageLayout(ImageLayoutTransition input) {
    ASHUtil::startJob(input.commandBuffer);

    vk::ImageSubresourceRange range;
    range.aspectMask = vk::ImageAspectFlagBits::eColor;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = 1;

    vk::ImageMemoryBarrier barrier;
    barrier.oldLayout = input.oldLayout;
    barrier.newLayout = input.newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = input.image;
    barrier.subresourceRange = range;

    vk::PipelineStageFlags sourceStage, dstStage;

    if (input.oldLayout == vk::ImageLayout::eUndefined && input.newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (input.oldLayout == vk::ImageLayout::eTransferDstOptimal && input.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        dstStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        throw std::invalid_argument("Unsupported layout transition");
    }

    input.commandBuffer.pipelineBarrier(
        sourceStage, dstStage,
        vk::DependencyFlags(),
        nullptr, nullptr, barrier
    );

    ASHUtil::endJob(input.commandBuffer, input.queue);
}

void ASHImage::copyBufferToImage(BufferCopy input) {
    ASHUtil::startJob(input.commandBuffer);

    vk::BufferImageCopy copy;
    copy.bufferOffset = 0;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;

    vk::ImageSubresourceLayers subresource;
    subresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    subresource.mipLevel = 0;
    subresource.baseArrayLayer = 0;
    subresource.layerCount = 1;
    copy.imageSubresource = subresource;

    copy.imageOffset = vk::Offset3D(0, 0, 0);
    copy.imageExtent = vk::Extent3D(input.width, input.height, 1);

    input.commandBuffer.copyBufferToImage(input.srcBuffer, input.dstImage, vk::ImageLayout::eTransferDstOptimal, copy);

    ASHUtil::endJob(input.commandBuffer, input.queue);
}

vk::ImageView ASHImage::createImageView(vk::Device device, vk::Image image, vk::Format format) {
    vk::ImageViewCreateInfo createInfo{};
    createInfo.image = image;
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    createInfo.format = format;

    return device.createImageView(createInfo);
}