//
// Created by FDC on 2022/1/13.
//

#include <vulkanRenderPass.h>
#include <vulkanDevice.h>
#include <debugUtils.h>
#include <cassert>

namespace Homura
{
    VulkanSubPass::VulkanSubPass(std::vector<VkAttachmentReference> &colorAttachmentRef,
                                 std::vector<VkAttachmentReference> &inputAttachmentRef,
                                 const VkAttachmentReference &depthStencilAttachmentRef,
                                 const VkAttachmentReference &resolveAttachmentRef)
        : mSubPassDescription{}
        , mColorAttachmentReferences{colorAttachmentRef}
        , mInputAttachmentReferences{inputAttachmentRef}
        , mDepthStencilAttachmentReference{depthStencilAttachmentRef}
        , mResolvedAttachmentReference{resolveAttachmentRef}
    {
        assert(!mColorAttachmentReferences.empty());

        mSubPassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        mSubPassDescription.colorAttachmentCount = static_cast<uint32_t>(mColorAttachmentReferences.size());
        mSubPassDescription.pColorAttachments = mColorAttachmentReferences.data();

        mSubPassDescription.inputAttachmentCount = static_cast<uint32_t>(mInputAttachmentReferences.size());
        mSubPassDescription.pInputAttachments = mInputAttachmentReferences.data();

        mSubPassDescription.pResolveAttachments = &mResolvedAttachmentReference;

        mSubPassDescription.pDepthStencilAttachment = mDepthStencilAttachmentReference.layout == VK_IMAGE_LAYOUT_UNDEFINED ? nullptr : &mDepthStencilAttachmentReference;
    }

    VulkanSubPass::~VulkanSubPass()
    {

    }

    VulkanRenderPass::VulkanRenderPass(VulkanDevicePtr device)
        : mDevice{device}
        , mRenderPass{VK_NULL_HANDLE}
    {

    }

    VulkanRenderPass::~VulkanRenderPass()
    {
        if (mRenderPass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(mDevice->getHandle(), mRenderPass, nullptr);
        }
    }

    void VulkanRenderPass::addSubPass(const VulkanSubPass &subPass)
    {
        mSubPasses.push_back(subPass);
    }

    void VulkanRenderPass::addDependency(const VkSubpassDependency &dependency)
    {
        mDependencies.push_back(dependency);
    }

    void VulkanRenderPass::addAttachment(const VkAttachmentDescription &attachment)
    {
        mAttachmentDescriptions.push_back(attachment);
    }

    void VulkanRenderPass::create()
    {
        assert(!mSubPasses.empty() && !mDependencies.empty() && !mAttachmentDescriptions.empty());

        std::vector<VkSubpassDescription> subPasses{};
        for (int i = 0; i < mSubPasses.size(); ++i)
        {
            subPasses.push_back(mSubPasses[i].getSubPassHandle());
        }

        VkRenderPassCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

        createInfo.attachmentCount = static_cast<uint32_t>(mAttachmentDescriptions.size());
        createInfo.pAttachments = mAttachmentDescriptions.data();

        createInfo.dependencyCount = static_cast<uint32_t>(mDependencies.size());
        createInfo.pDependencies = mDependencies.data();

        createInfo.subpassCount = static_cast<uint32_t>(subPasses.size());
        createInfo.pSubpasses = subPasses.data();

        VERIFYVULKANRESULT(vkCreateRenderPass(mDevice->getHandle(), &createInfo, nullptr, &mRenderPass));
    }

    void VulkanRenderPass::destroy()
    {
        if (mRenderPass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(mDevice->getHandle(), mRenderPass, nullptr);
            mRenderPass = VK_NULL_HANDLE;
        }
    }
}