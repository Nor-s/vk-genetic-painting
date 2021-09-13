#ifndef VKCPP_RENDER_BUFFER_SHADER_ATTRIBUTE_HPP
#define VKCPP_RENDER_BUFFER_SHADER_ATTRIBUTE_HPP

#include "vulkan_header.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <vector>

namespace vkcpp
{
    namespace shader
    {
        namespace attribute
        {
            struct Vertex
            {
                glm::vec3 pos;
                glm::vec3 color;
                glm::vec2 texCoord;

                static VkVertexInputBindingDescription getBindingDescription()
                {
                    VkVertexInputBindingDescription bindingDescription{};
                    bindingDescription.binding = 0;
                    bindingDescription.stride = sizeof(Vertex);
                    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                    return bindingDescription;
                }

                static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
                {
                    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

                    attributeDescriptions[0].binding = 0;
                    // shader input location = 0
                    attributeDescriptions[0].location = 0;
                    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                    attributeDescriptions[0].offset = offsetof(Vertex, pos);

                    attributeDescriptions[1].binding = 0;
                    // shader input location = 1
                    attributeDescriptions[1].location = 1;
                    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                    attributeDescriptions[1].offset = offsetof(Vertex, color);

                    attributeDescriptions[2].binding = 0;
                    // shader input location = 2
                    attributeDescriptions[2].location = 2;
                    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
                    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

                    return attributeDescriptions;
                }

            }; // struct Vertex2D
            struct TransformUBO
            {
                alignas(16) glm::mat4 model;
                alignas(16) glm::mat4 view;
                alignas(16) glm::mat4 proj;
                alignas(16) glm::vec4 color;
            }; // TransformUB
        }
    }
} // namespace vkcpk

#endif