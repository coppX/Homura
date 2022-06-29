//
// Created by coppx on 2022/6/16/016.
//

#ifndef HOMURA_RHIRESOURCES_H
#define HOMURA_RHIRESOURCES_H

#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura {
    struct RHIRenderPassInfo
    {
        std::vector<VulkanSubPassPtr> mSubPasses;
        std::vector<VkSubpassDependency> mDependencies;
        std::vector<VkAttachmentDescription> mAttachmentDescriptions;// color, depth

        RHIRenderPassInfo()
            : mSubPasses{}, mDependencies{}, mAttachmentDescriptions{} {}

        void addSubPass(const VulkanSubPassPtr subPass)
        {
            mSubPasses.push_back(subPass);
        }

        void addDependency(const VkSubpassDependency &dependency)
        {
            mDependencies.push_back(dependency);
        }

        void addAttachment(const VkAttachmentDescription &attachment)
        {
            mAttachmentDescriptions.push_back(attachment);
        }
    };

    struct ColorAttachmentReference
    {
        ColorAttachmentReference(int32_t attachmentIndex = -1)
            : mColorReference{}
        {
            mColorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            mColorReference.attachment = attachmentIndex == -1 ? 0 : attachmentIndex;
        }

        VkAttachmentReference& getHandle()
        {
            return mColorReference;
        }

        VkAttachmentReference mColorReference;
    };

    struct DepthAttachmentReference
    {
        DepthAttachmentReference(uint32_t attachmentIndex)
            : mDepthReference{}
        {
            mDepthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            mDepthReference.attachment = attachmentIndex;
        }

        VkAttachmentReference& getHandle()
        {
            return mDepthReference;
        }

        VkAttachmentReference mDepthReference;
    };

    struct AttachmentReference
    {
        AttachmentReference(std::vector<VkAttachmentDescription> descriptions)
            : ColorReference{}
            , DepthReference{}
            , InputReference{}
            , ResolveReference{}
        {
            bool hasMultiSample = false;
            for (int i = 0; i < descriptions.size(); i++)
            {
                VkAttachmentDescription description = descriptions[i];
                VkAttachmentReference reference{};
                if (description.finalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR ||
                    description.finalLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) // color description
                {
                    reference.attachment = i;
                    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    if (description.samples > VK_SAMPLE_COUNT_1_BIT)
                    {
                        hasMultiSample = true;
                    }
                    ColorReference.push_back(reference);
                }
                else if (description.finalLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) // depth description
                {
                    reference.attachment = i;
                    reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    DepthReference = reference;
                }
            }
            if (hasMultiSample)
            {
                ResolveReference.attachment = descriptions.size();
                ResolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }
        }

        std::vector<VkAttachmentReference>  ColorReference;
        std::vector<VkAttachmentReference>  InputReference;
        VkAttachmentReference               ResolveReference;
        VkAttachmentReference               DepthReference;
    };
}

#endif //HOMURA_RHIRESOURCES_H
