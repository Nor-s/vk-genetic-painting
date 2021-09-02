#include "command_buffers.h"

#include "device/device.h"
#include "command_pool.h"
#include "render/render_stage.h"
#include "render/pipeline/pipeline.hpp"
#include "render/swapchain/framebuffers.h"
#include "render/swapchain/render_pass.h"
#include "render/swapchain/swapchain.h"

#include <iostream>

namespace vkcpp
{
    CommandBuffers::CommandBuffers(const Device *device, const CommandPool *command_pool, uint32_t size, VkCommandBufferLevel level)
        : device_(device), command_pool_(command_pool), level_(level)
    {
        handle_.resize(size);
        init_command_buffers();
    }

    CommandBuffers::CommandBuffers(CommandBuffers &&a)
        : device_(a.device_), command_pool_(a.command_pool_), level_(std::move(a.level_)), handle_(std::move(a.handle_))
    {
        a.handle_.resize(0);
    }

    CommandBuffers::~CommandBuffers()
    {
        free_command_buffers();
    }

    CommandBuffers &CommandBuffers::operator=(CommandBuffers &&a)
    {
        if (this != &a)
        {
            device_ = a.device_;
            command_pool_ = a.command_pool_;
            level_ = a.level_;
            handle_ = std::move(a.handle_);
            a.handle_.resize(0);
        }
        return *this;
    }

    void CommandBuffers::init_command_buffers()
    {
        if (command_pool_ == nullptr)
        {
            throw std::runtime_error("failed to allocate command buffers : command pool is nullptr!");
        }
        command_pool_->alloc_buffers(handle_, handle_.size(), level_);
    }

    void CommandBuffers::free_command_buffers()
    {
        if (handle_.size() > 0)
        {
            vkFreeCommandBuffers(*device_, *command_pool_, handle_.size(), handle_.data());
            handle_.resize(0);
        }
    }

    void CommandBuffers::begin_command_buffer(int command_buffer_idx, VkCommandBufferUsageFlags flags)
    {
        VkCommandBufferBeginInfo begin_info{};

        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = flags;
        begin_info.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(handle_[command_buffer_idx], &begin_info) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
    }

    void CommandBuffers::begin_render_pass(int command_buffer_idx, const RenderStage *render_stage)
    {
        render_stage->begin_render_pass(handle_[command_buffer_idx], command_buffer_idx);
    }

    void CommandBuffers::bind_pipeline(int command_buffer_idx, const Pipeline *pipeline)
    {
        pipeline->bind_pipeline(handle_[command_buffer_idx]);
    }

    void CommandBuffers::end_render_pass(int command_buffer_idx, const RenderStage *render_stage)
    {
        render_stage->end_render_pass(handle_[command_buffer_idx], command_buffer_idx);
    }

    void CommandBuffers::end_command_buffer(int command_buffer_idx)
    {
        if (vkEndCommandBuffer(handle_[command_buffer_idx]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

} // namespace vkcpp
