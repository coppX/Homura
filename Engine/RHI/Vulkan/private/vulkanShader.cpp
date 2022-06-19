//
// Created by FDC on 2022/1/6.
//

#include <vulkanShader.h>
#include <vulkanDevice.h>
#include <debugUtils.h>
#include <fstream>

namespace Homura
{
    VulkanShader::VulkanShader(VulkanDevicePtr device, VkShaderStageFlagBits stage, std::string entryPoint)
        : mDevice{device}
        , mModule{VK_NULL_HANDLE}
        , mStage{stage}
        , mEntryPoint{entryPoint}
    {

    }

    VulkanShader::~VulkanShader()
    {
        destroyShaderModule();
    }

    void VulkanShader::createShaderModule(std::string filename)
    {
        std::vector<char> shaderCode = readFile(filename);

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCode.size();
        createInfo.pCode    = reinterpret_cast<const uint32_t*>(shaderCode.data());
        VERIFYVULKANRESULT(vkCreateShaderModule(mDevice->getHandle(), &createInfo, nullptr, &mModule));
    }

    void VulkanShader::destroyShaderModule()
    {
        if (mModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(mDevice->getHandle(), mModule, nullptr);
        }
    }

    std::vector<char> VulkanShader::readFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
}