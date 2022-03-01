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
        VulkanFramebuffers(VulkanDevicePtr device);
        ~VulkanFramebuffers();

        void create(VulkanRenderPassPtr renderPass, uint32_t imageCount,
                    std::vector<VkImageView> &images,
                    std::vector<VulkanTexture2DPtr> &multiSampleImages,
                    std::vector<VulkanTextureDepthPtr> &depthImages);
        void destroy();

        uint32_t getCount() const
        {
            return (uint32_t)mFrameBuffer.size();
        }

        VkFramebuffer* getData()
        {
            return mFrameBuffer.data();
        }

        VkFramebuffer& getFramebuffer(const int index)
        {
            assert(index < getCount());
            return mFrameBuffer[index];
        }
    private:
        VulkanDevicePtr                     mDevice;
        std::vector<VkFramebuffer>          mFrameBuffer;

        uint32_t                            mImageCount;
    };
}
#endif //HOMURA_VULKANFRAMEBUFFERS_H
