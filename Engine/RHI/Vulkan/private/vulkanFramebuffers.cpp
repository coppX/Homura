//
// Created by 最上川 on 2022/1/13.
//
#include <vulkanFramebuffers.h>
#include <vulkanRenderPass.h>
#include <vulkanSwapChain.h>
#include <vulkanTexture.h>
#include <vulkanDevice.h>
#include <debugUtils.h>
#include <array>

namespace Homura
{
    VulkanFramebuffers::VulkanFramebuffers(VulkanDevicePtr device, VulkanSwapChainPtr swapChain, VulkanRenderPassPtr renderPass)
        : mDevice{device}
        , mImageCount{swapChain->getImageCount()}
        , mExtent(swapChain->getExtent())
    {
        create(renderPass);
    }

    VulkanFramebuffers::~VulkanFramebuffers()
    {
        destroy();
    }

    void VulkanFramebuffers::create(VulkanRenderPassPtr renderPass)
    {
        std::array<VkImageView, 3> attachments = {
                mImages,
                mMultiSampleImages->getImageView(),
                mDepthImages->getImageView()
        };

        VkFramebufferCreateInfo frameBufferCreateInfo{};
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.renderPass = renderPass->getHandle();
        frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        frameBufferCreateInfo.pAttachments = attachments.data();
        frameBufferCreateInfo.width = mExtent.width;
        frameBufferCreateInfo.height = mExtent.height;
        frameBufferCreateInfo.layers = 1;

        VERIFYVULKANRESULT(vkCreateFramebuffer(mDevice->getHandle(), &frameBufferCreateInfo, nullptr, &mFrameBuffer));
    }

    void VulkanFramebuffers::destroy()
    {
        if (mFrameBuffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(mDevice->getHandle(), mFrameBuffer, nullptr);
        }
    }

}