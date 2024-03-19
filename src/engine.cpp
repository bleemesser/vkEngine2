#include "engine.hpp"

#include "instance.hpp"
#include "logging.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "framebuffer.hpp"
#include "commands.hpp"
#include "sync.hpp"
#include "descriptors.hpp"
#include "obj.hpp"

namespace ASH {
    Engine::Engine(int width, int height, GLFWwindow *window) : m_width(width), m_height(height), m_window(window) {
        createInstance();
        createDevice();
        createDescriptorSetLayouts();
        createPipeline();
        finishSetup();
        createAssets();
    }

    Engine::~Engine() {
        m_device.waitIdle();

        m_device.destroyCommandPool(m_commandPool);

        m_device.destroyPipeline(m_pipeline);
        m_device.destroyPipelineLayout(m_pipelineLayout);
        m_device.destroyRenderPass(m_renderPass);

        destroySwapchain();

        m_device.destroyDescriptorSetLayout(m_frameSetLayout);
        // frame descriptor pool is destroyed in destroySwapchain

        delete m_meshes;

        for (const auto& [key, texture] : m_materials) {
            delete texture;
        }

        m_device.destroyDescriptorSetLayout(m_meshSetLayout);
        m_device.destroyDescriptorPool(m_meshPool);

        m_device.destroy();
        #ifdef DEBUG
        m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger, nullptr, m_dispatchLoader);
        #endif
        m_instance.destroySurfaceKHR(m_surface);

