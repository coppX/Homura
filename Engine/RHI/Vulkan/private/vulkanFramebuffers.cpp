//
// Created by 最上川 on 2022/1/13.
//
#include <vulkanFramebuffers.h>
#include <vulkanRenderPass.h>
#include <vulkanTexture.h>
#include <vulkanDevice.h>
#include <debugUtils.h>
#include <cassert>
#include <array>

namespace Homura
{
    VulkanFramebuffers::VulkanFramebuffers(VulkanDevicePtr device)
        : mDevice{device}
    {

    }

    VulkanFramebuffers::~VulkanFramebuffers()
    {
        destroy();
    }

    void VulkanFramebuffers::create(VulkanRenderPassPtr renderPass, uint32_t imageCount,
                                    std::vector<VkImageView> &images,
                                    std::vector<VulkanTexture2DPtr> &multiSampleImages,
                                    std::vector<VulkanTextureDepthPtr> &depthImages)
    {
        assert(imageCount <= images.size() && imageCount <= multiSampleImages.size() && imageCount <= depthImages.size());

        mFrameBuffer.resize(imageCount);
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            std::array<VkImageView, 3> attachments = {
                    images[i],
                    multiSampleImages[i]->getImageView(),
                    depthImages[i]->getImageView()
            };

            VkFramebufferCreateInfo frameBufferCreateInfo{};
            frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            frameBufferCreateInfo.renderPass = renderPass->getHandle();
            frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            frameBufferCreateInfo.pAttachments = attachments.data();
            // todo
            frameBufferCreateInfo.width = 960;
            frameBufferCreateInfo.height = 540;
            frameBufferCreateInfo.layers = 1;

            VERIFYVULKANRESULT(vkCreateFramebuffer(mDevice->getHandle(), &frameBufferCreateInfo, nullptr, &mFrameBuffer[i]));
        }
    }

    void VulkanFramebuffers::destroy()
    {
        for (auto frameBuffer : mFrameBuffer)
        {
            vkDestroyFramebuffer(mDevice->getHandle(), frameBuffer, nullptr);
        }
    }

}