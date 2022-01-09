//
// Created by 最上川 on 2022/1/9.
//

#ifndef HOMURA_VULKANBUFFER_H
#define HOMURA_VULKANBUFFER_H
#include <vulkan/vulkan.h>
#include <memory>

namespace Homura
{
    class VulkanDevice;

    enum class BufferType
    {
        Vertex,
        Index,
        Staging,
        //Uniform,
    };

    class VulkanBuffer
    {
    public:
        VulkanBuffer(std::shared_ptr<VulkanDevice> device, VkDeviceSize size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags props, BufferType type);

        void fillBuffer(void *inData, uint64_t size);

        //void copyToBuffer(VulkanBuffer dstBuffer, VkDeviceSize size);

        ~VulkanBuffer();
    private:
        uint32_t  findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        std::shared_ptr<VulkanDevice>   mDevice;

        BufferType              mBufferType;
        VkDeviceSize            mSize;
        VkBufferUsageFlagBits   mUsage;
        VkMemoryPropertyFlags   mProperties;
        VkBuffer                mBuffer;
        VkDeviceMemory          mBufferMemory;
    };
}
#endif //HOMURA_VULKANBUFFER_H
