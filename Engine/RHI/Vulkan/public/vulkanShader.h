//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANSHADER_H
#define HOMURA_VULKANSHADER_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>
#include <string>

namespace Homura
{
    class VulkanShader
    {
    public:
        VulkanShader(VulkanDevicePtr device);
        ~VulkanShader();

        void createShaderModule(std::string filename, VkAllocationCallbacks *allocator = nullptr);
        VkShaderModule getHandle() { return mModule; }

    private:
        std::vector<char> readFile(const std::string &filename);
        void destroyShaderModule();

    private:
        VkShaderModule              mModule;
        VkAllocationCallbacks       *pAllocator;
        VulkanDevicePtr             mDevice;
    };
}

#endif //HOMURA_VULKANSHADER_H
