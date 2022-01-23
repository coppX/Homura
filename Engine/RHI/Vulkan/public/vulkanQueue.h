//
// Created by FDC on 2022/1/7.
//

#ifndef HOMURA_VULKANQUEUE_H
#define HOMURA_VULKANQUEUE_H
#include <vulkan/vulkan.h>
#include <memory>

namespace Homura
{
    class VulkanDevice;

    class VulkanQueue
    {
    public:
        VulkanQueue(std::shared_ptr<VkDevice> device, uint32_t familyIndex);

        virtual ~VulkanQueue()
        {

        }

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
        std::shared_ptr<VkDevice>   mDevice;
    };
}
#endif //HOMURA_VULKANQUEUE_H
