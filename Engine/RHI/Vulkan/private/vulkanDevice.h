//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANDEVICE_H
#define HOMURA_VULKANDEVICE_H

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace Homura
{
    class VulkanDevice
    {
    public:
        VulkanDevice(VkPhysicalDevice physicalDevice);

        ~VulkanDevice();

        bool queryDevice(uint32_t index);

        void initGPU(uint32_t index);

        bool createDevice();

        void destroyDevice();

        const VkPhysicalDeviceProperties& getDeviceProperties() const
        {
            return mPhysicalDeviceProperties;
        }

    private:
        VkDevice mDevice;
        VkPhysicalDevice mPhysicalDevice;

        std::vector<VkQueueFamilyProperties> mQueueFamilyProperties;

        VkPhysicalDeviceProperties  mPhysicalDeviceProperties;

        std::shared_ptr<VkQueue> mGfxQueue;
        std::shared_ptr<VkQueue> mPresent;
    };
}
#endif //HOMURA_VULKANDEVICE_H
