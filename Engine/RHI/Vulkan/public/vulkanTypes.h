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
    class VulkanBuffer;
    class VulkanQueue;

    using VulkanDevicePtr = std::shared_ptr<VulkanDevice>;
    using VulkanRenderPassPtr = std::shared_ptr<VulkanRenderPass>;
    using VulkanBufferPtr = std::shared_ptr<VulkanBuffer>;
    using VulkanQueuePtr = std::shared_ptr<VulkanQueue>;
}
#endif //HOMURA_VULKANTYPES_H
