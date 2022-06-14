//
// Created by FDC on 2022/1/7.
//

#include <vulkanSwapChain.h>
#include <vulkanDevice.h>
#include <vulkanQueue.h>
#include <debugUtils.h>
#include <vulkanTexture.h>
#include <vulkanRenderPass.h>
#include <vulkanSurface.h>
#include <pixelFormat.h>
#include <algorithm>
#include <array>
#include <vulkanFramebuffers.h>

namespace Homura
{
    VulkanSwapChain::VulkanSwapChain(VulkanDevicePtr device, GLFWwindow* window, VulkanSurfacePtr surface)
        : mDevice{device}
        , mSurface{surface}
        , mWindow(window)
        , mSwapChainFramebuffers{std::make_shared<VulkanFramebuffers>(device)}
    {
        create();
    }

    void VulkanSwapChain::createFrameBuffers(const VulkanRenderPassPtr renderPass)
    {
        mSwapChainFramebuffers->create(renderPass, mImageCount, mSwapChainImageViews, mMultiSampleImages, mDepthImages);
    }

    VulkanSwapChain::~VulkanSwapChain()
    {

    }

    VkFramebuffer& VulkanSwapChain::getFrameBuffer(const int index)
    {
        return mSwapChainFramebuffers->getFramebuffer(index);
    }

    void VulkanSwapChain::create()
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
        createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface          = mSurface->getHandle();
        createInfo.minImageCount    = mImageCount;
        createInfo.imageFormat      = surfaceFormat.format;
        createInfo.imageColorSpace  = surfaceFormat.colorSpace;
        createInfo.imageExtent      = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        std::vector<uint32_t> queueFamilies = {mDevice->getGraphicsQueue()->getFamilyIndex()};

        createInfo.imageSharingMode         = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount    = 0;
        createInfo.pQueueFamilyIndices      = nullptr;

        createInfo.preTransform     = swapChainSupportInfo.mCapabilities.currentTransform;
        createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode      = presentMode;
        createInfo.clipped          = VK_TRUE;

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
        region.aspectMask       = VK_IMAGE_ASPECT_DEPTH_BIT;
        region.baseMipLevel     = 0;
        region.levelCount       = 1;
        region.baseArrayLayer   = 0;
        region.layerCount       = 1;

        for (int i = 0; i < mImageCount; ++i)
        {
            mDepthImages[i] = std::make_shared<VulkanTextureDepth>(mDevice,
                         mSwapChainExtent.width,
                         mSwapChainExtent.height,
                         1,
                         mDevice->getSampleCount(),
                         findDepthFormat(mDevice),
                         VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                         );

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
        for (int i = 0; i < mImageCount; ++i)
        {
            mMultiSampleImages[i] = std::make_shared<VulkanTexture2D>(
                    mDevice,
                    mSwapChainExtent.width,
                    mSwapChainExtent.height,
                    1,
                    mDevice->getSampleCount(),
                    mSwapChainFormat,
                    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );
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

    void VulkanSwapChain::destroy()
    {
        for (auto& multiSampleImage : mMultiSampleImages)
        {
            multiSampleImage->destroy();
        }

        for (auto& depthImage : mDepthImages)
        {
            depthImage->destroy();
        }
        mMultiSampleImages.clear();
        mDepthImages.clear();

        destroyImageView();
        destroyFrameBuffer();
        destroySwapChain();
    }

    void VulkanSwapChain::destroyImageView()
    {
        for (auto &imageView : mSwapChainImageViews)
        {
            vkDestroyImageView(mDevice->getHandle(), imageView, nullptr);
        }
        mSwapChainImageViews.clear();
    }

    void VulkanSwapChain::destroyFrameBuffer()
    {
        mSwapChainFramebuffers->destroy();
    }

    void VulkanSwapChain::destroySwapChain()
    {
        if (mSwapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(mDevice->getHandle(), mSwapChain, nullptr);
            mSwapChain = VK_NULL_HANDLE;
        }
    }
    SwapChainSupportInfo VulkanSwapChain::querySwapChainSupportInfo()
    {
        SwapChainSupportInfo info;
        VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalHandle(), mSurface->getHandle(), &info.mCapabilities));

        uint32_t formatCount = 0;
        VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalHandle(), mSurface->getHandle(), &formatCount, nullptr));
        if (formatCount != 0)
        {
            info.mFormats.resize(formatCount);
            VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalHandle(), mSurface->getHandle(), &formatCount, info.mFormats.data()));
        }

        uint32_t presentModeCount = 0;
        VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalHandle(), mSurface->getHandle(), &presentModeCount, nullptr));
        if (presentModeCount != 0)
        {
            info.mPresentModes.resize(presentModeCount);
            VERIFYVULKANRESULT(
                    vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalHandle(), mSurface->getHandle(), &presentModeCount, info.mPresentModes.data()));
        }

        return info;
    }

    VkSurfaceFormatKHR VulkanSwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
    {
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
        {
            return {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        }

        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapChain::chooseSurfacePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
    {
        VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }
        return bestMode;
    }

    VkExtent2D VulkanSwapChain::chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }

        int width = 0, height = 0;
        glfwGetFramebufferSize(mWindow, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::max(capabilities.minImageExtent.width,
                                      std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                       std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }

    VkImageView VulkanSwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image      = image;
        viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format     = format;

        viewInfo.subresourceRange.aspectMask        = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel      = 0;
        viewInfo.subresourceRange.levelCount        = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer    = 0;
        viewInfo.subresourceRange.layerCount        = 1;

        VkImageView imageView{VK_NULL_HANDLE};
        VERIFYVULKANRESULT(vkCreateImageView(mDevice->getHandle(), &viewInfo, nullptr, &imageView));

        return imageView;
    }
}