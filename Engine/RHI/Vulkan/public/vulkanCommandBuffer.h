//
// Created by 最上川 on 2022/1/9.
//

#ifndef HOMURA_VULKANCOMMANDBUFFER_H
#define HOMURA_VULKANCOMMANDBUFFER_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura
{
    class VulkanCommandBuffer
    {
    public:
        VulkanCommandBuffer(VulkanDevicePtr device, VulkanCommandPoolPtr commandPool, bool asSecondary = false)
            : mCommandBuffer{VK_NULL_HANDLE}
            , mDevice{device}
            , mCommandPool{commandPool}
            , mAsSecondary(asSecondary)
        {
            create();
        }

        ~VulkanCommandBuffer()
        {
            destroy();
        }

        void create();
        void destroy();

        VkCommandBuffer& getHandle()
        {
            return mCommandBuffer;
        }

        void begin(const VkCommandBufferUsageFlags flag, const VkCommandBufferInheritanceInfo& inheritance);

        void beginRenderPass(const VkRenderPassBeginInfo& renderPassBeginInfo, const VkSubpassContents& subPassContents);

        void bindGraphicPipeline(VulkanPipelinePtr pipeline);

        void bindVertexBuffer(std::vector<VulkanVertexBufferPtr>& buffers);

        void bindIndexBuffer(VulkanIndexBufferPtr buffer);

        void bindDescriptorSet();

        void beginSingleTimeCommands();

        void endSingleTimeCommands();
    private:
        VulkanDevicePtr         mDevice;
        VulkanCommandPoolPtr    mCommandPool;
        VkCommandBuffer         mCommandBuffer;
        bool                    mAsSecondary;
    };
}
#endif //HOMURA_VULKANCOMMANDBUFFER_H
