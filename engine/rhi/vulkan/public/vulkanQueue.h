//
// Created by FDC on 2022/1/7.
//

#ifndef HOMURA_VULKANQUEUE_H
#define HOMURA_VULKANQUEUE_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <memory>

namespace Homura
{
    class VulkanQueue
    {
    public:
        VulkanQueue(VulkanDevicePtr device, uint32_t familyIndex);
        ~VulkanQueue() = default;

        uint32_t getFamilyIndex() const
        {
            return mFamilyIndex;
        }

        VkQueue getHandle() const
        {
            return mQueue;
        }

    private:
        VkQueue                     mQueue;
        uint32_t                    mFamilyIndex;
        VulkanDevicePtr             mDevice;
    };
}
#endif //HOMURA_VULKANQUEUE_H
