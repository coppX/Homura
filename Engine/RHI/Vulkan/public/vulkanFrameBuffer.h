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
    class VulkanFrameBuffer
    {
    public:
        VulkanFrameBuffer(VulkanDevicePtr device,
                          VulkanRenderPassPtr renderPass,
                          uint32_t imageCount,
                          std::vector<VulkanTexture2DPtr> &images,
                          std::vector<VulkanTexture2DPtr> &multiSampleImages,
                          std::vector<VulkanTextureDepthPtr> &depthImages);
        ~VulkanFrameBuffer();

        uint32_t getCount() const
        {
            return (uint32_t)mFrameBuffer.size();
        }

        VkFramebuffer* getData()
        {
            return mFrameBuffer.data();
        }
    private:
        VulkanDevicePtr                     mDevice;
        VulkanRenderPassPtr                 mRenderPass;
        std::vector<VkFramebuffer>          mFrameBuffer;

        std::vector<VulkanTexture2DPtr>     mImages;
        std::vector<VulkanTexture2DPtr>     mMultiSampleImages;
        std::vector<VulkanTextureDepthPtr>  mDepthImages;

        uint32_t                            mImageCount;
    };
}
#endif //HOMURA_VULKANFRAMEBUFFER_H
