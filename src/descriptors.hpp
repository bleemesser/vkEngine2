#pragma once

#include "libs.hpp"

namespace ASHInit {
    struct DescriptorSetLayoutData {
        int count;
        std::vector<int> indices;
        std::vector<vk::DescriptorType> types;
        std::vector<int> counts;
        std::vector<vk::ShaderStageFlags> stages;
    };

    vk::DescriptorSetLayout createDescriptorSetLayout(vk::Device device, DescriptorSetLayoutData& bindings);

    vk::DescriptorPool createDescriptorPool(vk::Device device, uint32_t size, const DescriptorSetLayoutData& bindings);

    vk::DescriptorSet allocateDescriptorSet(
        vk::Device device,
        vk::DescriptorPool pool,
        vk::DescriptorSetLayout layout
    );
}