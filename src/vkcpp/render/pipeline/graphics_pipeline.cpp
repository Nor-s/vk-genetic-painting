#include "graphics_pipeline.h"

#include "shader.h"
#include "device/device.h"
#include "render/render_stage.h"
#include "render/swapchain/swapchain.h"
#include "render/swapchain/render_pass.h"

#include <iostream>

namespace vkcpp
{
    GraphicsPipeline::GraphicsPipeline(const Device *device,
                                       const RenderStage *render_stage,
                                       const DescriptorSet *descriptor_set,
                                       std::string &vert_shader_file,
                                       std::string &frag_shader_file,
                                       int subpass_idx)
        : device_(device),
          render_stage_(render_stage),
          descriptor_set_(descriptor_set),
          vert_shader_file_(vert_shader_file),
          frag_shader_file_(frag_shader_file),
          subpass_idx_(subpass_idx),
          pipeline_bind_point_(VK_PIPELINE_BIND_POINT_GRAPHICS)
    {
        init_shader_stage_create_info_vec();
        init_vertex_input_state_create_info();
        init_input_assembly_state_create_info();
        init_viewport_state_create_info();
        init_rasterization_state_create_info();
        init_multisample_state_create_info();
        // Todo: VkPipelineDepthStencilStateCreateInfo = create
        init_color_blend_state_create_info();
        init_dynamic_state_create_info();
        init_pipeline_layout();
        init_pipeline();
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
        destroy();
    }

    void GraphicsPipeline::init_shader_stage_create_info_vec()
    {
        vert_shader_module_ = Shader::createShaderModule(device_, vert_shader_file_);
        frag_shader_module_ = Shader::createShaderModule(device_, frag_shader_file_);

        VkPipelineShaderStageCreateInfo vert_shader_stage_create_info{};

        vert_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_stage_create_info.module = vert_shader_module_;
        vert_shader_stage_create_info.pName = "main";

        VkPipelineShaderStageCreateInfo frag_shader_stage_create_info{};

        frag_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_stage_create_info.module = frag_shader_module_;
        frag_shader_stage_create_info.pName = "main";

        info_.shader_stages = {vert_shader_stage_create_info, frag_shader_stage_create_info};
    }

    void GraphicsPipeline::init_vertex_input_state_create_info()
    {
        VkPipelineVertexInputStateCreateInfo &vertex_input_info = info_.vertex_input_state;

        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto bindingDescription = Shader::Vertex2D::getBindingDescription();
        vertex_input_info.vertexBindingDescriptionCount = 1;
        vertex_input_info.pVertexBindingDescriptions = &bindingDescription;

        auto attributeDescriptions = Shader::Vertex2D::getAttributeDescriptions();
        vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertex_input_info.pVertexAttributeDescriptions = attributeDescriptions.data();
    }

    void GraphicsPipeline::init_input_assembly_state_create_info()
    {
        VkPipelineInputAssemblyStateCreateInfo &input_assembly = info_.input_assembly_state;

        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;
    }

    void GraphicsPipeline::init_viewport_state_create_info()
    {
        VkPipelineViewportStateCreateInfo &viewport_state = info_.viewport_state;
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;
    }

    void GraphicsPipeline::init_rasterization_state_create_info()
    {
        VkPipelineRasterizationStateCreateInfo &rasterizer = info_.rasterizer_state;

        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;        // VK_TRUE : fragment that are beyond the near and far plans are clamped.
        rasterizer.rasterizerDiscardEnable = VK_FALSE; // VK_TRUE : geometry never passes through the rasterizer stage.
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f;          // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional
    }

    void GraphicsPipeline::init_multisample_state_create_info()
    {
        VkPipelineMultisampleStateCreateInfo &multisampling = info_.multisample_state;

        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;          // Optional
        multisampling.pSampleMask = nullptr;            // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE;      // Optional
    }

    void GraphicsPipeline::init_color_blend_state_create_info()
    {
        VkPipelineColorBlendAttachmentState color_blend_attachment{};

        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

        VkPipelineColorBlendStateCreateInfo &color_blending = info_.color_blend_state;

        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable = VK_FALSE;   // VK_TRUE : bitwise combination.
        color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f; // Optional
        color_blending.blendConstants[1] = 0.0f; // Optional
        color_blending.blendConstants[2] = 0.0f; // Optional
        color_blending.blendConstants[3] = 0.0f; // Optional
    }

    void GraphicsPipeline::init_dynamic_state_create_info()
    {
        VkDynamicState dynamic_states[3] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_LINE_WIDTH};

        VkPipelineDynamicStateCreateInfo &dynamic_state = info_.dynamic_state;
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = 3;
        dynamic_state.pDynamicStates = dynamic_states;
    }

    void GraphicsPipeline::init_pipeline_layout()
    {
        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 1;
        pipeline_layout_info.pSetLayouts = &descriptor_set_layout_;
        pipeline_layout_info.pushConstantRangeCount = 0; // Optional
        pipeline_layout_info.pPushConstantRanges = 0;    // Optionnal

        if (vkCreatePipelineLayout(*device_, &pipeline_layout_info, nullptr, &layout_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void GraphicsPipeline::init_pipeline()
    {

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        // shader stage
        pipeline_info.stageCount = info_.shader_stages.size();
        pipeline_info.pStages = info_.shader_stages.data();
        // fixed-function state
        pipeline_info.pVertexInputState = &info_.vertex_input_state;
        pipeline_info.pInputAssemblyState = &info_.input_assembly_state;
        pipeline_info.pViewportState = &info_.viewport_state;
        pipeline_info.pRasterizationState = &info_.rasterizer_state;
        pipeline_info.pMultisampleState = &info_.multisample_state;
        pipeline_info.pDepthStencilState = nullptr; // Optional
        pipeline_info.pColorBlendState = &info_.color_blend_state;
        pipeline_info.pDynamicState = &info_.dynamic_state; // Optional
        // pipeline layout
        pipeline_info.layout = layout_;
        // render pass
        pipeline_info.renderPass = render_stage_->get_render_pass();
        pipeline_info.subpass = subpass_idx_;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipeline_info.basePipelineIndex = -1;              // Optional

        if (vkCreateGraphicsPipelines(*device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &handle_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
        vkDestroyShaderModule(*device_, vert_shader_module_, nullptr);
        vkDestroyShaderModule(*device_, frag_shader_module_, nullptr);
    }

    void GraphicsPipeline::destroy()
    {

        if (handle_ != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(*device_, handle_, nullptr);
            handle_ = VK_NULL_HANDLE;
        }
        if (layout_ != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(*device_, layout_, nullptr);
            layout_ = VK_NULL_HANDLE;
        }
    }
} // namespace vkcpp
