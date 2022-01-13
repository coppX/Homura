//
// Created by 最上川 on 2022/1/13.
//

#ifndef HOMURA_VULKANFRAMEBUFFER_H
#define HOMURA_VULKANFRAMEBUFFER_H
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace Homura
{
    class VulkanDevice;
    class VulkanTexture;
    class VulkanRenderPass;

    class VulkanFrameBuffer
    {
    public:
        VulkanFrameBuffer(std::shared_ptr<VulkanDevice> device,
                          std::shared_ptr<VulkanRenderPass> renderPass,
                          uint32_t imageCount,
                          std::vector<VulkanTexture> &images,
                          std::vector<VulkanTexture> &multiSampleImages,
                          std::vector<VulkanTexture> &depthImages);
        ~VulkanFrameBuffer();

    private:
        std::shared_ptr<VulkanDevice>       mDevice;
        std::shared_ptr<VulkanRenderPass>   mRenderPass;
        std::vector<VkFramebuffer>          mFrameBuffer;

        std::vector<VulkanTexture>          mImages;
        std::vector<VulkanTexture>          mMultiSampleImages;
        std::vector<VulkanTexture>          mDepthImages;

        uint32_t                            mImageCount;
    };
}
#endif //HOMURA_VULKANFRAMEBUFFER_H
