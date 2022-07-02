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
    class VulkanCommandPool
    {
    public:
        VulkanCommandPool(VulkanDevicePtr device);
        ~VulkanCommandPool();

        void create();
        void reset();
        void destroy();

        VkCommandPool& getHandle()
        {
            return mCommandPool;
        }
    private:
        VulkanDevicePtr             mDevice;
        VkCommandPool               mCommandPool;
    };

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

        void beginRenderPass(VulkanRenderPassPtr renderPass, VulkanFramebufferPtr framebuffer);

        void bindGraphicPipeline(VulkanPipelinePtr pipeline);

        void bindVertexBuffer(std::vector<VulkanVertexBufferPtr>& buffers);

        void bindIndexBuffer(VulkanIndexBufferPtr buffer);

        void bindDescriptorSet(const VulkanPipelineLayoutPtr layout, const VulkanDescriptorSetPtr descriptorSet);

        void draw(uint32_t vertexCount);

        void drawIndex(uint32_t indexCount);

        void drawIndirect(VulkanVertexBufferPtr buffer);

        void drawIndexIndirect(VulkanStagingBufferPtr buffer);

        void endRenderPass();

        void end();

        void submitSync(VulkanQueuePtr queue, VulkanFencesPtr fence);

        void transferImageLayout(const VkImageMemoryBarrier& imageMemoryBarrier, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

        void blitImage(VkImage srcImage, VkImageLayout srcImageLayout,
                       VkImage dstImage, VkImageLayout dstImageLayout,
                       uint32_t regionCount, VkImageBlit* Regions, VkFilter filter);
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
