#pragma once

#include "libs.hpp"

namespace ASHInit {
    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData
    ) {
        std::cerr << red("Validation layer: ") << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    vk::DebugUtilsMessengerEXT setupDebugMessenger(vk::Instance &instance, vk::DispatchLoaderDynamic &dispatchLoader) {
        vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
            vk::DebugUtilsMessengerCreateFlagsEXT(),
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            debugCallback,
            nullptr
        );

        return instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dispatchLoader);
    }

    // parse transform bits to strings
    std::vector<std::string> parseTransformBits(vk::SurfaceTransformFlagsKHR bits) {
        std::vector<std::string> result;

        if (bits & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
            result.push_back("Identity");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate90) {
            result.push_back("Rotate 90");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate180) {
            result.push_back("Rotate 180");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate270) {
            result.push_back("Rotate 270");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror) {
            result.push_back("Horizontal mirror");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90) {
            result.push_back("Horizontal mirror rotate 90");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180) {
            result.push_back("Horizontal mirror rotate 180");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270) {
            result.push_back("Horizontal mirror rotate 270");
        }
        if (bits & vk::SurfaceTransformFlagBitsKHR::eInherit) {
            result.push_back("Inherit");
        }

        return result;
    }

    // parse composite alpha bits to strings
    std::vector<std::string> parseCompositeAlphaBits(vk::CompositeAlphaFlagsKHR bits) {
        std::vector<std::string> result;

        if (bits & vk::CompositeAlphaFlagBitsKHR::eOpaque) {
            result.push_back("Opaque");
        }
        if (bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {
            result.push_back("Pre-multiplied");
        }
        if (bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) {
            result.push_back("Post-multiplied");
        }
        if (bits & vk::CompositeAlphaFlagBitsKHR::eInherit) {
            result.push_back("Inherit");
        }

        return result;
    }

    // parse image usage bits to strings
    std::vector<std::string> parseImageUsageBits(vk::ImageUsageFlags bits) {
        std::vector<std::string> result;

        if (bits & vk::ImageUsageFlagBits::eTransferSrc) {
            result.push_back("Transfer source");
        }
        if (bits & vk::ImageUsageFlagBits::eTransferDst) {
            result.push_back("Transfer destination");
        }
        if (bits & vk::ImageUsageFlagBits::eSampled) {
            result.push_back("Sampled");
        }
        if (bits & vk::ImageUsageFlagBits::eStorage) {
            result.push_back("Storage");
        }
        if (bits & vk::ImageUsageFlagBits::eColorAttachment) {
            result.push_back("Color attachment");
        }
        if (bits & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
            result.push_back("Depth stencil attachment");
        }
        if (bits & vk::ImageUsageFlagBits::eTransientAttachment) {
            result.push_back("Transient attachment");
        }
        if (bits & vk::ImageUsageFlagBits::eInputAttachment) {
            result.push_back("Input attachment");
        }

        return result;
    }

    // parse present mode bits to strings
    std::string parsePresentModeBits(vk::PresentModeKHR bits) {
        if (bits == vk::PresentModeKHR::eFifo) {
            return "FIFO";
        }
        if (bits == vk::PresentModeKHR::eFifoRelaxed) {
            return "FIFO relaxed";
        }
        if (bits == vk::PresentModeKHR::eImmediate) {
            return "Immediate";
        }
        if (bits == vk::PresentModeKHR::eMailbox) {
            return "Mailbox";
        }

        return "Unknown";
    }
    
}