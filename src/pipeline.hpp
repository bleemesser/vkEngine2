#pragma once

#include "libs.hpp"
#include "shaders.hpp"
#include "renderstructs.hpp"
#include "mesh.hpp"

namespace ASHInit {
    struct GraphicsPipelineInputBundle {
        vk::Device device;
        std::string vertFilePath;
        std::string fragFilePath;
        vk::Extent2D swapchainExtent;
        vk::Format swapchainImageFormat, depthFormat;
        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    };

    struct GraphicsPipelineOutputBundle {
        vk::Pipeline pipeline;
        vk::PipelineLayout layout;
        vk::RenderPass renderPass;
    };
    // Declarations
    vk::PipelineVertexInputStateCreateInfo createVertexInputInfo(
		const vk::VertexInputBindingDescription& bindingDescription,
		const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions
    );

	vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyInfo();
    
    vk::PipelineShaderStageCreateInfo createShaderInfo(
		const vk::ShaderModule& shaderModule, const vk::ShaderStageFlagBits& stage
    );

    vk::Viewport createViewport(const GraphicsPipelineInputBundle& input);

    vk::Rect2D createScissor(const GraphicsPipelineInputBundle& input);

    vk::PipelineViewportStateCreateInfo createViewportStateInfo(
        const vk::Viewport& viewport, const vk::Rect2D& scissor
    );

    vk::PipelineRasterizationStateCreateInfo createRasterizerInfo();

    vk::PipelineMultisampleStateCreateInfo createMultisamplingInfo();

    vk::PipelineColorBlendAttachmentState createColorBlendAttachmentState();

    vk::PipelineColorBlendStateCreateInfo createColorBlendAttachmentStage(
		const vk::PipelineColorBlendAttachmentState& colorBlendAttachment
    );

    vk::PipelineLayout createPipelineLayout(vk::Device device, std::vector<vk::DescriptorSetLayout> descriptorSetLayouts);

    vk::PushConstantRange createPushConstantInfo();

    vk::RenderPass createRenderPass(
        vk::Device device, const vk::Format swapchainImageFormat, const vk::Format depthFormat
    );

    vk::AttachmentDescription createColorAttachment(
        const vk::Format& swapchainImageFormat
    );

    vk::AttachmentReference createColorAttachmentRef();

    vk::AttachmentDescription createDepthAttachment(
        const vk::Format& depthFormat
    );

    vk::AttachmentReference createDepthAttachmentRef();

    vk::SubpassDescription createSubpass(
        const std::vector<vk::AttachmentReference>& attachments
    );

    vk::RenderPassCreateInfo createRenderPassInfo(
        const std::vector<vk::AttachmentDescription>& attachments,
        const vk::SubpassDescription& subpass
    );

    // Main creation function
    GraphicsPipelineOutputBundle createGraphicsPipeline(GraphicsPipelineInputBundle spec) {
        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.flags = vk::PipelineCreateFlags();

        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

        // Vertex Input
        vk::VertexInputBindingDescription bindingDescription = ASHModel::getPosColorBindingDescription();
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = ASHModel::getPosColorAttributeDescriptions();
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = createVertexInputInfo(bindingDescription, attributeDescriptions);
        pipelineInfo.pVertexInputState = &vertexInputInfo;

        // Input Assembly
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly = createInputAssemblyInfo();
        pipelineInfo.pInputAssemblyState = &inputAssembly;

        // Vertex Shader
        vk::ShaderModule vertShader = ASHUtil::createShaderModule(spec.vertFilePath, spec.device);
        vk::PipelineShaderStageCreateInfo vertShaderInfo = createShaderInfo(vertShader, vk::ShaderStageFlagBits::eVertex);
        shaderStages.push_back(vertShaderInfo);

        // Viewport and Scissor
        vk::Viewport viewport = createViewport(spec);
        vk::Rect2D scissor = createScissor(spec);
        vk::PipelineViewportStateCreateInfo viewportState = createViewportStateInfo(viewport, scissor);
        pipelineInfo.pViewportState = &viewportState;

        // Rasterizer
        vk::PipelineRasterizationStateCreateInfo rasterizer = createRasterizerInfo();
        pipelineInfo.pRasterizationState = &rasterizer;

        // Fragment Shader
        vk::ShaderModule fragShader = ASHUtil::createShaderModule(spec.fragFilePath, spec.device);
        vk::PipelineShaderStageCreateInfo fragShaderInfo = createShaderInfo(fragShader, vk::ShaderStageFlagBits::eFragment);
        shaderStages.push_back(fragShaderInfo);

        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages = shaderStages.data();

        // Depth test
        vk::PipelineDepthStencilStateCreateInfo depthState;
        depthState.flags = vk::PipelineDepthStencilStateCreateFlags();
        depthState.depthTestEnable = VK_TRUE;
        depthState.depthWriteEnable = VK_TRUE;
        depthState.depthCompareOp = vk::CompareOp::eLess;
        depthState.depthBoundsTestEnable = VK_FALSE;
        depthState.stencilTestEnable = VK_FALSE;
        pipelineInfo.pDepthStencilState = &depthState;

        // Multisampling
        vk::PipelineMultisampleStateCreateInfo multisampling = createMultisamplingInfo();
        pipelineInfo.pMultisampleState = &multisampling;

        // Color Blending
        vk::PipelineColorBlendAttachmentState colorBlendAttachment = createColorBlendAttachmentState();
        vk::PipelineColorBlendStateCreateInfo colorBlending = createColorBlendAttachmentStage(colorBlendAttachment);
        pipelineInfo.pColorBlendState = &colorBlending;

        // Pipeline Layout
        vk::PipelineLayout pipelineLayout = createPipelineLayout(spec.device, spec.descriptorSetLayouts);
        pipelineInfo.layout = pipelineLayout;

        // Render Pass
        vk::RenderPass renderPass = createRenderPass(spec.device, spec.swapchainImageFormat, spec.depthFormat);
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = nullptr;

        vk::Pipeline pipeline;
        try {
            pipeline = spec.device.createGraphicsPipeline(nullptr, pipelineInfo).value;
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to create graphics pipeline");
        }

        spec.device.destroyShaderModule(vertShader);
        spec.device.destroyShaderModule(fragShader);

        GraphicsPipelineOutputBundle output{};
        output.pipeline = pipeline;
        output.layout = pipelineLayout;
        output.renderPass = renderPass;

        return output;
    }

