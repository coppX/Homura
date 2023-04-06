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
        explicit VulkanCommandPool(VulkanDevicePtr device);
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
        VulkanCommandBuffer(VulkanDevicePtr device, VulkanSwapChainPtr swapChain, VulkanCommandPoolPtr commandPool, VulkanFramebufferPtr framebuffer, VulkanPipelinePtr pipeline);
        ~VulkanCommandBuffer();

        void create();
        void destroy();
        void createSyncObj();

        VkCommandBuffer& getHandle()
        {
            return mCommandBuffers[mCurrentFrame];
        }

        void begin();
        void beginRenderPass(VulkanRenderPassPtr renderPass);
        void bindGraphicPipeline();
        void bindVertexBuffer(VulkanVertexBufferPtr buffer, uint32_t count);
        void bindIndexBuffer(VulkanIndexBufferPtr buffer, uint32_t count);
        void bindDescriptorSet();
        void draw(uint32_t vertexCount);
        void drawIndex(uint32_t indexCount);
        void drawIndirect(VulkanVertexBufferPtr buffer);
        void drawIndexIndirect(VulkanStagingBufferPtr buffer);
        void endRenderPass();
        void end();

        void draw();
        void drawFrame(VulkanRHIPtr rhi);

        void transferImageLayout(VkCommandBuffer commandBuffer, const VkImageMemoryBarrier& imageMemoryBarrier, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

        void blitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout,
                       VkImage dstImage, VkImageLayout dstImageLayout,
                       uint32_t regionCount, VkImageBlit* Regions, VkFilter filter);
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VulkanBuffer srcBuffer, VulkanBuffer dstBuffer, VkDeviceSize size);
        void copyBufferToTexture(VulkanBuffer Buffer, VulkanTexture2DPtr texture, uint32_t width, uint32_t height);
        void submitSync(VulkanQueuePtr queue, VkCommandBuffer commandBuffer, bool isSync);

    private:
        VulkanDevicePtr                 mDevice;
        VulkanSwapChainPtr              mSwapChain;
        VulkanFramebufferPtr            mFramebuffer;
        VulkanPipelinePtr               mPipeline;
        //  sync
        VulkanFencesPtr                 imageInFlight;
        VulkanFencesPtr                 inFlightFences;
        VulkanSemaphoresPtr             mImageAvailableSemaphores;
        VulkanSemaphoresPtr             mRenderFinishedSemaphores;

        VulkanCommandPoolPtr            mCommandPool;
        std::vector<VkCommandBuffer>    mCommandBuffers;

        uint32_t                        mCurrentFrame;
        uint32_t                        mMaxFrameCount;

        bool                            mHasIndexBuffer;
        uint32_t                        mBufferDataCount;
    };
}
#endif //HOMURA_VULKANCOMMANDBUFFER_H
