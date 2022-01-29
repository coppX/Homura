//
// Created by FDC on 2022/1/6.
//

#include <vulkanRHI.h>
#include <debugUtils.h>
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

    void VulkanRHI::createInstance()
    {
        mInstance = std::make_shared<VulkanInstance>();
    }

    void VulkanRHI::createSurface()
    {
        mSurface = std::make_shared<VulkanSurface>(mInstance, mWindow);
    }

    void VulkanRHI::createDevice()
    {
        mDevice = std::make_shared<VulkanDevice>(mInstance, mSurface);
    }

    void VulkanRHI::createSwapChain()
    {
        mSwapChain = std::make_shared<VulkanSwapChain>(mDevice, mWindow, mSurface);
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

    void VulkanRHI::createColorResources()
    {

    }

    void VulkanRHI::createDepthResources()
    {

    }

    void VulkanRHI::createFrameBuffer()
    {
        mSwapChain->createFrameBuffers(mRenderPass);
    }

    void VulkanRHI::createFence()
    {
        mFence = std::make_shared<VulkanFence>(mDevice, false);
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

    void VulkanRHI::createCommandPool()
    {
        mCommandPool = std::make_shared<VulkanCommandPool>(mDevice);
    }

    void VulkanRHI::createCommandBuffer()
    {
        mCommandBuffer = std::make_shared<VulkanCommandBuffer>(mDevice, mCommandPool);
    }

    void VulkanRHI::createSampler()
    {

    }

    void VulkanRHI::createRenderPass()
    {
        mRenderPass = std::make_shared<VulkanRenderPass>(mDevice);
    }

    void VulkanRHI::createDescriptorPool()
    {
        // todo
        uint32_t uniformCount = 1;
        uint32_t imageCount = 1;
        uint32_t frameCount = 1;
        mDescriptorPool = std::make_shared<VulkanDescriptorPool>(mDevice, uniformCount, imageCount, frameCount);
    }

    void VulkanRHI::createPipeline()
    {
        mPipeline = std::make_shared<VulkanPipeline>(mDevice, mRenderPass);
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
}