//
// Created by 最上川 on 2022/1/24.
//

#ifndef HOMURA_VULKANCOMMANDPOOL_H
#define HOMURA_VULKANCOMMANDPOOL_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
namespace Homura
{
    class VulkanCommandPool
    {
    public:
        VulkanCommandPool(VulkanDevicePtr device);
        ~VulkanCommandPool();

        void create();
        void reset();
        void destroy();

        VkCommandPool& getHandle()
        {
            return mCommandPool;
        }
    private:
        VulkanDevicePtr             mDevice;
        VkCommandPool               mCommandPool;
    };
}
#endif //HOMURA_VULKANCOMMANDPOOL_H
