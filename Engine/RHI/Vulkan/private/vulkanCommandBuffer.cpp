//
// Created by 最上川 on 2022/1/9.
//

#include <vulkanCommandBuffer.h>
#include <vulkanGfxPipeline.h>
#include <vulkanDevice.h>
#include <vulkanQueue.h>
#include <vulkanBuffer.h>
#include <vulkanTexture.h>
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

    VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevicePtr device, VulkanSwapChainPtr swapChain, VulkanCommandPoolPtr commandPool, VulkanFramebufferPtr framebuffer, VulkanPipelinePtr pipeline)
        : mDevice{device}
        , mSwapChain{swapChain}
        , mCommandPool{commandPool}
        , mFramebuffrer{framebuffer}
        , mPipeline{pipeline}
        , mCurrentFrameIndex{0}
        , mImageIndex{0}
        , imageInFlight{}
        , inFlightFences{}
        , mImageAvailableSemaphores{}
        , mRenderFinishedSemaphores{}
        , mMaxFrameCount{3}
        , mCommandBuffers{}
        , mHasIndexBuffer{false}
        , mBuferSize{0}
    {
        imageInFlight = std::make_shared<VulkanFences>(mDevice);
        imageInFlight->create(swapChain->getImageCount());
        inFlightFences = std::make_shared<VulkanFences>(mDevice);
        inFlightFences->create(mMaxFrameCount);
        mImageAvailableSemaphores = std::make_shared<VulkanSemaphores>(mDevice);
        mImageAvailableSemaphores->create(mMaxFrameCount);
        mRenderFinishedSemaphores = std::make_shared<VulkanSemaphores>(mDevice);
        mRenderFinishedSemaphores->create(mMaxFrameCount);
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
        if (mImageAvailableSemaphores != nullptr)
        {
            mImageAvailableSemaphores->destroy();
            mImageAvailableSemaphores.reset();
        }
        if (mRenderFinishedSemaphores != nullptr)
        {
            mRenderFinishedSemaphores->destroy();
            mRenderFinishedSemaphores.reset();
        }
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

        submitSync(mDevice->getGraphicsQueue(), commandBuffer, false);
        vkFreeCommandBuffers(mDevice->getHandle(), mCommandPool->getHandle(), 1, &commandBuffer);
    }

    void VulkanCommandBuffer::begin()
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        for (const auto& commandBuffer : mCommandBuffers)
        {            
            VERIFYVULKANRESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));
        }
    }

    void VulkanCommandBuffer::beginRenderPass(VulkanRenderPassPtr renderPass)
    {
        VkRenderPassBeginInfo Info{};
        Info.sType                      = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
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

        for (const auto& commandBuffer : mCommandBuffers)
        {
            vkCmdBeginRenderPass(commandBuffer, &Info, VK_SUBPASS_CONTENTS_INLINE);
        }
    }

    void VulkanCommandBuffer::bindGraphicPipeline()
    {
        for (const auto& commandBuffer : mCommandBuffers)
        {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline->getHandle());
        }
    }

    void VulkanCommandBuffer::bindVertexBuffer(VulkanVertexBufferPtr buffer)
    {
        VkBuffer vertexBuffers[] = {buffer->getHandle()};
        VkDeviceSize offsets[] = {0};
        mBuferSize = buffer->getSize();
        for (const auto& commandBuffer : mCommandBuffers)
        {
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        }
    }

    void VulkanCommandBuffer::bindIndexBuffer(VulkanIndexBufferPtr buffer)
    {
        mBuferSize = buffer->getSize();
        for (const auto& commandBuffer : mCommandBuffers)
        {
            vkCmdBindIndexBuffer(commandBuffer, buffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void VulkanCommandBuffer::bindDescriptorSet()
    {
        const VulkanPipelineLayoutPtr layout = mPipeline->getPipelineLayout();
        const VulkanDescriptorSetPtr descriptorSet = mPipeline->getDescriptorSet();
        assert(mCommandBuffers.size() == descriptorSet->getCount());

        std::vector<VkDescriptorSet>& desSet = descriptorSet->getData();
        for (int i = 0; i < mCommandBuffers.size(); i++)
        {
            vkCmdBindDescriptorSets(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, layout->getHandle(), 0, 1, &desSet[i], 0, nullptr);
        }
    }

    void VulkanCommandBuffer::draw(uint32_t vertexCount)
    {
        for (const auto& commandBuffer : mCommandBuffers)
        {
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
    }

    void VulkanCommandBuffer::drawIndex(uint32_t indexCount)
    {
        for (const auto& commandBuffer : mCommandBuffers)
        {
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        }
    }

    void VulkanCommandBuffer::drawIndirect(VulkanVertexBufferPtr buffer)
    {
        for (const auto& commandBuffer : mCommandBuffers)
        {
            vkCmdDrawIndirect(commandBuffer, buffer->getHandle(), 0, 1, sizeof(VkDrawIndirectCommand));
        }
    }

    void VulkanCommandBuffer::draw()
    {
        if (mHasIndexBuffer != 0)
        {
            drawIndex(mBuferSize);
        }
        else
        {
            draw(mBuferSize);
        }
    }

    void VulkanCommandBuffer::drawIndexIndirect(VulkanStagingBufferPtr buffer)
    {
        for (const auto& commandBuffer : mCommandBuffers)
        {
            vkCmdDrawIndexedIndirect(commandBuffer, buffer->getHandle(), 0, 1, sizeof(VkDrawIndexedIndirectCommand));
        }
    }

    void VulkanCommandBuffer::endRenderPass()
    {
        for (const auto& commandBuffer : mCommandBuffers)
        {
            vkCmdEndRenderPass(commandBuffer);
        }
    }

    void VulkanCommandBuffer::end()
    {
        for (const auto& commandBuffer : mCommandBuffers)
        {
            VERIFYVULKANRESULT(vkEndCommandBuffer(commandBuffer));
        }
    }

    void VulkanCommandBuffer::submitSync(VulkanQueuePtr queue, VkCommandBuffer commandBuffer, bool isSync)
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &commandBuffer;

        VERIFYVULKANRESULT(vkQueueSubmit(queue->getHandle(), 1, &submitInfo, isSync ? inFlightFences->getFence(mCurrentFrameIndex) : VK_NULL_HANDLE));
        VERIFYVULKANRESULT(vkQueueWaitIdle(queue->getHandle()));
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

    void VulkanCommandBuffer::copyBuffer(VulkanBuffer srcBuffer, VulkanBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer.getHandle(), dstBuffer.getHandle(), 1, &copyRegion);

       endSingleTimeCommands(commandBuffer);
    }

    void VulkanCommandBuffer::copyBufferToTexture(VulkanBuffer buffer, VulkanTexture2DPtr texture, uint32_t width, uint32_t height)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0, };
        region.imageExtent = { width, height,1 };

        vkCmdCopyBufferToImage(commandBuffer, buffer.getHandle(), texture->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        endSingleTimeCommands(commandBuffer);
    }

    void VulkanCommandBuffer::beginFrame()
    {
        inFlightFences->wait(mImageIndex);

        VkResult result = vkAcquireNextImageKHR(mDevice->getHandle(), mSwapChain->getHandle(), UINT64_MAX, mImageAvailableSemaphores->getSemaphore(mCurrentFrameIndex), VK_NULL_HANDLE, &mImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // todo
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
        VkSemaphore waitSemaphores[] = {mImageAvailableSemaphores->getSemaphore(mCurrentFrameIndex)};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo{};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = waitSemaphores;
        submitInfo.pWaitDstStageMask    = waitStages;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &mCommandBuffers[mImageIndex];

        VkSemaphore signalSemaphores[]  = {mRenderFinishedSemaphores->getSemaphore(mCurrentFrameIndex)};
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
            // todo
            mSwapChain->recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
        mCurrentFrameIndex = (mCurrentFrameIndex + 1) % mMaxFrameCount;
    }
}