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
    class VulkanPipeline
    {
    public:
        VulkanPipeline(VulkanDevicePtr device);
        ~VulkanPipeline();

        void create(VulkanRenderPassPtr renderPass, VkSampleCountFlagBits samples);
        void destroy();

        void setShaders(VulkanShaderPtr shaders);
        void setViewports(const std::vector<VkViewport>& viewports);
        void setScissors(const std::vector<VkRect2D>& scissors);

        void build(VulkanDescriptorSetPtr descriptorSet);
        VkPipeline& getHandle()
        {
            return mPipeline;
        }

        VulkanPipelineLayoutPtr getPipelineLayout()
        {
            return mPipelineLayout;
        }

        VulkanDescriptorSetPtr getDescriptorSet()
        {
            return mDescriptSet;
        }
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
        VkPipelineDynamicStateCreateInfo                    mDynamicState;

        VkPipeline                                          mPipeline;
        VulkanDescriptorSetPtr                              mDescriptSet;
        VulkanPipelineLayoutPtr                             mPipelineLayout;

        std::vector<VkPipelineColorBlendAttachmentState>    mBlendAttachmentStates{};
        VulkanShaderPtr                                     mShaders;
        std::vector<VkViewport>                             mViewports;
        std::vector<VkRect2D>                               mScissors;
    };
}
#endif //HOMURA_VULKANGFXPIPELINE_H
