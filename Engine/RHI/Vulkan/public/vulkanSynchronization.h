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
        VulkanFenceEntity(VulkanDevicePtr device);
        ~VulkanFenceEntity();

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
        VkFence getFence(uint32_t index);
    private:
        VulkanDevicePtr                 mDevice;
        std::vector<VulkanFenceEntity>  mFences;
    };

    // semaphore : signal an image is ready for rendering // ready for presentation
    class VulkanSemaphores
    {
    public:
        VulkanSemaphores(VulkanDevicePtr device);
        ~VulkanSemaphores();

        void create(uint32_t size);
        void destroy();
    private:
        VulkanDevicePtr                 mDevice;
        std::vector<VkSemaphore>        mSemaphores;
    };
}
#endif //HOMURA_VULKANSYNCHRONIZATION_H
