//
// Created by FDC on 2022/1/6.
//

#include <vulkanRHI.h>
#include <GLFW/glfw3.h>
#include <vulkanDevice.h>
#include <vulkanSwapChain.h>
#include <vulkanInstance.h>
#include <vulkanSurface.h>
#include <vulkanRenderPass.h>
#include <vulkanDescriptorSet.h>
#include <vulkanCommandPool.h>
#include <vulkanCommandBuffer.h>
#include <vulkanGfxPipeline.h>
#include <vulkanFence.h>
#include <vulkanBuffer.h>
#include <vulkanTexture.h>

namespace Homura
{
    VulkanRHI::VulkanRHI(GLFWwindow* window)
        : mInstance{nullptr}
        , mDevice{nullptr}
        , mSurface{nullptr}
        , mSwapChain{nullptr}
        , mRenderPass{nullptr}
        , mDescriptorPool{nullptr}
        , mCommandBuffer{nullptr}
        , mFence{nullptr}
        , mWindow(window)
        , mWidth(960)
        , mHeight(540)
    {

    }

    VulkanRHI::~VulkanRHI()
    {
        destroyFence();
        destroyPipeline();
        destroyCommandBuffer();
        destroyDescriptorPool();
        destroySwapChain();
        destroySurface();
        destroyDevice();
        destroyInstance();
    }

    void VulkanRHI::init()
    {
        createInstance();
        createDevice();
        createSurface();
        createSwapChain();
        createRenderPass();
        createDescriptorPool();
        createCommandPool();
        createFrameBuffer();
        createCommandBuffer();
        createFence();
        createPipeline();
    }

    void VulkanRHI::exit()
    {
        destroyPipeline();
        destroyFence();
        destroyCommandBuffer();
        destroyFrameBuffer();
        destroyCommandPool();
        destroyDescriptorPool();
        destroyRenderPass();
        destroySwapChain();
        destroySurface();
        destroyDevice();
        destroyInstance();
    }

    VulkanInstancePtr VulkanRHI::getInstance()
    {
        return mInstance;
    }

    VulkanDevicePtr VulkanRHI::getDevice()
    {
        return mDevice;
    }

    VulkanSwapChainPtr VulkanRHI::getSwapChain()
    {
        return mSwapChain;
    }

    VulkanInstancePtr VulkanRHI::createInstance()
    {
        mInstance = std::make_shared<VulkanInstance>();
        return mInstance;
    }

    VulkanSurfacePtr VulkanRHI::createSurface()
    {
        mSurface = std::make_shared<VulkanSurface>(mInstance, mWindow);
        return mSurface;
    }

    VulkanDevicePtr VulkanRHI::createDevice()
    {
        mDevice = std::make_shared<VulkanDevice>(mInstance, mSurface);
        return mDevice;
    }

    VulkanSwapChainPtr VulkanRHI::createSwapChain()
    {
        mSwapChain = std::make_shared<VulkanSwapChain>(mDevice, mWindow, mSurface);
        return mSwapChain;
    }

    void VulkanRHI::destroyInstance()
    {
        mInstance->destroy();
    }

    void VulkanRHI::destroyDevice()
    {
        mDevice->destroy();
    }

    void VulkanRHI::destroySurface()
    {
        mSurface->destroy();
    }

    void VulkanRHI::destroySwapChain()
    {
        mSwapChain->destroy();
    }

    void VulkanRHI::destroyCommandBuffer()
    {
        mCommandBuffer->destroy();
    }

    void VulkanRHI::destroyFrameBuffer()
    {
        mSwapChain->destroyFrameBuffer();
    }

    VulkanTexture2DPtr VulkanRHI::createColorResources()
    {
        return std::make_shared<VulkanTexture2D>(mDevice, mWidth, mHeight, 1, mDevice->getSampleCount(), mSwapChain->getFormat(),
                                          VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    VulkanTextureDepthPtr VulkanRHI::createDepthResources()
    {
        return std::make_shared<VulkanTextureDepth>(mDevice, mWidth, mHeight, 1, mDevice->getSampleCount(), findDepthFormat(),
                                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    void VulkanRHI::createFrameBuffer()
    {
        mSwapChain->createFrameBuffers(mRenderPass);
    }

    VulkanFencePtr VulkanRHI::createFence()
    {
        mFence = std::make_shared<VulkanFence>(mDevice, false);
        return mFence;
    }

    VulkanVertexBufferPtr VulkanRHI::createVertexBuffer(uint32_t size, void* pData)
    {
        return std::make_shared<VulkanVertexBuffer>(mDevice, static_cast<VkDeviceSize>(size), pData);
    }

    VulkanIndexBufferPtr VulkanRHI::createIndexBuffer(uint32_t size, void* pData)
    {
        return std::make_shared<VulkanIndexBuffer>(mDevice, static_cast<VkDeviceSize>(size), pData);
    }

    VulkanUniformBufferPtr VulkanRHI::createUniformBuffer(uint32_t size, void* pData)
    {
        return std::make_shared<VulkanUniformBuffer>(mDevice, static_cast<VkDeviceSize>(size), pData);
    }

    VulkanStagingBufferPtr VulkanRHI::createStagingBuffer(uint32_t size, void* pData)
    {
        return std::make_shared<VulkanStagingBuffer>(mDevice, static_cast<VkDeviceSize>(size), pData);
    }

    VulkanCommandPoolPtr VulkanRHI::createCommandPool()
    {
        mCommandPool = std::make_shared<VulkanCommandPool>(mDevice);
        return mCommandPool;
    }

    VulkanCommandBufferPtr VulkanRHI::createCommandBuffer()
    {
        mCommandBuffer = std::make_shared<VulkanCommandBuffer>(mDevice, mCommandPool);
        return mCommandBuffer;
    }

    void VulkanRHI::createSampler()
    {

    }

    VulkanRenderPassPtr VulkanRHI::createRenderPass()
    {
        mRenderPass = std::make_shared<VulkanRenderPass>(mDevice);
        return mRenderPass;
    }

    VulkanDescriptorPoolPtr VulkanRHI::createDescriptorPool()
    {
        // todo
        uint32_t uniformCount = 1;
        uint32_t imageCount = 1;
        uint32_t frameCount = 1;
        mDescriptorPool = std::make_shared<VulkanDescriptorPool>(mDevice, uniformCount, imageCount, frameCount);
        return mDescriptorPool;
    }

    VulkanPipelinePtr VulkanRHI::createPipeline()
    {
        mPipeline = std::make_shared<VulkanPipeline>(mDevice, mRenderPass);
        return mPipeline;
    }

    void VulkanRHI::destroyRenderPass()
    {
        mRenderPass->destroy();
    }

    void VulkanRHI::destroyCommandPool()
    {
        mCommandPool->destroy();
    }

    void VulkanRHI::destroyDescriptorPool()
    {
        mDescriptorPool->destroy();
    }

    void VulkanRHI::destroyFence()
    {
        mFence->destroy();
    }

    void VulkanRHI::destroyPipeline()
    {
        mPipeline->destroy();
    }
    VkFormat VulkanRHI::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(mDevice->getPhysicalHandle(), format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL & (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }
    VkFormat VulkanRHI::findDepthFormat()
    {
        return findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                   VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
}