#include "descriptors.hpp"

vk::DescriptorSetLayout ASHInit::createDescriptorSetLayout(vk::Device device, DescriptorSetLayoutData& bindings) {
    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
    layoutBindings.reserve(bindings.count);

    for (int i = 0; i < bindings.count; i++) {
        vk::DescriptorSetLayoutBinding layoutBinding;
        layoutBinding.binding = bindings.indices[i];
        layoutBinding.descriptorType = bindings.types[i];
        layoutBinding.descriptorCount = bindings.counts[i];
        layoutBinding.stageFlags = bindings.stages[i];
        layoutBindings.push_back(layoutBinding);
    }

    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.flags = vk::DescriptorSetLayoutCreateFlags();
    layoutInfo.bindingCount = bindings.count;
    layoutInfo.pBindings = layoutBindings.data();

    try {
        return device.createDescriptorSetLayout(layoutInfo);
    } catch (vk::SystemError err) {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}

vk::DescriptorPool ASHInit::createDescriptorPool(vk::Device device, uint32_t size, const DescriptorSetLayoutData& bindings) {
    std::vector<vk::DescriptorPoolSize> poolSizes;

    for (int i = 0; i < bindings.count; i++) {
        vk::DescriptorPoolSize poolSize;
        poolSize.type = bindings.types[i];
        poolSize.descriptorCount = size;
        poolSizes.push_back(poolSize);
    }

    vk::DescriptorPoolCreateInfo poolInfo;
    poolInfo.flags = vk::DescriptorPoolCreateFlags();
    poolInfo.maxSets = size;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();

    try {
        return device.createDescriptorPool(poolInfo);
    } catch (vk::SystemError err) {
        throw std::runtime_error("Failed to create descriptor pool");
    }
}

vk::DescriptorSet ASHInit::allocateDescriptorSet(
    vk::Device device,
    vk::DescriptorPool pool,
    vk::DescriptorSetLayout layout
) {
    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    try {
        return device.allocateDescriptorSets(allocInfo)[0];
    } catch (vk::SystemError err) {
        throw std::runtime_error("Failed to allocate descriptor set");
    }
}
