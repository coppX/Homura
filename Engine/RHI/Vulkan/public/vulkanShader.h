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
        VulkanShader(VulkanDevicePtr device, VkShaderStageFlagBits stage, std::string entryPoint);
        ~VulkanShader();

        void createShaderModule(std::string filename);
        const VkShaderModule& getHandle() const
        {
            return mModule;
        }

        const VkShaderStageFlagBits& getStage() const
        {
            return mStage;
        }

        const char* getEntryPointName() const
        {
            return mEntryPoint.c_str();
        }
    private:
        std::vector<char> readFile(const std::string &filename);
        void destroyShaderModule();

    private:
        VulkanDevicePtr             mDevice;

        VkShaderModule              mModule;
        VkShaderStageFlagBits       mStage;
        std::string                 mEntryPoint;
    };
}

#endif //HOMURA_VULKANSHADER_H
