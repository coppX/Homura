//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANSHADER_H
#define HOMURA_VULKANSHADER_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>
#include <string>
#include <rhiResources.h>

namespace Homura
{
    class VulkanShaderEntity
    {
    public:
        VulkanShaderEntity(VulkanDevicePtr device, VkShaderStageFlagBits stage, std::string entryPoint);
        ~VulkanShaderEntity();

        void create(std::vector<char> shaderCode);
        void destroy();

        VkShaderStageFlagBits getStage()
        {
            return mStage;
        }

        VkShaderModule getHandle()
        {
            return mModule;
        }

        std::string& getName()
        {
            return mEntryPoint;
        }
    private:
        VulkanDevicePtr             mDevice;
        VkShaderModule              mModule;
        VkShaderStageFlagBits       mStage;
        std::string                 mEntryPoint;
    };

    class VulkanShader
    {
    public:
        explicit VulkanShader(VulkanDevicePtr device);
        ~VulkanShader();

        void setupShader(std::string filename, ShaderType type);
        void destroy();

        std::vector<VulkanShaderEntity>& getShaders()
        {
            return mShaders;
        }
    private:
        std::vector<char> readFile(const std::string &filename);
    private:
        VulkanDevicePtr                 mDevice;
        std::vector<VulkanShaderEntity> mShaders;
    };
}

#endif //HOMURA_VULKANSHADER_H
