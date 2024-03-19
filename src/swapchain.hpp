#pragma once

#include "libs.hpp"
#include "logging.hpp"
#include "queues.hpp"
#include "frame.hpp"
#include "image.hpp"

namespace ASHInit {
    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    struct SwapChainBundle {
        vk::SwapchainKHR swapChain;
        std::vector<ASHUtil::SwapChainFrame> frames;
        vk::Format imageFormat;
        vk::Extent2D extent;
    };

    SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
        SwapChainSupportDetails details;

        details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
        details.formats = device.getSurfaceFormatsKHR(surface);
        details.presentModes = device.getSurfacePresentModesKHR(surface);

        #ifdef DEBUG
        std::cout << "Available surface capabilities:" << std::endl;
        std::cout << "\tminImageCount: " << details.capabilities.minImageCount << std::endl;
        std::cout << "\tmaxImageCount: " << details.capabilities.maxImageCount << std::endl;
        std::cout << "\tcurrentExtent: " << details.capabilities.currentExtent.width << "x" << details.capabilities.currentExtent.height << std::endl;
        std::cout << "\tminImageExtent: " << details.capabilities.minImageExtent.width << "x" << details.capabilities.minImageExtent.height << std::endl;
        std::cout << "\tmaxImageExtent: " << details.capabilities.maxImageExtent.width << "x" << details.capabilities.maxImageExtent.height << std::endl;
        std::cout << "\tmaxImageArrayLayers: " << details.capabilities.maxImageArrayLayers << std::endl;
        std::cout << "\tsupportedTransforms: " << std::endl;
        for (const auto &transform : parseTransformBits(details.capabilities.supportedTransforms)) {
            std::cout << "\t\t" << transform << std::endl;
        }
        std::cout << "\tcurrentTransform: " << std::endl;
        for (const auto &transform : parseTransformBits(details.capabilities.currentTransform)) {
            std::cout << "\t\t" << transform << std::endl;
        }
        std::cout << "\tsupportedCompositeAlpha: " << std::endl;
        for (const auto &alpha : parseCompositeAlphaBits(details.capabilities.supportedCompositeAlpha)) {
            std::cout << "\t\t" << alpha << std::endl;
        }
        std::cout << "\tsupportedUsageFlags: " << std::endl;
        for (const auto &usage: parseImageUsageBits(details.capabilities.supportedUsageFlags)) {
            std::cout << "\t\t" << usage << std::endl;
        }

        std::cout << "Available surface formats:" << std::endl;
        for (const auto &format : details.formats) {
            std::cout << "\t" << vk::to_string(format.format) << ", " << vk::to_string(format.colorSpace) << std::endl;
        }

        std::cout << "Available present modes:" << std::endl;
        for (const auto &mode : details.presentModes) {
            std::cout << "\t" << parsePresentModeBits(mode) << std::endl;
        }
        #endif

        return details;
    }

    vk::SurfaceFormatKHR chooseSurfaceFormat(std::vector<vk::SurfaceFormatKHR> availableFormats) {
        for (const auto &availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR choosePresentMode(std::vector<vk::PresentModeKHR> availableModes) {
        #ifdef FORCE_IMMEDIATE_PRESENT
        #ifdef DEBUG
        std::cout << "Using immediate present mode" << std::endl;
        #endif
        return vk::PresentModeKHR::eImmediate;
        #endif

        for (const auto &availableMode : availableModes) {
            if (availableMode == vk::PresentModeKHR::eMailbox) {
                #ifdef DEBUG
                std::cout << "Using mailbox present mode" << std::endl;
                #endif
                return availableMode;
            }
        }

        #ifdef DEBUG
        std::cout << "Using FIFO present mode" << std::endl;
        #endif
        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D chooseSwapExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        }
        else {
            vk::Extent2D actualExtent = {width, height};

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    SwapChainBundle createSwapchain(vk::Device logialDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

        vk::SurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = choosePresentMode(swapChainSupport.presentModes);
        vk::Extent2D extent = chooseSwapExtent(width, height, swapChainSupport.capabilities);

        uint32_t imageCount = std::min(swapChainSupport.capabilities.maxImageCount, swapChainSupport.capabilities.minImageCount + 1);

        vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
            vk::SwapchainCreateFlagsKHR(),
            surface,
            imageCount,
            surfaceFormat.format,
            surfaceFormat.colorSpace,
            extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment
        );

        ASHUtil::QueueFamilyIndices indices = ASHUtil::findQueueFamilies(physicalDevice, surface);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        vk::SwapchainKHR swapChain = vk::SwapchainKHR(nullptr);

        SwapChainBundle bundle{};
        try {
            bundle.swapChain = logialDevice.createSwapchainKHR(createInfo);
        }
        catch (const vk::SystemError &err) {
            throw std::runtime_error("Failed to create swap chain");
        }

        std::vector<vk::Image> images = logialDevice.getSwapchainImagesKHR(bundle.swapChain);
        bundle.frames.resize(images.size());

        for (size_t i = 0; i < images.size(); i++) {
            bundle.frames[i].image = images[i];
            bundle.frames[i].imageView = ASHImage::createImageView(logialDevice, images[i], surfaceFormat.format);
        }

        bundle.imageFormat = surfaceFormat.format;
        bundle.extent = extent; 

        return bundle;
    }
}