//
// Created by 最上川 on 2022/2/1.
//

#ifndef HOMURA_VULKANSAMPLER_H
#define HOMURA_VULKANSAMPLER_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>

namespace Homura
{
    class VulkanSampler
    {
    public:
        VulkanSampler(VulkanDevicePtr device);
        ~VulkanSampler();

        void create();
        void destroy();

        VkSampler& getHandle()
        {
            return mSampler;
        }
    private:
        VulkanDevicePtr     mDevice;
        VkSampler           mSampler;
    };
}
#endif //HOMURA_VULKANSAMPLER_H
