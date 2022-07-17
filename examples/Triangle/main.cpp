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

#include <filesystem.h>
#include <application.h>
#include <vulkanRHI.h>
#include <vulkanVertex.h>
#include <vulkanTexture.h>
#include <vulkanRenderPass.h>
#include <rhiResources.h>
#include <vulkanShader.h>

#include <new>
namespace Homura
{
    const std::string MODEL_PATH = FileSystem::getPath("resources/models/viking_room.obj");
    const std::string TEXTURE_PATH = FileSystem::getPath("resources/textures/viking_room.png");

    const int MAX_FRAMES_IN_FLIGHT = 2;

    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    void OnFramebufferChanged(int width, int height)
    {
        std::cout << "framebuffer size changed " << width << " " << height << std::endl;
    }

    void OnMouseButtonClicked(int button, int action, int mods)
    {
        std::cout << "mouse clicked " << button << " " << action << " " << mods << std::endl;
    }


    class TriangleApplication : Application
    {
    public:
        TriangleApplication()
            : rhi{ std::make_shared<VulkanRHI>(960, 540) }
        {
            
        }

        ~TriangleApplication()
        {
            exit();
        }

        bool init()
        {
            rhi->init();
            rhi->addFramebufferResizeCallback(&OnFramebufferChanged);
            rhi->addMouseButtonCallBack(&OnMouseButtonClicked);

            VulkanTexture2DPtr colorImg = rhi->createColorResources();
            VulkanTextureDepthPtr depthImg = rhi->createDepthResources();
            colorImages.push_back(colorImg);
            depthStencilImages.push_back(depthImg);

            RHIRenderPassInfo info;
            VkAttachmentDescription colorAttachmentDescription{};
            colorAttachmentDescription.format                       = colorImg->getFormat();
            colorAttachmentDescription.samples                      = rhi->getSampleCount();
            colorAttachmentDescription.loadOp                       = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachmentDescription.storeOp                      = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentDescription.stencilLoadOp                = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentDescription.stencilStoreOp               = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentDescription.initialLayout                = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentDescription.finalLayout                  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentDescription depthAttachmentDescription{};
            depthAttachmentDescription.format                       = depthImg->getFormat();
            depthAttachmentDescription.samples                      = rhi->getSampleCount();
            depthAttachmentDescription.loadOp                       = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachmentDescription.storeOp                      = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachmentDescription.stencilLoadOp                = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachmentDescription.stencilStoreOp               = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachmentDescription.initialLayout                = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachmentDescription.finalLayout                  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentDescription colorAttachmentResolve{};
            colorAttachmentResolve.format                           = colorImg->getFormat();
            colorAttachmentResolve.samples                          = VK_SAMPLE_COUNT_1_BIT;
            colorAttachmentResolve.loadOp                           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.storeOp                          = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentResolve.stencilLoadOp                    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.stencilStoreOp                   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentResolve.initialLayout                    = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentResolve.finalLayout                      = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


            info.addAttachment(colorAttachmentDescription);
            info.addAttachment(depthAttachmentDescription);
            info.addAttachment(colorAttachmentResolve);

            AttachmentReference reference({colorAttachmentDescription, depthAttachmentDescription, colorAttachmentResolve });
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
            rhi->setupFramebuffer(colorImages, depthStencilImages);
            
            auto vertexShader = rhi->setupShaders(FileSystem::getPath("resources/shader/triangle/triangle.vert.spv"), ShaderType::VERTEX);
            vertexShader->setVertexAttributeDescription(Vertex::getAttributeDescriptions());
            vertexShader->setVertexInputBindingDescription(Vertex::getBindingDescription());
            rhi->setupShaders(FileSystem::getPath("resources/shader/triangle/triangle.frag.spv"), ShaderType::FRAGMENT);

            std::shared_ptr<VulkanDescriptorSet> descriptorSet = rhi->createDescriptorSet(getDescriptorSetLayoutBinding());
            rhi->setupPipeline(descriptorSet);
            
            loadModel();

            rhi->createCommandBuffer();
            rhi->beginCommandBuffer();
            rhi->createBuffer(vertices.data(), vertices.size(), BufferType::VERTEX);
            rhi->createBuffer(indices.data(), indices.size(), BufferType::INDEX);
            rhi->draw();
            rhi->endCommandBuffer();

            update();
            return true;
        }

        std::vector<VkDescriptorSetLayoutBinding> getDescriptorSetLayoutBinding()
        {
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = 1;
            samplerLayoutBinding.descriptorCount = 1;
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            return {uboLayoutBinding, samplerLayoutBinding};
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

        void loadModel()
        {
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string warn, err;

            if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str()))
            {
                throw std::runtime_error(warn + err);
            }

            std::unordered_map<Vertex, uint32_t> uniqueVertices{};

            for (const auto &shape : shapes)
            {
                for (const auto &index : shape.mesh.indices)
                {
                    Vertex vertex{};

                    vertex.pos = {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.texCoord = {
                            attrib.texcoords[2 * index.texcoord_index + 0],
                            1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };

                    vertex.color = {1.0f, 1.0f, 1.0f};

                    if (uniqueVertices.count(vertex) == 0)
                    {
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(vertex);
                    }

                    indices.push_back(uniqueVertices[vertex]);
                }
            }
        }

        std::vector<Vertex>                 vertices;
        std::vector<uint32_t>               indices;
        VulkanRHIPtr                        rhi;

        std::vector<VulkanTexture2DPtr>     colorImages;
        std::vector<VulkanTextureDepthPtr>  depthStencilImages;
        VulkanVertexBufferPtr               mVertexBuffer;
        VulkanIndexBufferPtr                mIndexBuffer;
    };
}

int main()
{
    Homura::TriangleApplication app;
    try
    {
        app.init();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
