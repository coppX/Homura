//
// Created by 最上川 on 2022/1/9.
//

#include <vulkanBuffer.h>
#include <debugUtils.h>
#include <vulkanDevice.h>
#include <vulkanCommandBuffer.h>

namespace Homura
{
    VulkanBuffer::VulkanBuffer(VulkanDevicePtr device, VulkanCommandBufferPtr commandBuffer, VkDeviceSize size, VkBufferUsageFlags  usage, VkMemoryPropertyFlags props)
        : mBuffer{VK_NULL_HANDLE}
        , mBufferMemory{VK_NULL_HANDLE}
        , mDevice{device}
        , mCommandBuffer{commandBuffer}
        , mSize{size}
        , mUsage{usage}
        , mProperties{props}
        , mStagingBuffer{nullptr}
    {
        create();
    }

    VulkanBuffer::~VulkanBuffer()
    {
        
    }

    void VulkanBuffer::create()
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType            = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size             = mSize;
        bufferInfo.usage            = mUsage;
        bufferInfo.sharingMode      = VK_SHARING_MODE_EXCLUSIVE;

        VERIFYVULKANRESULT(vkCreateBuffer(mDevice->getHandle(), &bufferInfo, nullptr, &mBuffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(mDevice->getHandle(), mBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType             = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize    = memRequirements.size;
        allocInfo.memoryTypeIndex   = findMemoryType(memRequirements.memoryTypeBits, mProperties);

        VERIFYVULKANRESULT(vkAllocateMemory(mDevice->getHandle(), &allocInfo, nullptr, &mBufferMemory));
        VERIFYVULKANRESULT(vkBindBufferMemory(mDevice->getHandle(), mBuffer, mBufferMemory, 0));
    }

    void VulkanBuffer::destroy()
    {
        if (mBuffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(mDevice->getHandle(), mBuffer, nullptr);
            mBuffer = VK_NULL_HANDLE;
        }

        if (mBufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(mDevice->getHandle(), mBufferMemory, nullptr);
            mBufferMemory = VK_NULL_HANDLE;
        }

        if (mStagingBuffer)
        {
            mStagingBuffer->destroy();
            delete mStagingBuffer;
            mStagingBuffer = nullptr;
        }
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

    void VulkanBuffer::updateBufferByStaging(void *pData, uint32_t size)
    {
        mStagingBuffer = new VulkanBuffer(mDevice, mCommandBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        mStagingBuffer->fillBuffer(pData, size);
        copyBuffer(*mStagingBuffer, *this, static_cast<VkDeviceSize>(size));
    }

    void VulkanBuffer::copyBuffer(VulkanBuffer& srcBuffer, VulkanBuffer& dstBuffer, VkDeviceSize size)
    {
        mCommandBuffer->copyBuffer(srcBuffer, dstBuffer, size);
    }

    void VulkanBuffer::copyToTexture(VulkanTexture2DPtr texture, uint32_t width, uint32_t height)
    {
        mCommandBuffer->copyBufferToTexture(*this, texture, width, height);
    }

}