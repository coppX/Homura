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
#include <vulkanCommandBuffer.h>
#include <vulkanFramebuffer.h>
#include <vulkanGfxPipeline.h>
#include <vulkanSynchronization.h>
#include <vulkanBuffer.h>
#include <vulkanTexture.h>
#include <vulkanLayout.h>
#include <vulkanShader.h>

namespace Homura
{
    VulkanRHI::VulkanRHI(GLFWwindow* window, uint32_t width, uint32_t height)
        : mInstance{nullptr}
        , mSurface{nullptr}
        , mDevice{nullptr}
        , mSwapChain{nullptr}
        , mFramebuffer{nullptr}
        , mCommandPool{nullptr}
        , mCommandBuffer{nullptr}
        , mDescriptorPool{nullptr}
        , mRenderPass{nullptr}
        , mWindow{window}
        , mWidth{width}
        , mHeight{height}
    {

    }

    VulkanRHI::~VulkanRHI()
    {

    }

    void VulkanRHI::init()
    {
        createInstance();
        createSurface();
        createDevice();
        createSwapChain();
        createFrameBuffer();
        createCommandPool();
        createDescriptorPool();
        createRenderPass();
        createShader();
        createPipeline();
    }

    void VulkanRHI::exit()
    {
        destroyPipeline();
        destroyShader();
        destroyRenderPass();
        destroyDescriptorPool();
        destroyCommandPool();
        destroyFrameBuffer();
        destroySwapChain();
        destroyDevice();
        destroySurface();
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

    VulkanFramebufferPtr VulkanRHI::getFrameBuffer()
    {
        return mFramebuffer;
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
        mDevice->initializeQueue();
        return mDevice;
    }

    VulkanSwapChainPtr VulkanRHI::createSwapChain()
    {
        mSwapChain = std::make_shared<VulkanSwapChain>(mDevice, mWindow, mSurface);
        return mSwapChain;
    }

    VulkanTexture2DPtr VulkanRHI::createColorResources()
    {
        mRenderTarget =  std::make_shared<VulkanTexture2D>(mDevice, mSwapChain->getExtent().width, mSwapChain->getExtent().height, 1, VK_SAMPLE_COUNT_1_BIT, mSwapChain->getFormat(),
                                          VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        return  mRenderTarget;
    }

    VulkanTextureDepthPtr VulkanRHI::createDepthResources()
    {
        mRenderTargetDepth =  std::make_shared<VulkanTextureDepth>(mDevice, mSwapChain->getExtent().width, mSwapChain->getExtent().height, 1, VK_SAMPLE_COUNT_1_BIT, findDepthFormat(mDevice),
                                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        return mRenderTargetDepth;
    }

    void VulkanRHI::destroyColorResources()
    {
        mRenderTarget->destroy();
    }

    void VulkanRHI::destroyDepthResources()
    {
        mRenderTargetDepth->destroy();
    }

    VulkanFramebufferPtr VulkanRHI::createFrameBuffer()
    {
        mFramebuffer = std::make_shared<VulkanFramebuffer>(mDevice, mSwapChain);
        return mFramebuffer;
    }

    VulkanShaderPtr VulkanRHI::createShader()
    {
        mShader = std::make_shared<VulkanShader>(mDevice);
        return mShader;
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
        mCommandBuffer = std::make_shared<VulkanCommandBuffer>(mDevice, mSwapChain, mCommandPool, mFramebuffer);
        return mCommandBuffer;
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

    VulkanDescriptorSetPtr VulkanRHI::createDescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
    {
        auto layout = std::make_shared<VulkanDescriptorSetLayout>(mDevice);
        layout->create(bindings);
        mDescriptorSet = std::make_shared<VulkanDescriptorSet>(mDevice, mDescriptorPool, layout);
        return mDescriptorSet;
    }

    VulkanPipelinePtr VulkanRHI::createPipeline()
    {
        mPipeline = std::make_shared<VulkanPipeline>(mDevice);
        return mPipeline;
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
        mFramebuffer->destroy();
    }

    void VulkanRHI::destroyRenderPass()
    {
        mRenderPass->destroy();
    }

    void VulkanRHI::destroyCommandPool()
    {
        mCommandPool->destroy();
    }

    void VulkanRHI::destroyDescriptorSet()
    {
        mDescriptorSet->destroy();
    }

    void VulkanRHI::destroyDescriptorPool()
    {
        mDescriptorPool->destroy();
    }

    void VulkanRHI::destroyShader()
    {
        mShader->destroy();
    }

    void VulkanRHI::destroyPipeline()
    {
        mPipeline->destroy();
    }

    void VulkanRHI::idle()
    {
        vkDeviceWaitIdle(mDevice->getHandle());
    }

    void VulkanRHI::setupAttachments()
    {

    }

    void VulkanRHI::setupRenderPass(RHIRenderPassInfo& info)
    {
        mRenderPass->create(info);
    }

    void VulkanRHI::setupFramebuffer(std::vector<VulkanTexture2DPtr> &colorImages, std::vector<VulkanTextureDepthPtr> &depthStencilImages)
    {
        mFramebuffer->create(mRenderPass, colorImages, depthStencilImages);
    }

    void VulkanRHI::setupPipeline(VulkanDescriptorSetPtr descriptorSet)
    {
        mPipeline->create(mRenderPass);
        VkViewport viewport{0.0, 0.0, (float)mWidth, (float)mHeight, 0.0, 1.0};
        VkRect2D scissor{{0, 0}, {mWidth, mHeight}};
        mPipeline->setViewports({viewport});
        mPipeline->setScissors({scissor});
        mPipeline->build(descriptorSet->getLayout());
    }

    void VulkanRHI::setupShaders(std::string filename, ShaderType type)
    {
        mShader->setupShader(filename, type);
    }
}