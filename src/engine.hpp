#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "libs.hpp"
#include "frame.hpp"
#include "scene.hpp"
#include "meshwrapper.hpp"
#include "image.hpp"

namespace ASH {
    class Engine
    {
    public:
        Engine(int width, int height, GLFWwindow *window);
        ~Engine();

        void render(Scene *scene);

    private:
        int m_width;
        int m_height;

        GLFWwindow *m_window;

        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_debugMessenger;
        vk::DispatchLoaderDynamic m_dispatchLoader;
        vk::SurfaceKHR m_surface;

        vk::PhysicalDevice m_physicalDevice;
        vk::Device m_device;
        vk::Queue m_graphicsQueue;
        vk::Queue m_presentQueue;
        vk::SwapchainKHR m_swapchain;
        std::vector<ASHUtil::SwapChainFrame> m_swapchainFrames;
        vk::Format m_swapchainFormat;
        vk::Extent2D m_swapchainExtent;

        vk::Pipeline m_pipeline;
        vk::PipelineLayout m_pipelineLayout;
        vk::RenderPass m_renderPass;

        vk::CommandPool m_commandPool;
        vk::CommandBuffer m_primaryCommandBuffer;

        int m_maxFramesInFlight, m_currentFrame;

        vk::DescriptorSetLayout m_frameSetLayout;
        vk::DescriptorPool m_framePool;
        vk::DescriptorSetLayout m_meshSetLayout;
        vk::DescriptorPool m_meshPool;

        MeshWrapper* m_meshes;
        std::unordered_map<meshTypes, ASHImage::Texture*> m_materials;

        void createInstance();

        void createDevice();
        void createSwapchain();
        void recreateSwapchain();
        void destroySwapchain();

        void createDescriptorSetLayouts();
        void createPipeline();

        void finishSetup();
        void createFramebuffers();
        void createFrameResources();

        void createAssets();
        void prepScene(vk::CommandBuffer commandBuffer);
        void prepFrame(uint32_t imageIndex, Scene *scene);

        void recordCommands(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Scene *scene);
        void renderObjects(vk::CommandBuffer commandBuffer, meshTypes type, uint32_t& startInstance, uint32_t instanceCount);
    };
}