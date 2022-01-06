//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANRHI_H
#define HOMURA_VULKANRHI_H

#include <vector>
#include <vulkan/vulkan.h>

namespace Homura
{
    class VulkanRHI
    {
    public:
        VulkanRHI();

        virtual ~VulkanRHI();

        void init();

    private:
        VkInstance mInstance;
        std::vector<const char*> mValidationLayers;
        std::vector<const char*> mInstanceExtensions;
        VkSurfaceFormatKHR mSurface;

        VkDevice mDevice;
        VkSwapchainKHR mSwapChain;
    };
}
#endif //HOMURA_VULKANRHI_H
