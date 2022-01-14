//
// Created by 最上川 on 2022/1/13.
//

#ifndef HOMURA_VULKANFRAMEBUFFER_H
#define HOMURA_VULKANFRAMEBUFFER_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura
{
    class VulkanDevice;
    class VulkanTexture;
    class VulkanRenderPass;

    class VulkanFrameBuffer
    {
    public:
        VulkanFrameBuffer(VulkanDevicePtr device,
                          VulkanRenderPassPtr renderPass,
                          uint32_t imageCount,
                          std::vector<VulkanTexture> &images,
                          std::vector<VulkanTexture> &multiSampleImages,
                          std::vector<VulkanTexture> &depthImages);
        ~VulkanFrameBuffer();

        uint32_t getCount() const
        {
            return mFrameBuffer.size();
        }

        VkFramebuffer* getData()
        {
            return mFrameBuffer.data();
        }
    private:
        VulkanDevicePtr                     mDevice;
        VulkanRenderPassPtr                 mRenderPass;
        std::vector<VkFramebuffer>          mFrameBuffer;

        std::vector<VulkanTexture>          mImages;
        std::vector<VulkanTexture>          mMultiSampleImages;
        std::vector<VulkanTexture>          mDepthImages;

        uint32_t                            mImageCount;
    };
}
#endif //HOMURA_VULKANFRAMEBUFFER_H
