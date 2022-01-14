//
// Created by FDC on 2022/1/14.
//

#include <vulkanGfxPipeline.h>
#include <vulkanRenderPass.h>
#include <vulkanDevice.h>

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
}