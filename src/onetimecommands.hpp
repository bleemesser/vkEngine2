#pragma once

#include "libs.hpp"

namespace ASHUtil {
    void startJob(vk::CommandBuffer commandBuffer);

    void endJob(vk::CommandBuffer commandBuffer, vk::Queue queue);
}