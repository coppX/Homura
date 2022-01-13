//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANDEVICE_H
#define HOMURA_VULKANDEVICE_H

#include <vulkan/vulkan.h>
#include <pixelFormat.h>
#include <memory>
#include <vector>
#include <string>

namespace Homura
{
    class VulkanQueue;

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


        std::shared_ptr<VulkanQueue> getGraphicsQueue()
        {
            return mGfxQueue;
        }

        std::shared_ptr<VulkanQueue> getComputeQueue()
        {
            return mComputeQueue;
        }

        std::shared_ptr<VulkanQueue> getTransferQueue()
        {
            return mTransferQueue;
        }

        std::shared_ptr<VulkanQueue> getPresentQueue()
        {
            return mPresentQueue;
        }

        std::shared_ptr<VkCommandPool> getCommandPool()
        {
            return mCommandPool;
        }

        void createCommandPool(uint32_t familyIndex);

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

        std::shared_ptr<VulkanQueue>            mGfxQueue;
        std::shared_ptr<VulkanQueue>            mComputeQueue;
        std::shared_ptr<VulkanQueue>            mTransferQueue;
        std::shared_ptr<VulkanQueue>            mPresentQueue;

        std::shared_ptr<VkCommandPool>          mCommandPool;
    };
}
#endif //HOMURA_VULKANDEVICE_H