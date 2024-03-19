#pragma once

#include "libs.hpp"
#include "logging.hpp"
#include "queues.hpp"

namespace ASHInit {
    bool deviceIsSuitable(const vk::PhysicalDevice& device) {
        const std::vector<const char*> requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

        for (const char *extension : requiredExtensions) {
            bool found = false;
            for (const vk::ExtensionProperties &availableExtension : availableExtensions) {
                if (strcmp(extension, availableExtension.extensionName) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }

        return true;
    }

    vk::PhysicalDevice pickPhysicalDevice(vk::Instance &instance) {
        std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

        #ifdef DEBUG
        std::cout << "Available devices:" << std::endl;
        for (const auto &device : devices) {
            vk::PhysicalDeviceProperties properties = device.getProperties();
            std::cout << "\t" << properties.deviceName << std::endl;
        }
        #endif

        if (devices.empty()) {
            throw std::runtime_error("No devices found");
        }

        // return the best device
        for (const auto &device : devices) {
            vk::PhysicalDeviceProperties properties = device.getProperties();
            if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && deviceIsSuitable(device)) {
                return device;
            }
        }

        throw std::runtime_error("No suitable devices found");
        return nullptr;
    }

   
    vk::Device createDevice(const vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {
        ASHUtil::QueueFamilyIndices indices = ASHUtil::findQueueFamilies(physicalDevice, surface, true);
        std::vector<uint32_t> uniqueIndices;
        uniqueIndices.push_back(indices.graphicsFamily.value());
        if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
            uniqueIndices.push_back(indices.presentFamily.value());
        }
        float queuePriority = 1.0f;

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        for (uint32_t queueFamily : uniqueIndices) {
            queueCreateInfos.push_back(
                vk::DeviceQueueCreateInfo(
                    vk::DeviceQueueCreateFlags(),
                    queueFamily,
                    1,
                    &queuePriority
                )
            );
        }

        std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        // vk::PhysicalDeviceFeatures deviceFeatures = physicalDevice.getFeatures();
        vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

        std::vector<const char*> enabledLayers;
        #ifdef DEBUG
        enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
        #endif

        vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
            vk::DeviceCreateFlags(),
            queueCreateInfos.size(),
            queueCreateInfos.data(),
            enabledLayers.size(),
            enabledLayers.data(),
            deviceExtensions.size(),
            deviceExtensions.data(),
            &deviceFeatures
        );

        try {
            vk::Device device = physicalDevice.createDevice(deviceInfo);
            return device;
        }
        catch (const vk::SystemError &err) {
            throw std::runtime_error("Failed to create logical device");
        }

        return nullptr;

    }

    std::array<vk::Queue, 2> createQueues(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface) {
        ASHUtil::QueueFamilyIndices indices = ASHUtil::findQueueFamilies(physicalDevice, surface);

        return {
            device.getQueue(indices.graphicsFamily.value(), 0),
            device.getQueue(indices.presentFamily.value(), 0)
        };
    }
}