//
// Created by 最上川 on 2022/1/9.
//

#ifndef HOMURA_VULKANCOMMANDBUFFER_H
#define HOMURA_VULKANCOMMANDBUFFER_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>

namespace Homura
{
    class VulkanDevice;

    class VulkanCommandBuffer
    {
    public:
        VulkanCommandBuffer(VulkanDevicePtr device)
            : mCommandBuffer{VK_NULL_HANDLE}
            , mDevice{device}
        {

        }

        ~VulkanCommandBuffer()
        {

        }

        VkCommandBuffer& getHandle()
        {
            return mCommandBuffer;
        }

        void beginSingleTimeCommands();

        void endSingleTimeCommands();
    private:
        VulkanDevicePtr mDevice;
        VkCommandBuffer mCommandBuffer;
    };
}
#endif //HOMURA_VULKANCOMMANDBUFFER_H
