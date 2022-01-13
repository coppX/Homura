//
// Created by 最上川 on 2022/1/13.
//

#ifndef HOMURA_VULKANFENCE_H
#define HOMURA_VULKANFENCE_H
#include <vulkan/vulkan.h>
#include <memory>

namespace Homura
{
    class VulkanDevice;

    class Fence
    {
    public:
        Fence(std::shared_ptr<VulkanDevice> device, bool signaled);
        ~Fence();

        VkFence& getHandle()
        {
            return mFence;
        }

        void reset();

        VkResult getResult();

    private:
        std::shared_ptr<VulkanDevice>   mDevice;
        VkFence                         mFence;
    };
}
#endif //HOMURA_VULKANFENCE_H
