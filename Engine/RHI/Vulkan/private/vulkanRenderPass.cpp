//
// Created by FDC on 2022/1/13.
//

#include <vulkanRenderPass.h>
#include <vulkanDevice.h>
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
}