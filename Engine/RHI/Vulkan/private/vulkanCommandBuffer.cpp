//
// Created by 最上川 on 2022/1/9.
//

#include <vulkanCommandBuffer.h>
#include <vulkanCommandPool.h>
#include <vulkanGfxPipeline.h>
#include <vulkanDevice.h>
#include <vulkanQueue.h>
#include <vulkanBuffer.h>
#include <vulkanLayout.h>
#include <vulkanDescriptorSet.h>
#include <debugUtils.h>

namespace Homura
{
    void VulkanCommandBuffer::create()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandBufferCount = 1;
        allocInfo.commandPool = mCommandPool->getHandle();
        allocInfo.level = mAsSecondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;

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
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = mCommandPool->getHandle();
        allocInfo.commandBufferCount = 1;
        VERIFYVULKANRESULT(vkAllocateCommandBuffers(mDevice->getHandle(), &allocInfo, &mCommandBuffer));

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VERIFYVULKANRESULT(vkBeginCommandBuffer(mCommandBuffer, &beginInfo));
    }

    void VulkanCommandBuffer::endSingleTimeCommands()
    {
        vkEndCommandBuffer(mCommandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mCommandBuffer;

        vkQueueSubmit(mDevice->getGraphicsQueue()->getHandle(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(mDevice->getGraphicsQueue()->getHandle());

        vkFreeCommandBuffers(mDevice->getHandle(), mCommandPool->getHandle(), 1, &mCommandBuffer);
    }

    void VulkanCommandBuffer::begin(const VkCommandBufferUsageFlags flag, const VkCommandBufferInheritanceInfo &inheritance)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = flag;
        beginInfo.pInheritanceInfo = &inheritance;
        VERIFYVULKANRESULT(vkBeginCommandBuffer(mCommandBuffer, &beginInfo));
    }

    void VulkanCommandBuffer::beginRenderPass(const VkRenderPassBeginInfo &renderPassBeginInfo, const VkSubpassContents &subPassContents)
    {
        vkCmdBeginRenderPass(mCommandBuffer, &renderPassBeginInfo, subPassContents);
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

    void VulkanCommandBuffer::draw(size_t vertexCount)
    {
        vkCmdDraw(mCommandBuffer, vertexCount, 1, 0, 0);
    }

    void VulkanCommandBuffer::drawIndex(size_t indexCount)
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
}