//
// Created by 最上川 on 2022/1/9.
//

#ifndef HOMURA_VULKANBUFFER_H
#define HOMURA_VULKANBUFFER_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>

namespace Homura
{
    class VulkanBuffer
    {
    public:
        VulkanBuffer(std::shared_ptr<VulkanDevice> device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props);

        ~VulkanBuffer();

        void fillBuffer(void *inData, uint64_t size);

        void copyBuffer(VulkanBuffer& srcBuffer, VulkanBuffer& dstBuffer, VkDeviceSize size);

        void copyToImage(VkImage image, uint32_t width, uint32_t height);

        void updateBufferByStaging(void *pData, uint32_t size);
        VkBuffer getHandle()
        {
            return mBuffer;
        }

        VkDeviceSize& getSize()
        {
            return mSize;
        }

    private:
        uint32_t  findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        VulkanDevicePtr         mDevice;
        VulkanCommandBufferPtr  mCommandBuffer;

        VkDeviceSize            mSize;
        VkBufferUsageFlags      mUsage;
        VkMemoryPropertyFlags   mProperties;
        VkBuffer                mBuffer;
        VkDeviceMemory          mBufferMemory;
    };

    class VulkanVertexBuffer : public VulkanBuffer
    {
    public:
        VulkanVertexBuffer(VulkanDevicePtr device, VkDeviceSize size, void* pData)
            : VulkanBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        {
            updateBufferByStaging(pData, static_cast<uint32_t>(size));
        }
    };

    class VulkanIndexBuffer : public VulkanBuffer
    {
    public:
        VulkanIndexBuffer(VulkanDevicePtr device, VkDeviceSize size, void* pData)
                : VulkanBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        {
            updateBufferByStaging(pData, static_cast<uint32_t>(size));
        }
    };

    class VulkanUniformBuffer : public VulkanBuffer
    {
    public:
        VulkanUniformBuffer(VulkanDevicePtr device, VkDeviceSize size, void* pData)
            : VulkanBuffer(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        {
            if (pData)
            {
                updateBufferByStaging(pData, static_cast<uint32_t>(size));
            }
        }
    };

    class VulkanStagingBuffer : public VulkanBuffer
    {
    public:
        VulkanStagingBuffer(VulkanDevicePtr device, VkDeviceSize size, void* pData)
            : VulkanBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        {
            if (pData)
            {
                updateBufferByStaging(pData, static_cast<uint32_t>(size));
            }
        }
    };
}
#endif //HOMURA_VULKANBUFFER_H
