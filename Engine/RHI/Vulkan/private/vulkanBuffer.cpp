//
// Created by 最上川 on 2022/1/9.
//

#include <vulkanBuffer.h>
#include <debugUtils.h>
#include <vulkanDevice.h>

namespace Homura
{
    VulkanBuffer::VulkanBuffer(std::shared_ptr<VulkanDevice> device, VkDeviceSize size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags props, BufferType type)
        : mBuffer{VK_NULL_HANDLE}
        , mBufferMemory{VK_NULL_HANDLE}
        , mDevice{device}
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


}