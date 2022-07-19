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

    class VulkanSemaphoreEntity
    {
    public:
        explicit VulkanSemaphoreEntity(VulkanDevicePtr device);
        ~VulkanSemaphoreEntity();
        void create();
        void destroy();

        VkSemaphore getHandle()
        {
            return mSemaphore;
        }
    private:
        VulkanDevicePtr mDevice;
        VkSemaphore     mSemaphore;
    };

    class VulkanFences
    {
    public:
        explicit VulkanFences(VulkanDevicePtr device);
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

    class VulkanSemaphores
    {
    public:
        explicit VulkanSemaphores(VulkanDevicePtr device);
        ~VulkanSemaphores();
        
        void create(uint32_t num);
        void destroy();
        VkSemaphore getSemaphore(uint32_t index);
    private:
        VulkanDevicePtr                     mDevice;
        std::vector<VulkanSemaphoreEntity>  mSemaphores;
    };
}
#endif //HOMURA_VULKANSYNCHRONIZATION_H
