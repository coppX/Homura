//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANDEVICE_H
#define HOMURA_VULKANDEVICE_H

#include <vulkan/vulkan.h>
#include <pixelFormat.h>
#include <vulkanTypes.h>
#include <vector>
#include <string>

namespace Homura
{
    class VulkanDevice
    {
    public:
        VulkanDevice(VkPhysicalDevice physicalDevice);

        ~VulkanDevice();

        bool queryDevice(uint32_t index);

        void initGPU(uint32_t index);

        void createDevice();

        void destroyDevice();

        void setPresentQueue(VkSurfaceKHR surface);

        VkDevice getHandle()
        {
            return mDevice;
        }

        VkPhysicalDevice getPhysicalHandle()
        {
            return mPhysicalDevice;
        }

        const VkPhysicalDeviceProperties& getDeviceProperties() const
        {
            return mPhysicalDeviceProperties;
        }


        VulkanQueuePtr getGraphicsQueue()
        {
            return mGfxQueue;
        }

        VulkanQueuePtr getComputeQueue()
        {
            return mComputeQueue;
        }

        VulkanQueuePtr getTransferQueue()
        {
            return mTransferQueue;
        }

        VulkanQueuePtr getPresentQueue()
        {
            return mPresentQueue;
        }

        std::shared_ptr<VkCommandPool> getCommandPool()
        {
            return mCommandPool;
        }

        void createCommandPool();

        void setPhysicalDeviceFeatures(VkPhysicalDeviceFeatures2* deviceFeatures)
        {
            mPhysicalDeviceFeatures2 = deviceFeatures;
        }
    private:
        VkDevice                                mDevice;
        VkPhysicalDevice                        mPhysicalDevice;

        std::vector<VkQueueFamilyProperties>    mQueueFamilyProperties;

        VkPhysicalDeviceProperties              mPhysicalDeviceProperties;
        VkPhysicalDeviceFeatures                mPhysicalDeviceFeatures;
        VkPhysicalDeviceFeatures2*              mPhysicalDeviceFeatures2;

        VulkanQueuePtr                          mGfxQueue;
        VulkanQueuePtr                          mComputeQueue;
        VulkanQueuePtr                          mTransferQueue;
        VulkanQueuePtr                          mPresentQueue;

        std::shared_ptr<VkCommandPool>          mCommandPool;
    };
}
#endif //HOMURA_VULKANDEVICE_H
