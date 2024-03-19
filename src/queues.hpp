#pragma once

#include "libs.hpp"

namespace ASHUtil {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device, vk::SurfaceKHR surface, bool log = false) {
        QueueFamilyIndices indices;

        std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

        #ifdef DEBUG
        if (log) {
            std::cout << "Available queue families:" << std::endl;
            for (size_t i = 0; i < queueFamilies.size(); i++) {
                std::cout << "\t" << i << ": " << queueFamilies[i].queueCount << " queues" << std::endl;
            }
        }
        #endif

        int i = 0;
        for (vk::QueueFamilyProperties queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
                indices.graphicsFamily = i;

                #ifdef DEBUG
                if (log) {
                    std::cout << "Queue family " << i << " supports graphics" << std::endl;
                }
                #endif
            }

            if (device.getSurfaceSupportKHR(i, surface)) {
                indices.presentFamily = i;

                #ifdef DEBUG
                if (log) {
                    std::cout << "Queue family " << i << " supports present" << std::endl;
                }
                #endif
            }

            if (indices.isComplete()) {
                break;
            }
            i++;

        }

        return indices;
    }

}