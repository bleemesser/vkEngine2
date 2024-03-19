#pragma once

#include "libs.hpp"

namespace ASHInit
{
    bool checkSupport(std::vector<const char*> &extensions, std::vector<const char*> &layers) {
        std::vector<vk::ExtensionProperties> availableExtensions = vk::enumerateInstanceExtensionProperties();

        #ifdef DEBUG
        std::cout << green("Available extensions:") << std::endl;
        for (const vk::ExtensionProperties &extension : availableExtensions)
        {
            std::cout << extension.extensionName << std::endl;
        }
        #endif

        std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

        #ifdef DEBUG
        std::cout << green("Available layers:") << std::endl;
        for (const vk::LayerProperties &layer : availableLayers)
        {
            std::cout << layer.layerName << std::endl;
        }
        #endif

        for (const char *extension : extensions)
        {
            bool found = false;
            for (const vk::ExtensionProperties &availableExtension : availableExtensions)
            {
                if (strcmp(extension, availableExtension.extensionName) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                #ifdef DEBUG
                std::cerr << "Extension not found: " << extension << std::endl;
                #endif
                return false;
            }
        }

        for (const char *layer : layers)
        {
            bool found = false;
            for (const vk::LayerProperties &availableLayer : availableLayers)
            {
                if (strcmp(layer, availableLayer.layerName) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                #ifdef DEBUG
                std::cerr << "Layer not found: " << layer << std::endl;
                #endif
                return false;
            }
        }

        return true;
    }

    vk::Instance createInstance(const std::string &name)
    {
        uint32_t version;
        vkEnumerateInstanceVersion(&version);
        std::cout << green("Vulkan version: ") << VK_VERSION_MAJOR(version) << "." << VK_VERSION_MINOR(version) << "." << VK_VERSION_PATCH(version) << std::endl;

        version = VK_MAKE_API_VERSION(0, 1, 1, 0);

        vk::ApplicationInfo appInfo(
            name.c_str(),
            VK_MAKE_VERSION(1, 0, 0),
            "ASH",
            VK_MAKE_VERSION(1, 0, 0),
            version);

        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        std::vector<const char *> layers;

        #ifdef DEBUG
        layers.push_back("VK_LAYER_KHRONOS_validation");
        extensions.push_back("VK_EXT_debug_utils");
        #endif

        #ifdef DEBUG
        std::cout << yellow("Required extensions:") << std::endl;
        for (const char *extension : extensions)
        {
            std::cout << extension << std::endl;
        }
        std::cout << yellow("Required layers:") << std::endl;
        for (const char *layer : layers)
        {
            std::cout << layer << std::endl;
        }
        #endif

        if (!checkSupport(extensions, layers))
        {
            throw std::runtime_error("Required extensions or layers are not supported");
        } else {
            std::cout << green("All required extensions and layers are supported") << std::endl;
        }

        vk::InstanceCreateInfo createInfo(
            vk::InstanceCreateFlags(),
            &appInfo,
            static_cast<uint32_t>(layers.size()), layers.data(),
            static_cast<uint32_t>(extensions.size()), extensions.data());

        try
        {
            return vk::createInstance(createInfo);
        }
        catch (const vk::SystemError &err)
        {
            throw std::runtime_error("Failed to create instance");
        }
    }
}