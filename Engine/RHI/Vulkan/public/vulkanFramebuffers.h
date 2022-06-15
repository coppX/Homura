//
// Created by 最上川 on 2022/1/13.
//

#ifndef HOMURA_VULKANFRAMEBUFFERS_H
#define HOMURA_VULKANFRAMEBUFFERS_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura
{
    class VulkanFramebuffers
    {
    public:
        VulkanFramebuffers(VulkanDevicePtr device, VulkanSwapChainPtr swapChain, VulkanRenderPassPtr renderPass);
        ~VulkanFramebuffers();

        void create(VulkanRenderPassPtr renderPass);
        void destroy();

        uint32_t getCount() const
        {
            return mImageCount;
        }

        VkFramebuffer& getHandle()
        {
            return mFrameBuffer;
        }

        VkExtent2D& getExtent()
        {
            return mExtent;
        }

    private:
        VulkanDevicePtr                     mDevice;
        VkFramebuffer                       mFrameBuffer;

        VkImageView                         mImages;
        VulkanTexture2DPtr                  mMultiSampleImages;
        VulkanTextureDepthPtr               mDepthImages;

        uint32_t                            mImageCount;
        VkExtent2D                          mExtent;
    };
}
#endif //HOMURA_VULKANFRAMEBUFFERS_H
