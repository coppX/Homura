//
// Created by 最上川 on 2022/1/9.
//

#include <vulkanCommandBuffer.h>
#include <vulkanCommandPool.h>
#include <vulkanDevice.h>
#include <vulkanQueue.h>
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
}