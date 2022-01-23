//
// Created by 最上川 on 2022/1/13.
//

#ifndef HOMURA_VULKANFENCE_H
#define HOMURA_VULKANFENCE_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>

namespace Homura
{
    class VulkanFence
    {
    public:
        VulkanFence(VulkanDevicePtr device, bool signaled);
        ~VulkanFence();

        VkFence& getHandle()
        {
            return mFence;
        }

        void reset();

        VkResult getResult();

    private:
        VulkanDevicePtr mDevice;
        VkFence         mFence;
    };
}
#endif //HOMURA_VULKANFENCE_H
