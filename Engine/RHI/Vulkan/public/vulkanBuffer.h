//
// Created by 最上川 on 2022/1/9.
//

#ifndef HOMURA_VULKANBUFFER_H
#define HOMURA_VULKANBUFFER_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>

namespace Homura
{
    class VulkanDevice;
    class VulkanCommandBuffer;

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

        void copyToBuffer(VulkanBuffer &dstBuffer, VkDeviceSize size);

        void copyToImage(VkImage image, uint32_t width, uint32_t height);

        ~VulkanBuffer();

        VkBuffer getHandle()
        {
            return mBuffer;
        }

    private:
        uint32_t  findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        VulkanDevicePtr         mDevice;
        VulkanCommandBufferPtr  mCommandBuffer;

        BufferType              mBufferType;
        VkDeviceSize            mSize;
        VkBufferUsageFlagBits   mUsage;
        VkMemoryPropertyFlags   mProperties;
        VkBuffer                mBuffer;
        VkDeviceMemory          mBufferMemory;
    };
}
#endif //HOMURA_VULKANBUFFER_H