        m_instance.destroy();
    }

    void Engine::createInstance() {
        m_instance = ASHInit::createInstance("Vulkan");
        m_dispatchLoader = vk::DispatchLoaderDynamic(m_instance, vkGetInstanceProcAddr);

        #ifdef DEBUG
        m_debugMessenger = ASHInit::setupDebugMessenger(m_instance, m_dispatchLoader);
        #endif

        if (glfwCreateWindowSurface(m_instance, m_window, nullptr, reinterpret_cast<VkSurfaceKHR *>(&m_surface)) != VK_SUCCESS) {
            std::cerr << "Failed to create window surface" << std::endl;
            exit(1);
        }
    }

    void Engine::createDevice() {
        m_physicalDevice = ASHInit::pickPhysicalDevice(m_instance);
        m_device = ASHInit::createDevice(m_physicalDevice, m_surface);
        std::array<vk::Queue, 2> queues = ASHInit::createQueues(m_physicalDevice, m_device, m_surface);
        m_graphicsQueue = queues[0];
        m_presentQueue = queues[1];
        createSwapchain();
        m_currentFrame = 0;
    }

    void Engine::createSwapchain() {
        ASHInit::SwapChainBundle bundle = ASHInit::createSwapchain(m_device, m_physicalDevice, m_surface, m_width, m_height);
        m_swapchain = bundle.swapChain;
        m_swapchainFrames = bundle.frames;
        m_swapchainFormat = bundle.imageFormat;
        m_swapchainExtent = bundle.extent;

        m_maxFramesInFlight = static_cast<int>(m_swapchainFrames.size());

        for (ASHUtil::SwapChainFrame& frame : m_swapchainFrames) {
            frame.device = m_device;
            frame.physicalDevice = m_physicalDevice;
            frame.width = m_swapchainExtent.width;
            frame.height = m_swapchainExtent.height;

            frame.createDepthResources();
        }
    }

    void Engine::recreateSwapchain() {
        m_width = 0;
        m_height = 0;
        while (m_width == 0 || m_height == 0) {
            glfwGetFramebufferSize(m_window, &m_width, &m_height);
            glfwWaitEvents();
        }

        m_device.waitIdle();
        #ifdef DEBUG
        std::cout << "Recreating swapchain" << std::endl;
        #endif
        destroySwapchain();
        createSwapchain();
        createFramebuffers();
        createFrameResources();
        ASHInit::CommandBufferInput cbInput = {m_device, m_commandPool, m_swapchainFrames};
        ASHInit::createFrameCommandBuffers(cbInput);

    }

    void Engine::createDescriptorSetLayouts() {
        ASHInit::DescriptorSetLayoutData bindings;
        bindings.count = 2;
        bindings.indices.push_back(0);
        bindings.types.push_back(vk::DescriptorType::eUniformBuffer);
        bindings.counts.push_back(1);
        bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        bindings.indices.push_back(1);
        bindings.types.push_back(vk::DescriptorType::eStorageBuffer);
        bindings.counts.push_back(1);
        bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        m_frameSetLayout = ASHInit::createDescriptorSetLayout(m_device, bindings);

        bindings.count = 1;

        bindings.indices[0] = 0;
        bindings.types[0] = vk::DescriptorType::eCombinedImageSampler;
        bindings.counts[0] = 1;
        bindings.stages[0] = vk::ShaderStageFlagBits::eFragment;

        m_meshSetLayout = ASHInit::createDescriptorSetLayout(m_device, bindings);
    }

    void Engine::createPipeline() {
        ASHInit::GraphicsPipelineInputBundle input{};
        input.device = m_device;
        input.vertFilePath = "shaders/shader.vert.spv";
        input.fragFilePath = "shaders/shader.frag.spv";
        input.swapchainExtent = m_swapchainExtent;
        input.swapchainImageFormat = m_swapchainFormat;
        input.depthFormat = m_swapchainFrames[0].depthFormat;
        input.descriptorSetLayouts = {m_frameSetLayout, m_meshSetLayout};
        ASHInit::GraphicsPipelineOutputBundle output = ASHInit::createGraphicsPipeline(input);
        m_pipeline = output.pipeline;
        m_pipelineLayout = output.layout;
        m_renderPass = output.renderPass;

    }

    void Engine::createFrameResources() {
        ASHInit::DescriptorSetLayoutData bindings;
        bindings.count = 2;
        bindings.types.push_back(vk::DescriptorType::eUniformBuffer);
        bindings.types.push_back(vk::DescriptorType::eStorageBuffer);
        m_framePool = ASHInit::createDescriptorPool(m_device, static_cast<uint32_t>(m_swapchainFrames.size()), bindings);

        for (ASHUtil::SwapChainFrame& frame: m_swapchainFrames) {
            frame.inFlightFence = ASHInit::createFence(m_device);
            frame.imageAvailableSemaphore = ASHInit::createSemaphore(m_device);
            frame.renderFinishedSemaphore = ASHInit::createSemaphore(m_device);

            frame.createDescriptorResources();

            frame.descriptorSet = ASHInit::allocateDescriptorSet(m_device, m_framePool, m_frameSetLayout);
        }
    }

    void Engine::finishSetup() {
        createFramebuffers();

        m_commandPool = ASHInit::createCommandPool(m_device, m_physicalDevice, m_surface);

        ASHInit::CommandBufferInput cbInput = {m_device, m_commandPool, m_swapchainFrames};
        m_primaryCommandBuffer = ASHInit::createCommandBuffer(cbInput);
        ASHInit::createFrameCommandBuffers(cbInput);

        createFrameResources();
    }

    void Engine::createFramebuffers() {
        ASHInit::FramebufferInput fbInput{};
        fbInput.device = m_device;
        fbInput.renderPass = m_renderPass;
        fbInput.swapchainExtent = m_swapchainExtent;
        ASHInit::createFramebuffers(fbInput, m_swapchainFrames);
    }

    void Engine::createAssets() {
        #ifdef DEBUG
        std::cout << yellow("Loading assets") << std::endl;
        #endif
        m_meshes = new MeshWrapper();

        std::unordered_map<meshTypes, std::vector<const char*>> modelPaths = {
            {meshTypes::GROUND, {"models/quad.obj","models/quad.mtl"}},
            {meshTypes::GIRL, {"models/girl.obj","models/girl.mtl"}},
            {meshTypes::SKULL, {"models/skull.obj","models/skull.mtl"}}
        };


        for (std::pair<meshTypes, std::vector<const char*>> pair : modelPaths) {
            ASHModel::Obj model(pair.second[0], pair.second[1], glm::mat4(1.f));
            m_meshes->consume(pair.first, model.vertices, model.indices);
        }

        FinalizationChunk finalizationInfo{};
        finalizationInfo.device = m_device;
        finalizationInfo.physicalDevice = m_physicalDevice;
        finalizationInfo.queue = m_graphicsQueue;
        finalizationInfo.commandBuffer = m_primaryCommandBuffer;
        m_meshes->finalize(finalizationInfo);

        std::unordered_map<meshTypes, const char*> filenames = {
            {meshTypes::GROUND, "models/quad.jpg"},
            {meshTypes::GIRL, "textures/none.png"},
            {meshTypes::SKULL, "models/skull.png"}
        };

        
        ASHInit::DescriptorSetLayoutData bindings;
        bindings.count = 1;
        bindings.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        m_meshPool = ASHInit::createDescriptorPool(m_device, static_cast<uint32_t>(filenames.size()), bindings);

        ASHImage::TextureInput input{};
        input.commandBuffer = m_primaryCommandBuffer;
        input.queue = m_graphicsQueue;
        input.device = m_device;
        input.physicalDevice = m_physicalDevice;
        input.layout = m_meshSetLayout;
        input.pool = m_meshPool;

        for (const auto & [object, filename] : filenames) {
            input.path = filename;
            m_materials[object] = new ASHImage::Texture(input);
        }

        #ifdef DEBUG
        std::cout << green("Assets loaded") << std::endl;
        #endif
    }

    void Engine::prepScene(vk::CommandBuffer commandBuffer) {
        vk::Buffer vertexBuffers[] = {m_meshes->m_vertexBuffer.buffer};
        vk::DeviceSize offsets[] = {0};
        commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
        commandBuffer.bindIndexBuffer(m_meshes->m_indexBuffer.buffer, 0, vk::IndexType::eUint32);
    }

    void Engine::prepFrame(uint32_t imageIndex, Scene *scene) {

        ASHUtil::SwapChainFrame& _frame = m_swapchainFrames[imageIndex];

        glm::vec3 eye = { -1.0f, 0.0f, 1.0f };
        glm::vec3 center = { 1.0f, 0.0f, 1.0f };
        glm::vec3 up = { 0.0f, 0.0f, 1.0f };
        glm::mat4 view = glm::lookAt(eye, center, up);
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), m_swapchainExtent.width / (float) m_swapchainExtent.height, 0.1f, 100.0f);
        
        // glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);
        projection[1][1] *= -1; // flip y coordinate

        _frame.cameraData.view = view;
        _frame.cameraData.projection = projection;
        _frame.cameraData.viewProjection = projection * view; // premul for performance

        memcpy(_frame.cameraDataWritePtr, &_frame.cameraData, sizeof(ASHUtil::UBO));

        size_t i = 0;

        for (std::pair<meshTypes, std::vector<glm::vec3>> pair : scene->positions) {
            for (glm::vec3& position : pair.second) {
                _frame.modelMatrices[i++] = glm::translate(glm::mat4(1.0f), position);
            }
        }

        memcpy(_frame.modelMatrixWritePtr, _frame.modelMatrices.data(), i * sizeof(glm::mat4));

        _frame.writeDescriptorSet();
    }

    void Engine::recordCommands(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Scene *scene) {
        vk::CommandBufferBeginInfo beginInfo{};

        try {
            commandBuffer.begin(beginInfo);
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to begin recording command buffer");
        }

        vk::RenderPassBeginInfo renderPassInfo{};
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_swapchainFrames[imageIndex].framebuffer;
        renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
        renderPassInfo.renderArea.extent = m_swapchainExtent;

        

        vk::ClearValue colorClear;
        std::array<float, 4> colors = { 0.2f,0.2f,0.2f, 1.0f };
        colorClear.color = vk::ClearColorValue(colors);
        vk::ClearValue depthClear;

        depthClear.depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
        std::vector<vk::ClearValue> clearValues = { {colorClear, depthClear} };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        

        commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);

        

        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0, m_swapchainFrames[imageIndex].descriptorSet, nullptr);

        

        prepScene(commandBuffer);

        

        uint32_t startInstance = 0;
        for (std::pair<meshTypes, std::vector<glm::vec3>> pair : scene->positions) {
            renderObjects(commandBuffer, pair.first, startInstance, static_cast<uint32_t>(pair.second.size()));
        }

        

        commandBuffer.endRenderPass();

        try {
            commandBuffer.end();
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to end recording command buffer");
        }
    }

    void Engine::renderObjects(vk::CommandBuffer commandBuffer, meshTypes type, uint32_t& startInstance, uint32_t instanceCount) {
        

        int indexCount = m_meshes->m_indexCounts.find(type)->second;
        int firstIndex = m_meshes->m_firstIndices.find(type)->second;

        

        m_materials[type]->use(commandBuffer, m_pipelineLayout);

        
        commandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, 0, startInstance);

        
        startInstance += instanceCount;
    }

    void Engine::render(Scene *scene) {
        m_device.waitForFences(1, &(m_swapchainFrames[m_currentFrame].inFlightFence), VK_TRUE, UINT64_MAX);
        m_device.resetFences(1, &(m_swapchainFrames[m_currentFrame].inFlightFence));


        uint32_t imageIndex;
        try {
            vk::ResultValue acquire = m_device.acquireNextImageKHR(m_swapchain, UINT64_MAX, m_swapchainFrames[m_currentFrame].imageAvailableSemaphore, nullptr);
            imageIndex = acquire.value;
        } catch (vk::OutOfDateKHRError) {
            recreateSwapchain();
            return;
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to acquire swap chain image");
        }

        vk::CommandBuffer commandBuffer = m_swapchainFrames[m_currentFrame].commandBuffer;

        commandBuffer.reset();

        prepFrame(imageIndex, scene);

        recordCommands(commandBuffer, imageIndex, scene);

        vk::SubmitInfo submitInfo{};
        vk::Semaphore waitSemaphores[] = {m_swapchainFrames[m_currentFrame].imageAvailableSemaphore};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        vk::Semaphore signalSemaphores[] = {m_swapchainFrames[m_currentFrame].renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        

        try {
            m_graphicsQueue.submit(submitInfo, m_swapchainFrames[m_currentFrame].inFlightFence);
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to submit draw command buffer");
        }

        

        vk::PresentInfoKHR presentInfo{};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        vk::SwapchainKHR swapChains[] = {m_swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        vk::Result presentResult;
        try {
            presentResult = m_presentQueue.presentKHR(presentInfo);
        } catch (vk::OutOfDateKHRError) {
            presentResult = vk::Result::eErrorOutOfDateKHR;
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to present swap chain image");
        }

        if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR) {
            recreateSwapchain();
            return;
        } else if (presentResult != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to present swap chain image");
        }

        m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;

        
    }

    void Engine::destroySwapchain() {
        for (ASHUtil::SwapChainFrame& frame : m_swapchainFrames) {
            frame.destroy();
        }

        m_device.destroySwapchainKHR(m_swapchain);

        m_device.destroyDescriptorPool(m_framePool);
    }

}