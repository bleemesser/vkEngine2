#pragma once

#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <optional>
#include <fstream>
#include <unordered_map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define DEBUG
#define FORCE_IMMEDIATE_PRESENT

// function to make text green
inline std::string green(const std::string &text) {
    return "\033[1;32m" + text + "\033[0m";
}

// function to make text red
inline std::string red(const std::string &text) {
    return "\033[1;31m" + text + "\033[0m";
}

// function to make text yellow
inline std::string yellow(const std::string &text) {
    return "\033[1;33m" + text + "\033[0m";
}

// Buffer creation structs

struct BufferInput {
    size_t size;
    vk::BufferUsageFlags usage;
    vk::Device device;
    vk::PhysicalDevice physicalDevice;
    vk::MemoryPropertyFlags properties;
};


struct Buffer {
    vk::Buffer buffer;
    vk::DeviceMemory memory;
};

enum class meshTypes {
    TRIANGLE,
    SQUARE,
    STAR
};