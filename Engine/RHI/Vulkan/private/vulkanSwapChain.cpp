//
// Created by FDC on 2022/1/7.
//

#include <vulkanSwapChain.h>
#include <vulkanDevice.h>
#include <vulkanQueue.h>
#include <debugUtils.h>
#include <vulkanTexture.h>
#include <vulkanRenderPass.h>
#include <algorithm>
#include <array>

namespace Homura
{
    VulkanSwapChain::VulkanSwapChain(VulkanDevicePtr device, GLFWwindow* window, VkSurfaceKHR surface, VkCommandPool commandPool)
        : mDevice{device}
        , mSurface{surface}
    {
        auto swapChainSupportInfo = querySwapChainSupportInfo();
        VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(swapChainSupportInfo.mFormats);
        VkPresentModeKHR presentMode = chooseSurfacePresentMode(swapChainSupportInfo.mPresentModes);
        VkExtent2D extent = chooseExtent(swapChainSupportInfo.mCapabilities);
        mImageCount = swapChainSupportInfo.mCapabilities.minImageCount + 1;

        if (swapChainSupportInfo.mCapabilities.maxImageCount > 0 && mImageCount > swapChainSupportInfo.mCapabilities.maxImageCount)
        {
            mImageCount = swapChainSupportInfo.mCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = mSurface;
        createInfo.minImageCount = mImageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        std::vector<uint32_t> queueFamilies = {mDevice->getGraphicsQueue()->getFamilyIndex(), mDevice->getPresentQueue()->getFamilyIndex()};

        if (mDevice->getGraphicsQueue()->getFamilyIndex() == mDevice->getPresentQueue()->getFamilyIndex())
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilies.size());
            createInfo.pQueueFamilyIndices = queueFamilies.data();
        }

        createInfo.preTransform = swapChainSupportInfo.mCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        VERIFYVULKANRESULT(vkCreateSwapchainKHR(mDevice->getHandle(), &createInfo, nullptr, &mSwapChain));
        mSwapChainFormat = surfaceFormat.format;
        mSwapChainExtent = extent;

        vkGetSwapchainImagesKHR(mDevice->getHandle(), mSwapChain, &mImageCount, nullptr);
        mSwapChainImages.resize(mImageCount);

        vkGetSwapchainImagesKHR(mDevice->getHandle(), mSwapChain, &mImageCount, mSwapChainImages.data());

        mSwapChainImageViews.resize(mImageCount);
        for (int i = 0; i < mImageCount; ++i)
        {
            mSwapChainImageViews[i] = createImageView(mSwapChainImages[i], mSwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
        }

        mDepthImages.resize(mImageCount);

        VkImageSubresourceRange region{};
        region.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        region.baseMipLevel = 0;
        region.levelCount = 1;
        region.baseArrayLayer = 0;
        region.layerCount = 1;

        for (int i = 0; i < mImageCount; ++i)
        {
//            mDepthImages[i] = Image::createDepthImage(
//                    mDevice,
//                    mSwapChainExtent.width,
//                    mSwapChainExtent.height,
//                    mDevice->getMaxUsableSampleCount()
//            );
//
//            mDepthImages[i]->setImageLayout(
//                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
//                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//                    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
//                    region,
//                    commandPool
//            );
        }

        mMultiSampleImages.resize(mImageCount);

        VkImageSubresourceRange regionMultiSample{};
        regionMultiSample.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        regionMultiSample.baseMipLevel = 0;
        regionMultiSample.levelCount = 1;
        regionMultiSample.baseArrayLayer = 0;
        regionMultiSample.layerCount = 1;
        for (int i = 0; i < mImageCount; ++i) {
//            mMutiSampleImages[i] = Image::createRenderTargetImage(
//                    mDevice,
//                    mSwapChainExtent.width,
//                    mSwapChainExtent.height,
//                    mSwapChainFormat
//            );
//
//            mMutiSampleImages[i]->setImageLayout(
//                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
//                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
//                    regionMutiSample,
//                    commandPool
//            );
        }
    }

    void VulkanSwapChain::createFrameBuffers(const VulkanRenderPassPtr renderPass)
    {
        mSwapChainFrameBuffers.resize(mImageCount);
        for (int i = 0; i < mImageCount; ++i)
        {
            std::array<VkImageView, 3> attachments = {
                    mSwapChainImageViews[i],
                    mMultiSampleImages[i]->getImageView(),
                    mDepthImages[i]->getImageView()
            };

            VkFramebufferCreateInfo framebufferCreateInfo{};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = renderPass->getHandle();
            framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferCreateInfo.pAttachments = attachments.data();
            framebufferCreateInfo.width = mSwapChainExtent.width;
            framebufferCreateInfo.height = mSwapChainExtent.height;
            framebufferCreateInfo.layers = 1;

            VERIFYVULKANRESULT(vkCreateFramebuffer(mDevice->getHandle(), &framebufferCreateInfo, nullptr, &mSwapChainFrameBuffers[i]));
        }
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        for (auto& imageView : mSwapChainImageViews)
        {
            vkDestroyImageView(mDevice->getHandle(), imageView, nullptr);
        }

        for (auto& framebuffer : mSwapChainFrameBuffers)
        {
            vkDestroyFramebuffer(mDevice->getHandle(), framebuffer, nullptr);
        }

        if (mSwapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(mDevice->getHandle(), mSwapChain, nullptr);
        }
    }























}