//
// Created by FDC on 2022/1/11.
//

#ifndef HOMURA_VULKANSAMPLER_H
#define HOMURA_VULKANSAMPLER_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>

namespace Homura
{
    class VulkanSampler
    {
        VulkanSampler(VulkanDevicePtr device);
        ~VulkanSampler();

        VkSampler& getHandle()
        {
            return mSampler;
        }
    private:
        VulkanDevicePtr                 mDevice;
        VkSampler                       mSampler;
    };
}
#endif //HOMURA_VULKANSAMPLER_H
