//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANDEVICE_H
#define HOMURA_VULKANDEVICE_H

#include <vulkan/vulkan.h>

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

        VkDevice getHandle()
        {
            return mDevice;
        }

        const VkPhysicalDeviceProperties& getDeviceProperties() const
        {
            return mPhysicalDeviceProperties;
        }

    private:
        VkDevice mDevice;
        VkPhysicalDevice mPhysicalDevice;

        std::vector<VkQueueFamilyProperties> mQueueFamilyProperties;

        VkPhysicalDeviceProperties  mPhysicalDeviceProperties;
        VkPhysicalDeviceFeatures mPhysicalDeviceFeatures;

        std::shared_ptr<VulkanQueue> mGfxQueue;
        std::shared_ptr<VulkanQueue> mComputeQueue;
        std::shared_ptr<VulkanQueue> mTransferQueue;
        std::shared_ptr<VulkanQueue> mPresent;
    };
}
#endif //HOMURA_VULKANDEVICE_H
