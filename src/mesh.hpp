#pragma once

#include "libs.hpp"

namespace ASHModel {
    vk::VertexInputBindingDescription getPosColorBindingDescription() {
        vk::VertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        // x y r g b u v
        bindingDescription.stride = 7 * sizeof(float);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
    }

    std::vector<vk::VertexInputAttributeDescription> getPosColorAttributeDescriptions() {
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.resize(3);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32Sfloat; // 2D position
        attributeDescriptions[0].offset = 0;

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat; // 3D color
        attributeDescriptions[1].offset = 2 * sizeof(float);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32Sfloat; // 2D UV
        attributeDescriptions[2].offset = 5 * sizeof(float);

        return attributeDescriptions;
    }
}