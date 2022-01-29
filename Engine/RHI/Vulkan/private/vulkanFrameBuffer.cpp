//
// Created by 最上川 on 2022/1/13.
//
#include <vulkanFrameBuffer.h>
#include <vulkanRenderPass.h>
#include <vulkanTexture.h>
#include <vulkanDevice.h>
#include <debugUtils.h>
#include <cassert>
#include <array>

namespace Homura
{
    VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevicePtr device,
                                         VulkanRenderPassPtr renderPass,
                                         uint32_t imageCount,
                                         std::vector<VulkanTexture2DPtr> &images,
                                         std::vector<VulkanTexture2DPtr> &multiSampleImages,
                                         std::vector<VulkanTextureDepthPtr> &depthImages)
        : mDevice{device}
        , mRenderPass{renderPass}
        , mImageCount{imageCount}
        , mImages{images}
        , mMultiSampleImages{multiSampleImages}
        , mDepthImages{depthImages}
    {
        assert(imageCount <= images.size() && imageCount <= multiSampleImages.size() && imageCount <= depthImages.size());

        mFrameBuffer.resize(mImageCount);
        for (uint32_t i = 0; i < mImageCount; ++i)
        {
            std::array<VkImageView, 3> attachments = {
                mImages[i]->getImageView(),
                mMultiSampleImages[i]->getImageView(),
                mDepthImages[i]->getImageView()
            };

            VkFramebufferCreateInfo frameBufferCreateInfo{};
            frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            frameBufferCreateInfo.renderPass = mRenderPass->getHandle();
            frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            frameBufferCreateInfo.pAttachments = attachments.data();
            // todo
            frameBufferCreateInfo.width = 960;
            frameBufferCreateInfo.height = 540;
            frameBufferCreateInfo.layers = 1;

            VERIFYVULKANRESULT(vkCreateFramebuffer(mDevice->getHandle(), &frameBufferCreateInfo, nullptr, &mFrameBuffer[i]));
        }
    }

    VulkanFrameBuffer::~VulkanFrameBuffer()
    {
        for (auto frameBuffer : mFrameBuffer)
        {
            vkDestroyFramebuffer(mDevice->getHandle(), frameBuffer, nullptr);
        }
    }
}