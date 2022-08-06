//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANRHI_H
#define HOMURA_VULKANRHI_H

#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <rhiResources.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

namespace Homura
{
    class VulkanRHI : public std::enable_shared_from_this<VulkanRHI>
    {
    public:
        VulkanRHI();
        virtual ~VulkanRHI();

        void init(int width, int height, std::string title);
        void exit();
        void update();
        VkSampleCountFlagBits getSampleCount();
        VulkanSamplerPtr getSampler();
        VulkanTexture2DPtr createColorResources();
        VulkanTextureDepthPtr createDepthResources();

        void setupRenderPass(RHIRenderPassInfo& info);
        void setupFramebuffer(std::vector<VulkanTexture2DPtr>& colorImages, std::vector<VulkanTextureDepthPtr>& depthStencilImages);
        VulkanShaderEntityPtr setupShaders(std::string filename, ShaderType type);
        void setupPipeline();

        void beginCommandBuffer();
        void createVertexBuffer(void* bufferData, uint32_t bufferSize, uint32_t count);
        void createIndexBuffer(void* bufferData, uint32_t bufferSize, uint32_t count);
        void createUniformBuffer(int binding, uint32_t bufferSize);
        void updateUniformBuffer(uint32_t index);
        void createSampleTexture(int binding, void* imageData, uint32_t imageSize, uint32_t width, uint32_t height);

        void draw();
        void endCommandBuffer();

        // callback
        void setMouseButtonCallBack(MouseCallback cb);
        void setFramebufferResizeCallback(FramebufferResizeCallback cb);
        void setWriteDataCallback(UnifromUpdateCallback cb);

        void createDescriptorSet();
        VulkanCommandBufferPtr createCommandBuffer();
        void updateDescriptorSet();
    private:
        
        ApplicationWindowPtr createWindow(int width, int height, std::string title);
        VulkanInstancePtr createInstance();
        VulkanDevicePtr createDevice();
        VulkanSurfacePtr createSurface();
        VulkanSwapChainPtr createSwapChain();
        VulkanRenderPassPtr createRenderPass();
        VulkanDescriptorPoolPtr createDescriptorPool();

        VulkanCommandPoolPtr createCommandPool();
        VulkanFramebufferPtr createFrameBuffer();
        VulkanShaderPtr createShader();
        VulkanPipelinePtr createPipeline();
        VulkanSamplerPtr createSampler();

        void destroyBuffer();
        void destroySampleTexture();
        void destroyColorResources();
        void destroyDepthResources();
        void destroyWindow();
        void destroyInstance();
        void destroyDevice();
        void destroySurface();
        void destroySwapChain();
        void destroyRenderPass();
        void destroyDescriptorSet();
        void destroyDescriptorPool();
        void destroyFrameBuffer();
        void destroyCommandBuffer();
        void destroyCommandPool();
        void destroyShader();
        void destroyPipeline();
        void destroyBuffers();
        void destroySampler();

        void recreateSwapChain();
        void cleanupSwapchain();
        void cleanup();
        
        void idle();
        //        void addPushConstant(const VkPushConstantRange& constantRange, const char* data);

    private:
        VulkanInstancePtr                   mInstance;;
        VulkanDevicePtr                     mDevice;
        VulkanSurfacePtr                    mSurface;
        VulkanSwapChainPtr                  mSwapChain;
        VulkanRenderPassPtr                 mRenderPass;
        VulkanDescriptorPoolPtr             mDescriptorPool;
        VulkanDescriptorSetPtr              mDescriptorSet;
        VulkanCommandPoolPtr                mCommandPool;
        VulkanCommandBufferPtr              mCommandBuffer;
        VulkanFramebufferPtr                mFramebuffer;
        VulkanPipelinePtr                   mPipeline;
        VulkanTexture2DPtr                  mRenderTarget;
        VulkanTextureDepthPtr               mRenderTargetDepth;
        VulkanShaderPtr                     mShader;
        VulkanSamplerPtr                    mSampler;

        VulkanTexture2DPtr                  mDepthStencil;
        std::vector<VulkanBufferPtr>        mBuffers;
        std::vector<VulkanUniformBufferPtr> mUniformBuffers;
        std::vector<VulkanTexture2DPtr>     mSampleTextures;

        // window
        ApplicationWindowPtr                mWindow;
    public:
        MouseCallback                       mMouseCallback;
        FramebufferResizeCallback           mFramebufferResizeCallback;
    };
}
#endif //HOMURA_VULKANRHI_H