    // Implementations

    vk::PipelineVertexInputStateCreateInfo createVertexInputInfo(
        const vk::VertexInputBindingDescription& bindingDescription,
        const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions
    ) {
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        return vertexInputInfo;
    }

    vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyInfo() {
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.flags = vk::PipelineInputAssemblyStateCreateFlags();
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        return inputAssembly;
    }

    vk::PipelineShaderStageCreateInfo createShaderInfo(
        const vk::ShaderModule& shaderModule, const vk::ShaderStageFlagBits& stage
    ) {
        vk::PipelineShaderStageCreateInfo shaderStage{};
        shaderStage.flags = vk::PipelineShaderStageCreateFlags();
        shaderStage.stage = stage;
        shaderStage.module = shaderModule;
        shaderStage.pName = "main";
        return shaderStage;
    }

    vk::Viewport createViewport(const GraphicsPipelineInputBundle& input) {
        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(input.swapchainExtent.width);
        viewport.height = static_cast<float>(input.swapchainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        return viewport;
    }

    vk::Rect2D createScissor(const GraphicsPipelineInputBundle& input) {
        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D{0, 0};
        scissor.extent = input.swapchainExtent;
        return scissor;
    }

    vk::PipelineViewportStateCreateInfo createViewportStateInfo(
        const vk::Viewport& viewport, const vk::Rect2D& scissor
    ) {
        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.flags = vk::PipelineViewportStateCreateFlags();
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        return viewportState;
    }

    vk::PipelineRasterizationStateCreateInfo createRasterizerInfo() {
        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
        rasterizer.depthBiasEnable = VK_FALSE;
        return rasterizer;
    }

    vk::PipelineMultisampleStateCreateInfo createMultisamplingInfo() {
        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
        return multisampling;
    }

    vk::PipelineColorBlendAttachmentState createColorBlendAttachmentState() {
        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;
        return colorBlendAttachment;
    }

    vk::PipelineColorBlendStateCreateInfo createColorBlendAttachmentStage(
        const vk::PipelineColorBlendAttachmentState& colorBlendAttachment
    ) {
        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
        return colorBlending;
    }

    vk::PipelineLayout createPipelineLayout(vk::Device device, std::vector<vk::DescriptorSetLayout> descriptorSetLayouts) {
        vk::PipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.flags = vk::PipelineLayoutCreateFlags();

        layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        layoutInfo.pSetLayouts = descriptorSetLayouts.data();

        layoutInfo.pushConstantRangeCount = 0;

        try {
            return device.createPipelineLayout(layoutInfo);
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to create pipeline layout");
        }

    }

    vk::RenderPass createRenderPass(
        vk::Device device, const vk::Format swapchainImageFormat, const vk::Format depthFormat
    ) {
        std::vector<vk::AttachmentDescription> attachments;
        std::vector<vk::AttachmentReference> attachmentRefs;

        attachments.push_back(createColorAttachment(swapchainImageFormat));
        attachmentRefs.push_back(createColorAttachmentRef());

        attachments.push_back(createDepthAttachment(depthFormat));
        attachmentRefs.push_back(createDepthAttachmentRef());

        vk::SubpassDescription subpass = createSubpass(attachmentRefs);
        vk::RenderPassCreateInfo renderPassInfo = createRenderPassInfo(attachments, subpass);

        vk::RenderPass renderPass;
        try {
            renderPass = device.createRenderPass(renderPassInfo);
        } catch (vk::SystemError err) {
            throw std::runtime_error("Failed to create render pass");
        }

        return renderPass;
    }

    vk::AttachmentDescription createColorAttachment(
        const vk::Format& swapchainImageFormat
    ) {
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.flags = vk::AttachmentDescriptionFlags();
        colorAttachment.format = swapchainImageFormat;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
        return colorAttachment;
    }

    vk::AttachmentReference createColorAttachmentRef() {
        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
        return colorAttachmentRef;
    }

    vk::AttachmentDescription createDepthAttachment(
        const vk::Format& depthFormat
    ) {
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.flags = vk::AttachmentDescriptionFlags();
        colorAttachment.format = depthFormat;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        return colorAttachment;
    }

    vk::AttachmentReference createDepthAttachmentRef() {
        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 1;
        colorAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        return colorAttachmentRef;
    }



    vk::SubpassDescription createSubpass(
        const std::vector<vk::AttachmentReference>& attachments
    ) {
        vk::SubpassDescription subpass{};
        subpass.flags = vk::SubpassDescriptionFlags();
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &attachments[0];
        subpass.pDepthStencilAttachment = &attachments[1];

        return subpass;
    }

    vk::RenderPassCreateInfo createRenderPassInfo(
        const std::vector<vk::AttachmentDescription>& attachments,
        const vk::SubpassDescription& subpass
    ) {
        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.flags = vk::RenderPassCreateFlags();
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        return renderPassInfo;
    }
    
}