//
// Created by 最上川 on 2022/1/9.
//

#include <vulkanBuffer.h>
#include <debugUtils.h>
#include <vulkanDevice.h>
#include <vulkanCommandBuffer.h>

namespace Homura
{
    VulkanBuffer::VulkanBuffer(VulkanDevicePtr device, VkDeviceSize size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags props, BufferType type)
        : mBuffer{VK_NULL_HANDLE}
        , mBufferMemory{VK_NULL_HANDLE}
        , mDevice{device}
        , mCommandBuffer{std::make_shared<VulkanCommandBuffer>(mDevice)}
        , mSize{size}
        , mUsage{usage}
        , mProperties{props}
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = mSize;
        bufferInfo.usage = mUsage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VERIFYVULKANRESULT(vkCreateBuffer(mDevice->getHandle(), &bufferInfo, nullptr, &mBuffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(mDevice->getHandle(), mBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, props);

        VERIFYVULKANRESULT(vkAllocateMemory(mDevice->getHandle(), &allocInfo, nullptr, &mBufferMemory));

        vkBindBufferMemory(mDevice->getHandle(), mBuffer, mBufferMemory, 0);
    }

    uint32_t VulkanBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(mDevice->getPhysicalHandle(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        return -1;
    }
    void VulkanBuffer::fillBuffer(void *inData, uint64_t size)
    {
        void* data;
        vkMapMemory(mDevice->getHandle(), mBufferMemory, 0, mSize, 0, &data);
        memcpy(data, inData, (size_t)size);
        vkUnmapMemory(mDevice->getHandle(), mBufferMemory);
    }

    void VulkanBuffer::copyToBuffer(VulkanBuffer &dstBuffer, VkDeviceSize size)
    {
        mCommandBuffer->beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(mCommandBuffer->getHandle(), mBuffer, dstBuffer.mBuffer, 1, &copyRegion);

        mCommandBuffer->endSingleTimeCommands();
    }

    void VulkanBuffer::copyToImage(VkImage image, uint32_t width, uint32_t height)
    {
        mCommandBuffer->beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0,};
        region.imageExtent = {
                width,
                height,
                1
        };

        vkCmdCopyBufferToImage(mCommandBuffer->getHandle(), mBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        mCommandBuffer->endSingleTimeCommands();
    }

}