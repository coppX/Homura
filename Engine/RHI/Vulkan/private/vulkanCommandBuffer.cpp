//
// Created by 最上川 on 2022/1/9.
//

#include <vulkanCommandBuffer.h>
#include <vulkanGfxPipeline.h>
#include <vulkanDevice.h>
#include <vulkanQueue.h>
#include <vulkanBuffer.h>
#include <vulkanLayout.h>
#include <vulkanDescriptorSet.h>
#include <vulkanRenderPass.h>
#include <vulkanFramebuffer.h>
#include <vulkanFence.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanCommandPool::VulkanCommandPool(VulkanDevicePtr device)
            : mDevice{device}
            , mCommandPool{VK_NULL_HANDLE}
    {
        create();
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        destroy();
    }

    void VulkanCommandPool::create()
    {
        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.queueFamilyIndex = mDevice->getGraphicsQueue()->getFamilyIndex();
        VERIFYVULKANRESULT(vkCreateCommandPool(mDevice->getHandle(), &createInfo, nullptr, &mCommandPool));
    }

    void VulkanCommandPool::reset()
    {
        VERIFYVULKANRESULT(vkResetCommandPool(mDevice->getHandle(), mCommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
    }

    void VulkanCommandPool::destroy()
    {
        if (mCommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(mDevice->getHandle(), mCommandPool, nullptr);
            mCommandPool = VK_NULL_HANDLE;
        }
    }

    void VulkanCommandBuffer::create()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandBufferCount    = 1;
        allocInfo.commandPool           = mCommandPool->getHandle();
        allocInfo.level                 = mAsSecondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VERIFYVULKANRESULT(vkAllocateCommandBuffers(mDevice->getHandle(), &allocInfo, &mCommandBuffer));
    }

    void VulkanCommandBuffer::destroy()
    {
        if (mCommandBuffer != VK_NULL_HANDLE)
        {
            vkFreeCommandBuffers(mDevice->getHandle(), mCommandPool->getHandle(), 1, &mCommandBuffer);
            mCommandBuffer = VK_NULL_HANDLE;
        }
    }

    void VulkanCommandBuffer::beginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool           = mCommandPool->getHandle();
        allocInfo.commandBufferCount    = 1;
        VERIFYVULKANRESULT(vkAllocateCommandBuffers(mDevice->getHandle(), &allocInfo, &mCommandBuffer));

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VERIFYVULKANRESULT(vkBeginCommandBuffer(mCommandBuffer, &beginInfo));
    }

    void VulkanCommandBuffer::endSingleTimeCommands()
    {
        vkEndCommandBuffer(mCommandBuffer);

        submitSync(mDevice->getGraphicsQueue(), VK_NULL_HANDLE);
        vkFreeCommandBuffers(mDevice->getHandle(), mCommandPool->getHandle(), 1, &mCommandBuffer);
    }

    void VulkanCommandBuffer::begin(const VkCommandBufferUsageFlags flag, const VkCommandBufferInheritanceInfo &inheritance)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType             = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags             = flag;
        beginInfo.pInheritanceInfo  = &inheritance;
        VERIFYVULKANRESULT(vkBeginCommandBuffer(mCommandBuffer, &beginInfo));
    }

    void VulkanCommandBuffer::beginRenderPass(VulkanRenderPassPtr renderPass, VulkanFramebufferPtr framebuffer)
    {
        VkRenderPassBeginInfo Info{};
        Info.renderPass                 = renderPass->getHandle();
        Info.framebuffer                = framebuffer->getHandle();
        Info.renderArea.offset.x        = 0;
        Info.renderArea.offset.y        = 0;
        Info.renderArea.extent.width    = framebuffer->getExtent().width;
        Info.renderArea.extent.height   = framebuffer->getExtent().height;
        // todo
        Info.clearValueCount            = 0;
        Info.pClearValues               = nullptr;

        vkCmdBeginRenderPass(mCommandBuffer, &Info, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandBuffer::bindGraphicPipeline(VulkanPipelinePtr pipeline)
    {
        vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getHandle());
    }

    void VulkanCommandBuffer::bindVertexBuffer(std::vector<VulkanVertexBufferPtr>& buffers)
    {
        std::vector<VkDeviceSize> offsets(buffers.size(), 0);
        std::vector<VkBuffer> nativeBuffers;
        for (const auto& buffer : buffers)
        {
            nativeBuffers.push_back(buffer->getHandle());
        }

        vkCmdBindVertexBuffers(mCommandBuffer, 0, static_cast<uint32_t>(nativeBuffers.size()), nativeBuffers.data(), offsets.data());
    }

    void VulkanCommandBuffer::bindIndexBuffer(VulkanIndexBufferPtr buffer)
    {
        vkCmdBindIndexBuffer(mCommandBuffer, buffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanCommandBuffer::bindDescriptorSet(const VulkanPipelineLayoutPtr layout, const VulkanDescriptorSetPtr descriptorSet)
    {
        vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout->getHandle(), 0, descriptorSet->getCount(), descriptorSet->getData(), 0, nullptr);
    }

    void VulkanCommandBuffer::draw(uint32_t vertexCount)
    {
        vkCmdDraw(mCommandBuffer, vertexCount, 1, 0, 0);
    }

    void VulkanCommandBuffer::drawIndex(uint32_t indexCount)
    {
        vkCmdDrawIndexed(mCommandBuffer, indexCount, 1, 0, 0, 0);
    }

    void VulkanCommandBuffer::drawIndirect(VulkanVertexBufferPtr buffer)
    {
        vkCmdDrawIndirect(mCommandBuffer, buffer->getHandle(), 0, 1, sizeof(VkDrawIndirectCommand));
    }

    void VulkanCommandBuffer::drawIndexIndirect(VulkanStagingBufferPtr buffer)
    {
        vkCmdDrawIndexedIndirect(mCommandBuffer, buffer->getHandle(), 0, 1, sizeof(VkDrawIndexedIndirectCommand));
    }

    void VulkanCommandBuffer::endRenderPass()
    {
        vkCmdEndRenderPass(mCommandBuffer);
    }

    void VulkanCommandBuffer::end()
    {
        VERIFYVULKANRESULT(vkEndCommandBuffer(mCommandBuffer));
    }

    void VulkanCommandBuffer::submitSync(VulkanQueuePtr queue, VulkanFencesPtr fence)
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &mCommandBuffer;

//        VERIFYVULKANRESULT(vkQueueSubmit(queue->getHandle(), 1, &submitInfo, fence->getHandle()));
        VERIFYVULKANRESULT(vkQueueWaitIdle(queue->getHandle()));
    }

    void VulkanCommandBuffer::transferImageLayout(const VkImageMemoryBarrier& imageMemoryBarrier, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask)
    {
        vkCmdPipelineBarrier(
                mCommandBuffer,
                srcStageMask,
                dstStageMask,
                0,
                0, nullptr,// MemoryBarrier
                0, nullptr,//BufferMemoryBarrier
                1, &imageMemoryBarrier//imageMemoryBarrier
                );
    }

    void VulkanCommandBuffer::blitImage(VkImage srcImage, VkImageLayout srcImageLayout,
                   VkImage dstImage, VkImageLayout dstImageLayout,
                   uint32_t regionCount, VkImageBlit* Regions, VkFilter filter)
    {
        vkCmdBlitImage(mCommandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, Regions, filter);
    }
}