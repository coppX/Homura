//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANRHI_H
#define HOMURA_VULKANRHI_H

#include <vector>

#include <vulkan/vulkan.h>

class VulkanRHI
{
public:
    VulkanRHI();
    virtual ~VulkanRHI();

    void init();
private:
    VkInstance mInstance;
    std::vector<const char*> mInstanceLayers;
    std::vector<const char*> mInstanceExtensions;
};
#endif //HOMURA_VULKANRHI_H
