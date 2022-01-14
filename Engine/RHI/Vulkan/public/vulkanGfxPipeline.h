//
// Created by FDC on 2022/1/14.
//

#ifndef HOMURA_VULKANGFXPIPELINE_H
#define HOMURA_VULKANGFXPIPELINE_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura
{
    class VulkanDevice;
    class VulkanRenderPass;

    class VulkanPipeline
    {
    public:
        VulkanPipeline(VulkanDevicePtr device, VulkanRenderPassPtr renderPass);
        ~VulkanPipeline();

    private:
        VulkanDevicePtr                                     mDevice;
        VulkanRenderPassPtr                                 mRenderPass;

        VkPipelineVertexInputStateCreateInfo                mVertexInputState;
        VkPipelineInputAssemblyStateCreateInfo              mInputAssemblyState;
        VkPipelineTessellationStateCreateInfo               mTessellationState;
        VkPipelineViewportStateCreateInfo                   mViewportState;
        VkPipelineRasterizationStateCreateInfo              mRasterizationState;
        VkPipelineMultisampleStateCreateInfo                mMultisampleState;
        VkPipelineDepthStencilStateCreateInfo               mDepthStencilState;
        VkPipelineColorBlendStateCreateInfo                 mColorBlendState;

        VkPipeline                                          mPipeline;
        VkPipelineLayout                                    mPipelineLayout;

        std::vector<VkPipelineColorBlendAttachmentState>    mBlendAttachmentStates{};
    };
}
#endif //HOMURA_VULKANGFXPIPELINE_H
