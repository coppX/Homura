//
// Created by FDC on 2022/1/14.
//

#include <vulkanGfxPipeline.h>
#include <vulkanRenderPass.h>
#include <vulkanDevice.h>
#include <vulkanShader.h>
#include <debugUtils.h>
#include <vulkanLayout.h>

namespace Homura
{
    VulkanPipeline::VulkanPipeline(VulkanDevicePtr device)
        : mDevice{device}
        , mVertexInputState{}
        , mInputAssemblyState{}
        , mTessellationState{}
        , mViewportState{}
        , mRasterizationState{}
        , mMultisampleState{}
        , mDepthStencilState{}
        , mColorBlendState{}
        , mDynamicState{}
        , mPipeline{VK_NULL_HANDLE}
        , mPipelineLayout{std::make_shared<VulkanPipelineLayout>(device)}
        , mBlendAttachmentStates{}
        , mShaders{}
        , mViewports{}
        , mScissors{}
    {
        mVertexInputState.sType     = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        mInputAssemblyState.sType   = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        mTessellationState.sType    = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        mViewportState.sType        = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        mRasterizationState.sType   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        mMultisampleState.sType     = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        mDepthStencilState.sType    = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        mColorBlendState.sType      = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        mDynamicState.sType         = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    }

    VulkanPipeline::~VulkanPipeline()
    {

    }

    void VulkanPipeline::create(VulkanRenderPassPtr renderPass)
    {
        mRenderPass = renderPass;

        mInputAssemblyState.topology                        = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        mInputAssemblyState.primitiveRestartEnable          = VK_FALSE;

        mRasterizationState.depthBiasClamp                  = VK_FALSE;
        mRasterizationState.rasterizerDiscardEnable         = VK_FALSE;
        mRasterizationState.polygonMode                     = VK_POLYGON_MODE_FILL;
        mRasterizationState.lineWidth                       = 1.0f;
        mRasterizationState.cullMode                        = VK_CULL_MODE_BACK_BIT;
        mRasterizationState.frontFace                       = VK_FRONT_FACE_CLOCKWISE;
        mRasterizationState.depthBiasEnable                 = VK_FALSE;
        mRasterizationState.depthBiasConstantFactor         = 0.0f;
        mRasterizationState.depthBiasClamp                  = 0.0f;
        mRasterizationState.depthBiasSlopeFactor            = 0.0f;

        mMultisampleState.sampleShadingEnable               = VK_FALSE;
        mMultisampleState.rasterizationSamples              = VK_SAMPLE_COUNT_1_BIT;

        mDepthStencilState.depthTestEnable                  = VK_TRUE;
        mDepthStencilState.depthWriteEnable                 = VK_TRUE;
        mDepthStencilState.depthCompareOp                   = VK_COMPARE_OP_LESS;
        mDepthStencilState.depthBoundsTestEnable            = VK_FALSE;
        mDepthStencilState.stencilTestEnable                = VK_FALSE;

        std::vector<VkDynamicState> dynamicState{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        mDynamicState.dynamicStateCount                     = static_cast<uint32_t >(dynamicState.size());
        mDynamicState.pDynamicStates                        = dynamicState.data();

        VkPipelineColorBlendAttachmentState colorBlendState{};
        colorBlendState.colorWriteMask                      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendState.blendEnable                         = VK_FALSE;
        colorBlendState.srcColorBlendFactor                 = VK_BLEND_FACTOR_ONE;
        colorBlendState.dstColorBlendFactor                 = VK_BLEND_FACTOR_ZERO;
        colorBlendState.colorBlendOp                        = VK_BLEND_OP_ADD;
        colorBlendState.srcAlphaBlendFactor                 = VK_BLEND_FACTOR_ONE;
        colorBlendState.dstAlphaBlendFactor                 = VK_BLEND_FACTOR_ZERO;
        colorBlendState.alphaBlendOp                        = VK_BLEND_OP_ADD;
        mBlendAttachmentStates.push_back(colorBlendState);
    }

    void VulkanPipeline::destroy()
    {
        mPipelineLayout->destroy();

        if (mPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(mDevice->getHandle(), mPipeline, nullptr);
            mPipeline = VK_NULL_HANDLE;
        }
    }

    void VulkanPipeline::setShaders(VulkanShaderPtr shaders)
    {
        mShaders = shaders;
    }

    void VulkanPipeline::setViewports(const std::vector<VkViewport>& viewports)
    {
        mViewports = viewports;
    }

    void VulkanPipeline::setScissors(const std::vector<VkRect2D>& scissors)
    {
        mScissors = scissors;
    }

    void VulkanPipeline::build(VulkanDescriptorSetLayoutPtr descriptorSetLayout)
    {
        mPipelineLayout->create(descriptorSetLayout);

        std::vector<VkPipelineShaderStageCreateInfo> shaderCreateInfos{};
        std::vector<VulkanShaderEntityPtr>& shaders = mShaders->getShaders();
        for (auto& shader : shaders)
        {
            VkPipelineShaderStageCreateInfo shaderCreateInfo{};
            shaderCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderCreateInfo.stage  = shader->getStage();
            shaderCreateInfo.pName  = shader->getName().c_str();
            shaderCreateInfo.module = shader->getHandle();
            shaderCreateInfos.push_back(shaderCreateInfo);
        }
        mVertexInputState.vertexBindingDescriptionCount = mShaders->getVertexInputBindingDescriptionCount();
        mVertexInputState.pVertexBindingDescriptions = mShaders->getVertexBindingDesriptionData();
        mVertexInputState.vertexAttributeDescriptionCount = mShaders->getVertexAttributeDesriptionCount();
        mVertexInputState.pVertexAttributeDescriptions = mShaders->getVertexAttributeDesriptionData();


        mViewportState.viewportCount        = static_cast<uint32_t>(mViewports.size());
        mViewportState.pViewports           = mViewports.data();
        mViewportState.scissorCount         = static_cast<uint32_t>(mScissors.size());
        mViewportState.pScissors            = mScissors.data();

        mColorBlendState.attachmentCount    = static_cast<uint32_t>(mBlendAttachmentStates.size());
        mColorBlendState.pAttachments       = mBlendAttachmentStates.data();

        VkGraphicsPipelineCreateInfo  gfxPipelineInfo{};
        gfxPipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        gfxPipelineInfo.stageCount          = static_cast<uint32_t>(shaderCreateInfos.size());
        gfxPipelineInfo.pStages             = shaderCreateInfos.data();

        gfxPipelineInfo.pVertexInputState   = &mVertexInputState;
        gfxPipelineInfo.pInputAssemblyState = &mInputAssemblyState;
        gfxPipelineInfo.pViewportState      = &mViewportState;
        gfxPipelineInfo.pRasterizationState = &mRasterizationState;
        gfxPipelineInfo.pMultisampleState   = &mMultisampleState;
        gfxPipelineInfo.pDepthStencilState  = &mDepthStencilState;
        gfxPipelineInfo.pColorBlendState    = &mColorBlendState;
        gfxPipelineInfo.pDynamicState       = nullptr;
        gfxPipelineInfo.layout              = mPipelineLayout->getHandle();
        gfxPipelineInfo.renderPass          = mRenderPass->getHandle();
        gfxPipelineInfo.subpass             = 0;

        gfxPipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;
        gfxPipelineInfo.basePipelineIndex   = -1;

        if (mPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(mDevice->getHandle(), mPipeline, nullptr);
        }

        VERIFYVULKANRESULT(vkCreateGraphicsPipelines(mDevice->getHandle(), VK_NULL_HANDLE, 1, &gfxPipelineInfo, nullptr, &mPipeline));
    }
}