//
// Created by FDC on 2022/1/6.
//

#include <vulkanRHI.h>
#include <GLFW/glfw3.h>
#include <applicationWindow.h>
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
#include <vulkanSampler.h>

namespace Homura
{
    VulkanRHI::VulkanRHI()
        : mInstance{nullptr}
        , mSurface{nullptr}
        , mDevice{nullptr}
        , mSwapChain{nullptr}
        , mFramebuffer{nullptr}
        , mCommandPool{nullptr}
        , mCommandBuffer{nullptr}
        , mDescriptorPool{nullptr}
        , mRenderPass{nullptr}
        , mWindow{nullptr}
        , mMouseCallback{}
        , mFramebufferResizeCallback{}
    {

    }

    VulkanRHI::~VulkanRHI()
    {

    }

    void VulkanRHI::init(int width, int height, std::string title)
    {
        createWindow(width, height, title);
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
        createSampler();
    }

    void VulkanRHI::exit()
    {
        //cleanup();
    }

    void VulkanRHI::update()
    {
        while (!mWindow->shouldClose())
        {
            mWindow->processInput();
            mCommandBuffer->drawFrame(shared_from_this());
        }
        idle();
        cleanup();
    }

    VkSampleCountFlagBits VulkanRHI::getSampleCount()
    {
        return VK_SAMPLE_COUNT_4_BIT;
    }

    VulkanSamplerPtr VulkanRHI::getSampler()
    {
        return mSampler;
    }

