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

        void create(bool signaled);
        void destroy();

        void reset();
        VkResult getResult();

        VkFence& getHandle()
        {
            return mFence;
        }
    private:
        VulkanDevicePtr mDevice;
        VkFence         mFence;
    };
}
#endif //HOMURA_VULKANFENCE_H
