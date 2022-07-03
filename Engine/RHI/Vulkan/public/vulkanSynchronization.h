//
// Created by 最上川 on 2022/1/13.
//

#ifndef HOMURA_VULKANSYNCHRONIZATION_H
#define HOMURA_VULKANSYNCHRONIZATION_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura
{
    class VulkanFenceEntity
    {
    public:
        explicit VulkanFenceEntity(VulkanDevicePtr device);
        ~VulkanFenceEntity();

        VulkanFenceEntity& operator=(VulkanFenceEntity& r);

        void create(bool signaled);
        void destroy();

        void reset();
        void wait();
        VkResult getResult();

        VkFence& getHandle()
        {
            return mFence;
        }
    private:
        VulkanDevicePtr mDevice;
        VkFence         mFence;
    };

    class VulkanFences
    {
    public:
        VulkanFences(VulkanDevicePtr device);
        ~VulkanFences();

        void create(uint32_t num);
        void destroy();

        void wait(uint32_t index);
        void waitAll();
        void setValue(VulkanFenceEntity& value, uint32_t index);
        VkFence getFence(uint32_t index);
        VulkanFenceEntity& getEntity(uint32_t index);
    private:
        VulkanDevicePtr                 mDevice;
        std::vector<VulkanFenceEntity>  mFences;
    };
}
#endif //HOMURA_VULKANSYNCHRONIZATION_H
