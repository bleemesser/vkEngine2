#pragma once

#include "libs.hpp"

namespace ASHUtil {
    std::vector<char> readFile(const std::string &filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        #ifdef DEBUG
        std::cout << "Opened file: " << filename << std::endl;
        #endif

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    vk::ShaderModule createShaderModule(std::string filename, vk::Device device) {
        std::vector<char> code = readFile(filename);

        vk::ShaderModuleCreateInfo createInfo = {};
        createInfo.flags = vk::ShaderModuleCreateFlags();
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        try {
            return device.createShaderModule(createInfo);
        } catch (const vk::SystemError &err) {
            throw std::runtime_error("Failed to create shader module: " + filename);
        }
    }
}