    ApplicationWindowPtr VulkanRHI::createWindow(int width, int height, std::string title)
    {
        mWindow = std::make_shared<ApplicationWindow>(shared_from_this(), width, height);
        mWindow->create(title);
        return mWindow;
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

    void VulkanRHI::recreateSwapChain()
    {
        // todo
        mWindow->resize();
        idle();
        cleanupSwapchain();
        createSwapChain();
        createRenderPass();
        createPipeline();
        createColorResources();
        createDepthResources();
        createFrameBuffer();
        createCommandBuffer();
    }

    VulkanTexture2DPtr VulkanRHI::createColorResources()
    {
        mColorAttachment = std::make_shared<VulkanTexture2D>(mDevice, mSwapChain->getExtent().width, mSwapChain->getExtent().height, 1, getSampleCount(), mSwapChain->getFormat(),
                                          VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        return mColorAttachment;
    }

    VulkanTextureDepthPtr VulkanRHI::createDepthResources()
    {
        mDepthAttachment = std::make_shared<VulkanTextureDepth>(mDevice, mSwapChain->getExtent().width, mSwapChain->getExtent().height, 1, getSampleCount(), findDepthFormat(mDevice),
                                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        return mDepthAttachment;
    }

    void VulkanRHI::destroyColorResources()
    {
        mColorAttachment->destroy();
    }

    void VulkanRHI::destroyDepthResources()
    {
        mDepthAttachment->destroy();
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

    VulkanCommandPoolPtr VulkanRHI::createCommandPool()
    {
        mCommandPool = std::make_shared<VulkanCommandPool>(mDevice);
        return mCommandPool;
    }

    VulkanCommandBufferPtr VulkanRHI::createCommandBuffer()
    {
        mCommandBuffer = std::make_shared<VulkanCommandBuffer>(mDevice, mSwapChain, mCommandPool, mFramebuffer, mPipeline);
        return mCommandBuffer;
    }

    VulkanRenderPassPtr VulkanRHI::createRenderPass()
    {
        mRenderPass = std::make_shared<VulkanRenderPass>(mDevice);
        return mRenderPass;
    }

    VulkanDescriptorPoolPtr VulkanRHI::createDescriptorPool()
    {
        mDescriptorPool = std::make_shared<VulkanDescriptorPool>(mDevice, mSwapChain->getImageCount());
        return mDescriptorPool;
    }

    void VulkanRHI::createDescriptorSet()
    {
        VulkanDescriptorSetLayoutPtr layout = std::make_shared<VulkanDescriptorSetLayout>(mDevice);
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        VkDescriptorSetLayoutBinding binding{};
        if (!mUniformBuffers.empty())
        {
            binding.binding         = mUniformBuffers[0]->getBinding();
            binding.descriptorCount = 1;
            binding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            binding.stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;
            bindings.push_back(binding);
        }
        if (!mSampleTextures.empty())
        {
            binding.binding         = mSampleTextures[0]->getBinding();
            binding.descriptorCount = 1;
            binding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            binding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings.push_back(binding);
        }
        layout->create(bindings);
        mDescriptorSet = std::make_shared<VulkanDescriptorSet>(mDevice, mDescriptorPool, layout);
    }

    void VulkanRHI::updateDescriptorSet()
    {
        mDescriptorSet->updateDescriptorSet(mUniformBuffers, mSampleTextures);
    }

    VulkanPipelinePtr VulkanRHI::createPipeline()
    {
        mPipeline = std::make_shared<VulkanPipeline>(mDevice);
        return mPipeline;
    }

    VulkanSamplerPtr VulkanRHI::createSampler()
    {
        mSampler = std::make_shared<VulkanSampler>(mDevice);
        return mSampler;
    }

    void VulkanRHI::destroyWindow()
    {
        mWindow->destroy();
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

    void VulkanRHI::destroyBuffers()
    {
        for (auto& buffer : mBuffers)
        {
            buffer->destroy();
        }
        mBuffers.clear();
        for (auto& uniform : mUniformBuffers)
        {
            uniform->destroy();
        }
        mUniformBuffers.clear();
    }

    void VulkanRHI::destroySampler()
    {
        mSampler->destroy();
    }

    void VulkanRHI::cleanupSwapchain()
    {
        destroyColorResources();
        destroyDepthResources();
        destroyCommandBuffer();
        destroyFrameBuffer();
        destroyPipeline();
        destroyRenderPass();
        destroySwapChain();
        mSwapChain->destroy();
    }

    void VulkanRHI::cleanup()
    {
        destroySampler();
        destroySampleTexture();
        destroyColorResources();
        destroyDepthResources();
        destroyDescriptorSet();
        destroyBuffers();
        destroyCommandBuffer();
        destroyCommandPool();
        destroyFrameBuffer();
        destroySwapChain();
        destroySurface();
        destroyPipeline();
        destroyRenderPass();
        destroyDescriptorPool();
        destroyInstance();
        destroyWindow();
    }

    void VulkanRHI::idle()
    {
        mDevice->idle();
    }

    void VulkanRHI::setupRenderPass(RHIRenderPassInfo& info)
    {
        mRenderPass->create(info);
    }

    void VulkanRHI::setupFramebuffer(std::vector<VulkanTexture2DPtr> &colorImages, std::vector<VulkanTextureDepthPtr> &depthStencilImages)
    {
        mFramebuffer->create(mRenderPass, colorImages, depthStencilImages);
    }

    void VulkanRHI::setupPipeline()
    {
        mPipeline->create(mRenderPass, getSampleCount());
        VkViewport viewport{0.0, 0.0, (float)mWindow->getWidth(), (float)mWindow->getHeight(), 0.0, 1.0};
        VkRect2D scissor{{0, 0}, {mWindow->getWidth(), mWindow->getHeight()}};
        mPipeline->setViewports({viewport});
        mPipeline->setScissors({scissor});
        mPipeline->setShaders(mShader);
        updateDescriptorSet();
        mPipeline->build(mDescriptorSet);
        mShader->destroy();
    }

    VulkanShaderEntityPtr VulkanRHI::setupShaders(std::string filename, ShaderType type)
    {
        return mShader->setupShader(filename, type);
    }

    void VulkanRHI::beginCommandBuffer()
    {
        mCommandBuffer->begin();
        mCommandBuffer->beginRenderPass(mRenderPass);
        mCommandBuffer->bindGraphicPipeline();
        mCommandBuffer->bindDescriptorSet();
    }

    void VulkanRHI::createVertexBuffer(void* bufferData, uint32_t bufferSize, uint32_t count)
    {
        VulkanVertexBufferPtr buffer = std::make_shared<VulkanVertexBuffer>(mDevice, mCommandBuffer, bufferSize, bufferData);
        mCommandBuffer->bindVertexBuffer(buffer, count);
        mBuffers.push_back(buffer);
    }

    void VulkanRHI::createIndexBuffer(void* bufferData, uint32_t bufferSize, uint32_t count)
    {
        VulkanIndexBufferPtr buffer = std::make_shared<VulkanIndexBuffer>(mDevice, mCommandBuffer, bufferSize, bufferData);
        mCommandBuffer->bindIndexBuffer(buffer, count);
        mBuffers.push_back(buffer);
    }

    void VulkanRHI::createUniformBuffer(int binding, uint32_t bufferSize)
    {
        for (int i = 0; i < mSwapChain->getImageCount(); i++)
        {
            VulkanUniformBufferPtr buffer = std::make_shared<VulkanUniformBuffer>(mDevice, mCommandBuffer, bufferSize, binding);
            mUniformBuffers.push_back(buffer);
        }
    }

    void VulkanRHI::setWriteDataCallback(UnifromUpdateCallback callback)
    {
        for (auto& uniform : mUniformBuffers)
        {
            uniform->setUpdateCallBack(callback);
        }
    }

    void VulkanRHI::updateUniformBuffer(uint32_t index)
    {
        assert(index < mUniformBuffers.size());
        mUniformBuffers[index]->update();
    }

    void VulkanRHI::createSampleTexture(int binding, void* imageData, uint32_t imageSize, uint32_t width, uint32_t height)
    {
        VulkanStagingBufferPtr stagingBuffer = std::make_shared<VulkanStagingBuffer>(mDevice, mCommandBuffer, imageSize, imageData);
        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
        VulkanTexture2DPtr sampleTexture = std::make_shared<VulkanTexture2D>(mDevice, width, height, mipLevels, 
                                                                            VK_SAMPLE_COUNT_1_BIT, 
                                                                            VK_FORMAT_R8G8B8A8_SRGB, 
                                                                            VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                                                            VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                            VK_IMAGE_USAGE_SAMPLED_BIT,
                                                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        sampleTexture->setImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mCommandBuffer);
        sampleTexture->fromBuffer(mCommandBuffer, stagingBuffer);
        sampleTexture->generateMipmaps(mCommandBuffer);
        sampleTexture->setSampler(mSampler, binding);
        mSampleTextures.push_back(sampleTexture);
        stagingBuffer->destroy();
    }

    void VulkanRHI::draw()
    {
        mCommandBuffer->draw();
    }

    void VulkanRHI::endCommandBuffer()
    {
        mCommandBuffer->endRenderPass();
        mCommandBuffer->end();
    }

    void VulkanRHI::destroySampleTexture()
    {
        for (auto& texture : mSampleTextures)
        {
            texture->destroy();
        }
    }

    void VulkanRHI::setMouseButtonCallBack(MouseCallback cb)
    {
        mMouseCallback = cb;
    }

    void VulkanRHI::setFramebufferResizeCallback(FramebufferResizeCallback cb)
    {
        mFramebufferResizeCallback = cb;
    }
}