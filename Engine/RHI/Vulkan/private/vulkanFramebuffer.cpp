//
// Created by 最上川 on 2022/1/13.
//
#include <vulkanFramebuffer.h>
#include <vulkanRenderPass.h>
#include <vulkanSwapChain.h>
#include <vulkanTexture.h>
#include <vulkanDevice.h>
#include <debugUtils.h>
#include <array>

namespace Homura
{
    VulkanFramebuffer::VulkanFramebuffer(VulkanDevicePtr device, VulkanSwapChainPtr swapChain)
        : mDevice{device}
        , mSwapchain{swapChain}
        , mExtent(swapChain->getExtent())
    {

    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {

    }

    void VulkanFramebuffer::create(VulkanRenderPassPtr renderPass, std::vector<VulkanTexture2DPtr>& colorImages,
                                                                    std::vector<VulkanTextureDepthPtr>& depthStencilImages)
    {
        for (int i = 0; i < mSwapchain->getImageCount(); i++)
        {
            std::vector<VkImageView> attachments;
            for (auto img : colorImages)
            {
                attachments.push_back(img->getImageView());
            }

            for (auto img : depthStencilImages)
            {
                attachments.push_back(img->getImageView());
            }

            attachments.push_back(mSwapchain->getImageView(i));

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
    }

    void VulkanFramebuffer::destroy()
    {
        if (mFrameBuffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(mDevice->getHandle(), mFrameBuffer, nullptr);
            mFrameBuffer = VK_NULL_HANDLE;
        }
    }

}