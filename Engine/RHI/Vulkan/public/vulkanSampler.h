//
// Created by FDC on 2022/1/11.
//

#ifndef HOMURA_VULKANSAMPLER_H
#define HOMURA_VULKANSAMPLER_H
#include <vulkan/vulkan.h>
#include <memory>

namespace Homura
{
    class VulkanDevice;

    class VulkanSampler
    {
        VulkanSampler(std::shared_ptr<VulkanDevice> device);
        ~VulkanSampler();

        VkSampler& getHandle()
        {
            return mSampler;
        }
    private:
        std::shared_ptr<VulkanDevice>   mDevice;
        VkSampler                       mSampler;
    };
}
#endif //HOMURA_VULKANSAMPLER_H
