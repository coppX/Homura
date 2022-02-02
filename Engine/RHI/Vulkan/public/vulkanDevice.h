//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANDEVICE_H
#define HOMURA_VULKANDEVICE_H

#include <vulkan/vulkan.h>
#include <pixelFormat.h>
#include <vulkanTypes.h>
#include <optional>
#include <vector>
#include <string>

namespace Homura
{
    const std::vector<const char*> deviceRequiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            "VK_KHR_portability_subset"
    };

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value();
        }
    };

    class VulkanDevice
    {
    public:
        VulkanDevice(VulkanInstancePtr instance, VulkanSurfacePtr surface);

        ~VulkanDevice();

        void create();

        void destroy();

        VkDevice getHandle()
        {
            return mDevice;
        }

        VkPhysicalDevice getPhysicalHandle()
        {
            return mPhysicalDevice;
        }

        VulkanQueuePtr getGraphicsQueue()
        {
            return mGfxQueue;
        }

        const VkSampleCountFlagBits& getSampleCount() const
        {
            return mMsaaSamples;
        }

    private:
        void pickPhysicalDevice();
        void createLogicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device);
        VkSampleCountFlagBits getMaxUsableSampleCount();
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    private:
        VkDevice                        mDevice;
        VkPhysicalDevice                mPhysicalDevice;

        VulkanInstancePtr               mInstance;
        VulkanSurfacePtr                mSurface;

        VulkanQueuePtr                  mGfxQueue;

        VkSampleCountFlagBits           mMsaaSamples;
    };
}
#endif //HOMURA_VULKANDEVICE_H
