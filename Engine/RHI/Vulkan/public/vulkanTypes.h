//
// Created by FDC on 2022/1/14.
//

#ifndef HOMURA_VULKANTYPES_H
#define HOMURA_VULKANTYPES_H
#include <memory>

namespace Homura
{
    class VulkanDevice;
    class VulkanRenderPass;

    using VulkanDevicePtr = std::shared_ptr<VulkanDevice>;
    using VulkanRenderPassPtr = std::shared_ptr<VulkanRenderPass>;
}
#endif //HOMURA_VULKANTYPES_H
