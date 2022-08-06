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

    struct AttachmentDescription
    {
        AttachmentDescription(VkFormat format, VkSampleCountFlagBits samples,
            VkAttachmentLoadOp loadOp,
            VkAttachmentStoreOp storeOp,
            VkAttachmentLoadOp stencilLoadOp,
            VkAttachmentStoreOp stencilStoreOp,
            VkImageLayout initialLayout,
            VkImageLayout finalLayout
        )
            : mDescription{}
        {
            mDescription.format = format;
            mDescription.samples = samples;
            mDescription.loadOp = loadOp;
            mDescription.storeOp = storeOp;
            mDescription.stencilLoadOp = stencilLoadOp;
            mDescription.stencilStoreOp = stencilStoreOp;
            mDescription.initialLayout = initialLayout;
            mDescription.finalLayout = finalLayout;
        }

        VkAttachmentDescription getHandle()
        {
            return mDescription;
        }
    private:
        VkAttachmentDescription mDescription;
    };

    struct ColorAttachmentDescription : public AttachmentDescription
    {
        ColorAttachmentDescription(VkFormat format, VkSampleCountFlagBits samples,
            VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        )
            : AttachmentDescription(format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout)
        {
            
        }
    };

    struct DepthAttachmentDescription : public AttachmentDescription
    {
        DepthAttachmentDescription(VkFormat format, VkSampleCountFlagBits samples,
            VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        )
            : AttachmentDescription(format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout)
        {

        }
    };

    struct InputAttachmentDescription : public AttachmentDescription
    {
        InputAttachmentDescription(VkFormat format, VkSampleCountFlagBits samples,
            VkAttachmentLoadOp loadOp,
            VkAttachmentStoreOp storeOp,
            VkAttachmentLoadOp stencilLoadOp,
            VkAttachmentStoreOp stencilStoreOp,
            VkImageLayout initialLayout,
            VkImageLayout finalLayout
        )
            : AttachmentDescription(format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout)
        {

        }
    };

    struct ResolveAttachmentDescription : public AttachmentDescription
    {
        ResolveAttachmentDescription(VkFormat format, VkSampleCountFlagBits samples,
            VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        )
            : AttachmentDescription(format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout)
        {

        }
    };

    struct AttachmentReference {
        AttachmentReference(std::vector<AttachmentDescription> descriptions)
                : ColorReference{}, DepthReference{}, InputReference{}, ResolveReference{} 
        {
            for (int i = 0; i < descriptions.size(); i++) {
                VkAttachmentDescription description = descriptions[i].getHandle();
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

    enum ShaderType
    {
        VERTEX = 0,
        TESSELLATION_CONTROL,
        TESSELLATION_EVALUATION,
        GEOMETRY,
        FRAGMENT,
        COMPUTE,
        ALL,
    };
}

#endif //HOMURA_RHIRESOURCES_H
