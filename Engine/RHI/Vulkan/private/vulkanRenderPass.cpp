//
// Created by FDC on 2022/1/13.
//

#include <vulkanRenderPass.h>
#include <vulkanDevice.h>
#include <rhiResources.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanSubPass::VulkanSubPass(AttachmentReference references)
        : mSubPassDescription{}
    {
        assert(!references.ColorReference.empty());
        mSubPassDescription.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        mSubPassDescription.colorAttachmentCount    = static_cast<uint32_t>(references.ColorReference.size());
        mSubPassDescription.pColorAttachments       = references.ColorReference.data();
        mSubPassDescription.inputAttachmentCount    = static_cast<uint32_t>(references.InputReference.size());
        mSubPassDescription.pInputAttachments       = references.InputReference.data();
        mSubPassDescription.pResolveAttachments     = &references.ResolveReference;
        mSubPassDescription.pDepthStencilAttachment = references.DepthReference.layout == VK_IMAGE_LAYOUT_UNDEFINED ? nullptr : &references.DepthReference;
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

    void VulkanRenderPass::create(RHIRenderPassInfo& info)
    {
        std::vector<VkSubpassDescription> subPasses{};
        for (int i = 0; i < info.mSubPasses.size(); ++i)
        {
            subPasses.push_back(info.mSubPasses[i]->getHandle());
        }

        VkRenderPassCreateInfo createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount  = static_cast<uint32_t>(info.mAttachmentDescriptions.size());
        createInfo.pAttachments     = info.mAttachmentDescriptions.data();
        createInfo.dependencyCount  = static_cast<uint32_t>(info.mDependencies.size());
        createInfo.pDependencies    = info.mDependencies.data();
        createInfo.subpassCount     = static_cast<uint32_t>(subPasses.size());
        createInfo.pSubpasses       = subPasses.data();

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