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
        VulkanCommandBuffer(VulkanDevicePtr device, VulkanSwapChainPtr swapChain, VulkanCommandPoolPtr commandPool, VulkanFramebufferPtr framebuffer, VulkanPipelinePtr pipeline);
        ~VulkanCommandBuffer();

        void create();
        void destroy();

        VkCommandBuffer& getHandle()
        {
            return mCommandBuffers[mCurrentFrameIndex];
        }

        void begin();
        void beginRenderPass(VulkanRenderPassPtr renderPass);
        void bindGraphicPipeline();
        void bindVertexBuffer(VulkanVertexBufferPtr buffer);
        void bindIndexBuffer(VulkanIndexBufferPtr buffer);
        void bindDescriptorSet();
        void draw(uint32_t vertexCount);
        void drawIndex(uint32_t indexCount);
        void drawIndirect(VulkanVertexBufferPtr buffer);
        void drawIndexIndirect(VulkanStagingBufferPtr buffer);
        void endRenderPass();
        void end();

        void draw();
        void beginFrame();
        void endFrame();

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
        void begin(uint32_t index);
        void beginRenderPass(VulkanRenderPassPtr renderPass, uint32_t index);
        void bindGraphicPipeline(VulkanPipelinePtr pipeline, uint32_t index);
        void bindVertexBuffer(std::vector<VulkanVertexBufferPtr>& buffers, uint32_t index);
        void bindIndexBuffer(VulkanIndexBufferPtr buffer, uint32_t index);
        void bindDescriptorSet(const VulkanPipelineLayoutPtr layout, const VulkanDescriptorSetPtr descriptorSet, uint32_t index);
        void draw(uint32_t vertexCount, uint32_t index);
        void drawIndex(uint32_t indexCount, uint32_t index);
        void drawIndirect(VulkanVertexBufferPtr buffer, uint32_t index);
        void drawIndexIndirect(VulkanStagingBufferPtr buffer, uint32_t index);
        void endRenderPass(uint32_t index);
        void end(uint32_t index);

        VulkanDevicePtr                 mDevice;
        VulkanSwapChainPtr              mSwapChain;
        VulkanFramebufferPtr            mFramebuffrer;
        VulkanPipelinePtr               mPipeline;
        //  sync
        VulkanFencesPtr                 imageInFlight;
        VulkanFencesPtr                 inFlightFences;
        VulkanSemaphoresPtr             mImageAvailableSemaphores;
        VulkanSemaphoresPtr             mRenderFinishedSemaphores;

        VulkanCommandPoolPtr            mCommandPool;
        std::vector<VkCommandBuffer>    mCommandBuffers;

        uint32_t                        mCurrentFrameIndex;
        uint32_t                        mMaxFrameCount;
        uint32_t                        mImageIndex;

        bool                            mHasIndexBuffer;
        uint32_t                        mBuferSize;
    };
}
#endif //HOMURA_VULKANCOMMANDBUFFER_H
