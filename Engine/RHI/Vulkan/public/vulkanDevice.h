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
            VK_KHR_MAINTENANCE1_EXTENSION_NAME
    };

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
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

        VulkanQueuePtr getPresentQueue()
        {
            return mPresentQueue;
        }

        std::shared_ptr<VkCommandPool> getCommandPool()
        {
            return mCommandPool;
        }

        const VkSampleCountFlagBits& getSampleCount() const
        {
            return mMsaaSamples;
        }

        void createCommandPool();

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
        VulkanQueuePtr                  mPresentQueue;

        VkSampleCountFlagBits           mMsaaSamples;
        std::shared_ptr<VkCommandPool>  mCommandPool;
    };
}
#endif //HOMURA_VULKANDEVICE_H
