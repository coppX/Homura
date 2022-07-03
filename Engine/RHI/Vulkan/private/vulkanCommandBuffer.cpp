//
// Created by 最上川 on 2022/1/9.
//

#include <vulkanCommandBuffer.h>
#include <vulkanGfxPipeline.h>
#include <vulkanDevice.h>
#include <vulkanQueue.h>
#include <vulkanBuffer.h>
#include <vulkanLayout.h>
#include <vulkanDescriptorSet.h>
#include <vulkanRenderPass.h>
#include <vulkanFramebuffer.h>
#include <vulkanSwapChain.h>
#include <vulkanSynchronization.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanCommandPool::VulkanCommandPool(VulkanDevicePtr device)
        : mDevice{device}
        , mCommandPool{VK_NULL_HANDLE}
    {
        create();
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        destroy();
    }

    void VulkanCommandPool::create()
    {
        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.queueFamilyIndex = mDevice->getGraphicsQueue()->getFamilyIndex();
        VERIFYVULKANRESULT(vkCreateCommandPool(mDevice->getHandle(), &createInfo, nullptr, &mCommandPool));
    }

    void VulkanCommandPool::reset()
    {
        VERIFYVULKANRESULT(vkResetCommandPool(mDevice->getHandle(), mCommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
    }

    void VulkanCommandPool::destroy()
    {
        if (mCommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(mDevice->getHandle(), mCommandPool, nullptr);
            mCommandPool = VK_NULL_HANDLE;
        }
    }

    VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevicePtr device, VulkanSwapChainPtr swapChain, VulkanCommandPoolPtr commandPool, VulkanFramebufferPtr framebuffer)
        : mDevice{device}
        , mSwapChain{swapChain}
        , mCommandPool{commandPool}
        , mFramebuffrer{framebuffer}
        , mCurrentFrameIndex{0}
        , mImageIndex{0}
        , imageInFlight{}
        , inFlightFences{}
        , mImageAvailableSemaphores{}
        , mRenderFinishedSemaphores{}
        , mMaxFrameCount{swapChain->getImageCount()}
        , mCommandBuffers{}
    {
        imageInFlight = std::make_shared<VulkanFences>(mDevice);
        imageInFlight->create(swapChain->getImageCount());
        inFlightFences = std::make_shared<VulkanFences>(mDevice);
        inFlightFences->create(swapChain->getImageCount());
        // todo semaphores size
        mImageAvailableSemaphores.resize(swapChain->getImageCount());
        mRenderFinishedSemaphores.resize(swapChain->getImageCount());
        create();
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {

    }

    void VulkanCommandBuffer::create()
    {
        mCommandBuffers.resize(mSwapChain->getImageCount());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandBufferCount    = (uint32_t)mCommandBuffers.size();
        allocInfo.commandPool           = mCommandPool->getHandle();
        allocInfo.level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VERIFYVULKANRESULT(vkAllocateCommandBuffers(mDevice->getHandle(), &allocInfo, mCommandBuffers.data()));
    }

    void VulkanCommandBuffer::destroy()
    {
        for (auto& commandBuffer : mCommandBuffers)
        {
            vkFreeCommandBuffers(mDevice->getHandle(), mCommandPool->getHandle(), 1, &commandBuffer);
        }
        mCommandBuffers.clear();

        if (imageInFlight != nullptr)
        {
            imageInFlight->destroy();
            imageInFlight.reset();
        }
        if (inFlightFences != nullptr)
        {
            inFlightFences->destroy();
            inFlightFences.reset();
        }
        for (auto& semaphore : mImageAvailableSemaphores)
        {
            vkDestroySemaphore(mDevice->getHandle(), semaphore, nullptr);
        }
        mImageAvailableSemaphores.clear();
        for (auto& semaphore : mRenderFinishedSemaphores)
        {
            vkDestroySemaphore(mDevice->getHandle(), semaphore, nullptr);
        }
        mRenderFinishedSemaphores.clear();
    }

    VkCommandBuffer VulkanCommandBuffer::beginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool           = mCommandPool->getHandle();
        allocInfo.commandBufferCount    = 1;

        VkCommandBuffer commandBuffer;
        VERIFYVULKANRESULT(vkAllocateCommandBuffers(mDevice->getHandle(), &allocInfo, &commandBuffer));

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VERIFYVULKANRESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));
        return commandBuffer;
    }

    void VulkanCommandBuffer::endSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        submitSync(mDevice->getGraphicsQueue(), false);
        vkFreeCommandBuffers(mDevice->getHandle(), mCommandPool->getHandle(), 1, &commandBuffer);
    }

    void VulkanCommandBuffer::begin(uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType             = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VERIFYVULKANRESULT(vkBeginCommandBuffer(mCommandBuffers[index], &beginInfo));
    }

    void VulkanCommandBuffer::beginRenderPass(VulkanRenderPassPtr renderPass, uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        VkRenderPassBeginInfo Info{};
        Info.renderPass                 = renderPass->getHandle();
        Info.framebuffer                = mFramebuffrer->getHandle();
        Info.renderArea.offset.x        = 0;
        Info.renderArea.offset.y        = 0;
        Info.renderArea.extent.width    = mFramebuffrer->getExtent().width;
        Info.renderArea.extent.height   = mFramebuffrer->getExtent().height;

        std::vector<VkClearValue> clearValues{2};
        clearValues[0].color            = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil     = {1.0f, 0};

        Info.clearValueCount            = static_cast<uint32_t>(clearValues.size());
        Info.pClearValues               = clearValues.data();

        vkCmdBeginRenderPass(mCommandBuffers[index], &Info, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandBuffer::bindGraphicPipeline(VulkanPipelinePtr pipeline, uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        vkCmdBindPipeline(mCommandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getHandle());
    }

    void VulkanCommandBuffer::bindVertexBuffer(std::vector<VulkanVertexBufferPtr>& buffers, uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        std::vector<VkDeviceSize> offsets(buffers.size(), 0);
        std::vector<VkBuffer> nativeBuffers;
        for (const auto& buffer : buffers)
        {
            nativeBuffers.push_back(buffer->getHandle());
        }

        vkCmdBindVertexBuffers(mCommandBuffers[index], 0, static_cast<uint32_t>(nativeBuffers.size()), nativeBuffers.data(), offsets.data());
    }

    void VulkanCommandBuffer::bindIndexBuffer(VulkanIndexBufferPtr buffer, uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        vkCmdBindIndexBuffer(mCommandBuffers[index], buffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanCommandBuffer::bindDescriptorSet(const VulkanPipelineLayoutPtr layout, const VulkanDescriptorSetPtr descriptorSet, uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        vkCmdBindDescriptorSets(mCommandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, layout->getHandle(), 0, descriptorSet->getCount(), descriptorSet->getData(), 0, nullptr);
    }

    void VulkanCommandBuffer::draw(uint32_t vertexCount, uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        vkCmdDraw(mCommandBuffers[index], vertexCount, 1, 0, 0);
    }

    void VulkanCommandBuffer::drawIndex(uint32_t indexCount, uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        vkCmdDrawIndexed(mCommandBuffers[index], indexCount, 1, 0, 0, 0);
    }

    void VulkanCommandBuffer::drawIndirect(VulkanVertexBufferPtr buffer, uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        vkCmdDrawIndirect(mCommandBuffers[index], buffer->getHandle(), 0, 1, sizeof(VkDrawIndirectCommand));
    }

    void VulkanCommandBuffer::drawIndexIndirect(VulkanStagingBufferPtr buffer, uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        vkCmdDrawIndexedIndirect(mCommandBuffers[index], buffer->getHandle(), 0, 1, sizeof(VkDrawIndexedIndirectCommand));
    }

    void VulkanCommandBuffer::endRenderPass(uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        vkCmdEndRenderPass(mCommandBuffers[index]);
    }

    void VulkanCommandBuffer::end(uint32_t index)
    {
        assert(index < mCommandBuffers.size());
        VERIFYVULKANRESULT(vkEndCommandBuffer(mCommandBuffers[index]));
    }

    void VulkanCommandBuffer::submitSync(VulkanQueuePtr queue, bool isSync)
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &mCommandBuffers[mImageIndex];

        VERIFYVULKANRESULT(vkQueueSubmit(queue->getHandle(), 1, &submitInfo, isSync ? inFlightFences->getFence(mCurrentFrameIndex) : VK_NULL_HANDLE));
//        VERIFYVULKANRESULT(vkQueueWaitIdle(queue->getHandle()));
    }

    void VulkanCommandBuffer::transferImageLayout(VkCommandBuffer commandBuffer, const VkImageMemoryBarrier& imageMemoryBarrier, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask)
    {
        vkCmdPipelineBarrier(
                commandBuffer,
                srcStageMask,
                dstStageMask,
                0,
                0, nullptr,// MemoryBarrier
                0, nullptr,//BufferMemoryBarrier
                1, &imageMemoryBarrier//imageMemoryBarrier
                );
    }

    void VulkanCommandBuffer::blitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout,
                   VkImage dstImage, VkImageLayout dstImageLayout,
                   uint32_t regionCount, VkImageBlit* Regions, VkFilter filter)
    {
        vkCmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, Regions, filter);
    }

    void VulkanCommandBuffer::beginFrame()
    {
        inFlightFences->wait(mImageIndex);

        VkResult result = vkAcquireNextImageKHR(mDevice->getHandle(), mSwapChain->getHandle(), UINT64_MAX, mImageAvailableSemaphores[mCurrentFrameIndex], VK_NULL_HANDLE, &mImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            mSwapChain->recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swapchain image");
        }
    }

    void VulkanCommandBuffer::endFrame()
    {
        if (imageInFlight->getFence(mImageIndex) != VK_NULL_HANDLE)
        {
            imageInFlight->wait(mImageIndex);
        }
        imageInFlight->setValue(inFlightFences->getEntity(mCurrentFrameIndex), mImageIndex);
        VkSemaphore waitSemaphores[] = {mImageAvailableSemaphores[mCurrentFrameIndex]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo{};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = waitSemaphores;
        submitInfo.pWaitDstStageMask    = waitStages;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &mCommandBuffers[mImageIndex];

        VkSemaphore signalSemaphores[]  = {mRenderFinishedSemaphores[mCurrentFrameIndex]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = signalSemaphores;
        inFlightFences->getEntity(mCurrentFrameIndex).reset();
        VERIFYVULKANRESULT(vkQueueSubmit(mDevice->getGraphicsQueue()->getHandle(), 1, &submitInfo, inFlightFences->getFence(mCurrentFrameIndex)));

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType               = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount  = 1;
        presentInfo.pWaitSemaphores     = signalSemaphores;

        VkSwapchainKHR swapchain[]      = {mSwapChain->getHandle()};
        presentInfo.swapchainCount      = 1;
        presentInfo.pSwapchains         = swapchain;
        presentInfo.pImageIndices       = &mImageIndex;

        VkResult result = vkQueuePresentKHR(mDevice->getPresentQueue()->getHandle(), &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            mSwapChain->recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
        mCurrentFrameIndex = (mCurrentFrameIndex + 1) % mMaxFrameCount;
    }
}