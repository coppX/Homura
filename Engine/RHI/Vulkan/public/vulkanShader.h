//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANSHADER_H
#define HOMURA_VULKANSHADER_H
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <memory>

namespace Homura
{
    class VulkanShader
    {
    public:
        VulkanShader(std::shared_ptr<VkDevice> device);
        ~VulkanShader();

        void createShaderModule(std::string filename, VkAllocationCallbacks *allocator = nullptr);
        VkShaderModule getHandle() { return mModule; }

    private:
        std::vector<char> readFile(const std::string &filename);
        void destroyShaderModule();

    private:
        VkShaderModule mModule;
        VkAllocationCallbacks *pAllocator;
        std::shared_ptr<VkDevice> mDevice;
    };
}

#endif //HOMURA_VULKANSHADER_H
