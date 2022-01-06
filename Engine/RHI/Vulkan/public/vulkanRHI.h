//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANRHI_H
#define HOMURA_VULKANRHI_H

#include <vector>
#include <vulkan/vulkan.h>

namespace Homura
{
    class VulkanDevice;

    class VulkanRHI
    {
    public:
        VulkanRHI();

        virtual ~VulkanRHI();

        void init();

        const VkInstance& getInstance() const
        {
            return mInstance;
        }

        const VkDevice& getDevice() const
        {
            return mDevice;
        }

        const VkSwapchainKHR& getSwapChain() const
        {
            return mSwapChain;
        }

        void createInstance();
        void SelectAndInitDevice();
        void createSwapChain();

        void destroyInstance();
        void destroyDevice();
        void destroySwapChain();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    private:
        bool checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();

    private:
        VkInstance mInstance;
        std::vector<const char*> mValidationLayers;
        std::vector<const char*> mDeviceExtensions;

        std::shared_ptr<VulkanDevice> mDevice;
        VkSwapchainKHR mSwapChain;
    };
}
#endif //HOMURA_VULKANRHI_H
