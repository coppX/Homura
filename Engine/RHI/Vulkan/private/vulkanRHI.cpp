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

namespace Homura
{
    VulkanRHI::VulkanRHI(GLFWwindow* window)
        : mInstance{nullptr}
        , mDevice{nullptr}
        , mSurface{nullptr}
        , mSwapChain{nullptr}
        , mRenderPass{nullptr}
        , mWindow(window)
        , mWidth(960)
        , mHeight(540)
    {

    }

    VulkanRHI::~VulkanRHI()
    {
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

    void VulkanRHI::createSwapChain(GLFWwindow *window)
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

    void VulkanRHI::createVertexBuffer()
    {

    }

    void VulkanRHI::createIndexBuffer()
    {

    }

    void VulkanRHI::createUniformBuffer()
    {

    }

    void VulkanRHI::createCommandPool()
    {
        mDevice->createCommandPool();
    }

    void VulkanRHI::createCommandBuffer()
    {

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

    void VulkanRHI::destroyDescriptorPool()
    {
        mDescriptorPool->destroy();
    }
}