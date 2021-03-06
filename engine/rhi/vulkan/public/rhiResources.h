//
// Created by coppx on 2022/6/16/016.
//

#ifndef HOMURA_RHIRESOURCES_H
#define HOMURA_RHIRESOURCES_H

#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura {
    struct RHIRenderPassInfo {
        std::vector<VulkanSubPassPtr> mSubPasses;
        std::vector<VkSubpassDependency> mDependencies;
        std::vector<VkAttachmentDescription> mAttachmentDescriptions;// color, depth

        RHIRenderPassInfo()
            : mSubPasses{}, mDependencies{}, mAttachmentDescriptions{} 
        {

        }

        void addSubPass(const VulkanSubPassPtr subPass) 
        {
            mSubPasses.push_back(subPass);
        }

        void addDependency(const VkSubpassDependency& dependency) 
        {
            mDependencies.push_back(dependency);
        }

        void addAttachment(const VkAttachmentDescription& attachment) 
        {
            mAttachmentDescriptions.push_back(attachment);
        }
    };

    struct ColorAttachmentReference {
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

    struct ColorAttachmentResolveRef
    {
        ColorAttachmentResolveRef(uint32_t attachmentIndex)
            : mColorResolveRef{}
        {
            mColorResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            mColorResolveRef.attachment = attachmentIndex;
        }

        VkAttachmentReference mColorResolveRef;
    };

    struct AttachmentReference {
        AttachmentReference(std::vector<VkAttachmentDescription> descriptions)
                : ColorReference{}, DepthReference{}, InputReference{}, ResolveReference{} 
        {
            for (int i = 0; i < descriptions.size(); i++) {
                VkAttachmentDescription description = descriptions[i];
                VkAttachmentReference reference{};
                if (description.finalLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ||
                    description.finalLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) // color description
                {
                    reference.attachment = i;
                    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    ColorReference.push_back(reference);
                } 
                else if (description.finalLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) // depth description
                {
                    reference.attachment = i;
                    reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    DepthReference = reference;
                }
                else if (description.finalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) // color resolved
                {
                    ResolveReference.attachment = i;
                    ResolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                }
            }
        }

        std::vector<VkAttachmentReference> ColorReference;
        std::vector<VkAttachmentReference> InputReference;
        VkAttachmentReference ResolveReference;
        VkAttachmentReference DepthReference;
    };

    enum class ShaderType
    {
        VERTEX = 0,
        TESSELLATION_CONTROL,
        TESSELLATION_EVALUATION,
        GEOMETRY,
        FRAGMENT,
        COMPUTE,
        ALL,
    };
    enum class BufferType
    {
        VERTEX = 0,
        INDEX,
        UNIFORM,
        STAGGING,
        ALL,
    };
}

#endif //HOMURA_RHIRESOURCES_H
