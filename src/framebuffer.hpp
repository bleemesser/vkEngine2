#pragma once

#include "libs.hpp"
#include "frame.hpp"

namespace ASHInit {
    struct FramebufferInput {
        vk::Device device;
        vk::RenderPass renderPass;
        vk::Extent2D swapchainExtent;
    };
    
    void createFramebuffers(FramebufferInput input, std::vector<ASHUtil::SwapChainFrame>& frames) {
        for (int i = 0; i < frames.size(); ++i) {
            std::vector<vk::ImageView> attachments = {
                frames[i].imageView
            };

            vk::FramebufferCreateInfo framebufferInfo{};
            framebufferInfo.flags = vk::FramebufferCreateFlags();
            framebufferInfo.renderPass = input.renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = input.swapchainExtent.width;
            framebufferInfo.height = input.swapchainExtent.height;
            framebufferInfo.layers = 1;

            try {
                frames[i].framebuffer = input.device.createFramebuffer(framebufferInfo);
                #ifdef DEBUG
                std::cout << "Framebuffer " << i << " created" << std::endl;
                #endif
            } catch (vk::SystemError err) {
                throw std::runtime_error("Failed to create framebuffer");
            }
        }
    }
}