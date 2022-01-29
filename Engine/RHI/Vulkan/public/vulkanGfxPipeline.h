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
        VulkanPipeline(VulkanDevicePtr device, VulkanRenderPassPtr renderPass);
        ~VulkanPipeline();

        void create();
        void destroy();

        void setShaders(const std::vector<VulkanShaderPtr>& shaders);
        void setViewports(const std::vector<VkViewport>& viewports);
        void setScissors(const std::vector<VkRect2D>& scissors);

        void build(VulkanDescriptorSetLayoutPtr descriptorSetLayout);
        VkPipeline& getHandle()
        {
            return mPipeline;
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

        VkPipeline                                          mPipeline;
        VulkanPipelineLayoutPtr                             mPipelineLayout;

        std::vector<VkPipelineColorBlendAttachmentState>    mBlendAttachmentStates{};
        std::vector<VulkanShaderPtr>                        mShaders;
        std::vector<VkViewport>                             mViewports;
        std::vector<VkRect2D>                               mScissors;
    };
}
#endif //HOMURA_VULKANGFXPIPELINE_H
