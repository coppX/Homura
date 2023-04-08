#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <iostream>
#include <exception>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>

#include <filesystem.h>
#include <application.h>
#include <vulkanRHI.h>
#include <vulkanTexture.h>
#include <vulkanRenderPass.h>
#include <rhiResources.h>
#include <vulkanShader.h>

#include <new>
#include <functional>
#include <glm/gtx/hash.hpp>
static int width = 960;
static int height = 520;
static float aspect = width / (float)height;

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)});
        attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        return std::move(attributeDescriptions);
    }

    bool operator==(const Vertex& vertex) const
    {
        return pos == vertex.pos && color == vertex.color;
    }
};

template<>
struct std::hash<Vertex>
{
    size_t operator()(const Vertex& vertex) const
    {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1);
    }
};

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

size_t UpdateUniform(void* data, uint32_t size)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    memcpy(data, &ubo, size);
    return sizeof(ubo);
}


namespace Homura
{
    class TriangleApplication : Application {
    public:
        TriangleApplication()
            : rhi{std::make_shared<VulkanRHI>()}
        {

        }

        ~TriangleApplication()
        {
            exit();
        }

        bool init()
        {
            rhi->init(width, height, "triangle");
            rhi->setFramebufferResizeCallback([](int width, int height) -> void {
                aspect = width / (float)height;
                std::cout << "framebuffer size changed " << width << " " << height << std::endl;
            });
            rhi->setMouseButtonCallBack([](int button, int action, int mods) -> void {
                std::cout << "mouse clicked " << button << " " << action << " " << mods << std::endl;
            });

            rhi->setUpdateAfterRecreateSwapchain([this]() -> void {
                recordCommand();
            });
            VulkanTexture2DPtr colorImg = rhi->createColorResources();
            VulkanTextureDepthPtr depthImg = rhi->createDepthResources();
            ColorAttachmentDescription colorAttachmentDescription(colorImg->getFormat(), rhi->getSampleCount());
            DepthAttachmentDescription depthAttachmentDescription(depthImg->getFormat(), rhi->getSampleCount());
            ResolveAttachmentDescription resolveAttachmentDescription(colorImg->getFormat(), VK_SAMPLE_COUNT_1_BIT);

            RHIRenderPassInfo info;
            info.addAttachment(colorAttachmentDescription.getHandle());
            info.addAttachment(depthAttachmentDescription.getHandle());
            info.addAttachment(resolveAttachmentDescription.getHandle());

            AttachmentReference reference({colorAttachmentDescription, depthAttachmentDescription, resolveAttachmentDescription});
            VulkanSubPassPtr subPass = std::make_shared<VulkanSubPass>(reference);

            info.addSubPass(subPass);

            VkSubpassDependency dependency{};
            dependency.srcSubpass       = 0;
            dependency.dstSubpass       = VK_SUBPASS_EXTERNAL;
            dependency.srcStageMask     = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstStageMask     = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            dependency.srcAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dependency.dstAccessMask    = 0;
            dependency.dependencyFlags  = 0;
            info.addDependency(dependency);

            rhi->setupRenderPass(info);
            rhi->setupFramebuffer();

            auto vertexShader = rhi->setupShaders(FileSystem::getPath("resources/shader/triangle/triangle.vert.spv"), VERTEX);
            vertexShader->setVertexAttributeDescription(Vertex::getAttributeDescriptions());
            vertexShader->setVertexInputBindingDescription(Vertex::getBindingDescription());
            rhi->setupShaders(FileSystem::getPath("resources/shader/triangle/triangle.frag.spv"), FRAGMENT);
            rhi->createCommandBuffer();

            rhi->createUniformBuffer(0, sizeof(UniformBufferObject));
            rhi->setWriteDataCallback(UpdateUniform);

            setVertices();
            setIndices();

            rhi->createDescriptorSet();
            rhi->setupPipeline();

            recordCommand();

            update();
            return true;
        }

        void recordCommand()
        {
            rhi->beginCommandBuffer();
            rhi->createVertexBuffer(vertices.data(), sizeof(vertices[0]) * vertices.size(), vertices.size());
            rhi->createIndexBuffer(indices.data(), sizeof(indices[0]) * indices.size(), indices.size());
            rhi->draw();
            rhi->endCommandBuffer();
        }

        void setVertices()
        {
            vertices ={
                {
                    {1.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}
                },
                {
                    {-1.0f, 1.0f,  0.0f}, {0.0f, 1.0f, 0.0f}
                },
                {
                    {0.0f,  -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}
                }
            };
        }

        void setIndices()
        {
            indices = {0, 1, 2};
        }

        void exit()
        {
            rhi->exit();
        }

        void update()
        {
            rhi->update();
        }
    private:
        std::vector<Vertex>     vertices;
        std::vector<uint32_t>   indices;
        VulkanRHIPtr rhi;
    };
}

int main()
{
    Homura::TriangleApplication app;
    try
    {
        app.init();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_SUCCESS;
    }
	return 0;
}