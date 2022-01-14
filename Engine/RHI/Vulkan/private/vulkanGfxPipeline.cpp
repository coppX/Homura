//
// Created by FDC on 2022/1/14.
//

#include <vulkanGfxPipeline.h>
#include <vulkanRenderPass.h>
#include <vulkanDevice.h>
#include <vulkanShader.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanPipeline::VulkanPipeline(VulkanDevicePtr device, VulkanRenderPassPtr renderPass)
        : mDevice{device}
        , mRenderPass{renderPass}
        , mVertexInputState{}
        , mInputAssemblyState{}
        , mTessellationState{}
        , mViewportState{}
        , mRasterizationState{}
        , mMultisampleState{}
        , mDepthStencilState{}
        , mColorBlendState{}
        , mPipeline{VK_NULL_HANDLE}
        , mPipelineLayout{VK_NULL_HANDLE}
        , mBlendAttachmentStates{}
        , mShaders{}
        , mViewports{}
        , mScissors{}
    {
        mVertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        mInputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        mTessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        mViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        mRasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        mMultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        mDepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        mColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    }

    void VulkanPipeline::setShaders(const std::vector<VulkanShaderPtr>& shaders)
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

    VulkanPipeline::~VulkanPipeline()
    {
        if (mPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(mDevice->getHandle(), mPipelineLayout, nullptr);
        }

        if (mPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(mDevice->getHandle(), mPipeline, nullptr);
        }
    }

    void VulkanPipeline::build()
    {
        std::vector<VkPipelineShaderStageCreateInfo> shaderCreateInfos{};
        for (const auto& shader : mShaders)
        {
            VkPipelineShaderStageCreateInfo shaderCreateInfo{};
            shaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderCreateInfo.stage = shader->getStage();
            shaderCreateInfo.pName = shader->getEntryPointName();
            shaderCreateInfo.module = shader->getHandle();

            shaderCreateInfos.push_back(shaderCreateInfo);
        }
        mViewportState.viewportCount = static_cast<uint32_t>(mViewports.size());
        mViewportState.pViewports = mViewports.data();
        mViewportState.scissorCount = static_cast<uint32_t>(mScissors.size());
        mViewportState.pScissors = mScissors.data();

        mColorBlendState.attachmentCount = static_cast<uint32_t>(mBlendAttachmentStates.size());
        mColorBlendState.pAttachments = mBlendAttachmentStates.data();

        if (mPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(mDevice->getHandle(), mPipelineLayout, nullptr);
        }
        VkPipelineLayoutCreateInfo  layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        VERIFYVULKANRESULT(vkCreatePipelineLayout(mDevice->getHandle(), &layoutInfo, nullptr, &mPipelineLayout));

        VkGraphicsPipelineCreateInfo  gfxPipelineInfo{};
        gfxPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        gfxPipelineInfo.stageCount = static_cast<uint32_t>(shaderCreateInfos.size());
        gfxPipelineInfo.pStages = shaderCreateInfos.data();

        gfxPipelineInfo.pVertexInputState = &mVertexInputState;
        gfxPipelineInfo.pInputAssemblyState = &mInputAssemblyState;
        gfxPipelineInfo.pViewportState = &mViewportState;
        gfxPipelineInfo.pRasterizationState = &mRasterizationState;
        gfxPipelineInfo.pMultisampleState = &mMultisampleState;
        gfxPipelineInfo.pDepthStencilState = &mDepthStencilState;
        gfxPipelineInfo.pColorBlendState = &mColorBlendState;
        gfxPipelineInfo.layout = mPipelineLayout;
        gfxPipelineInfo.renderPass = mRenderPass->getHandle();
        gfxPipelineInfo.subpass = 0;

        gfxPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        gfxPipelineInfo.basePipelineIndex = -1;

        if (mPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(mDevice->getHandle(), mPipeline, nullptr);
        }

        VERIFYVULKANRESULT(vkCreateGraphicsPipelines(mDevice->getHandle(), VK_NULL_HANDLE, 1, &gfxPipelineInfo, nullptr, &mPipeline));
    }
}