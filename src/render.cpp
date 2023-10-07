#include "render.hpp"

namespace proj
{
    RenderLoop::RenderLoop(vk::Device device)
        : device_(device)
    {
        for (int i = 0; i < 2; i++)
        {
            vk::FenceCreateInfo fence_info{};
            fence_info.flags = vk::FenceCreateFlagBits::eSignaled;
            try_vk(frame_fence_[i] = device.createFence(fence_info));

            vk::SemaphoreCreateInfo sem_info{};
            image_sems_[i] = device.createSemaphore(sem_info);
            submit_sems_[i] = device.createSemaphore(sem_info);
        }

        vk::CommandPoolCreateInfo cmd_pool_info{};
        cmd_pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        cmd_pool_ = device.createCommandPool(cmd_pool_info);

        vk::CommandBufferAllocateInfo cmd_alloc_info{};
        cmd_alloc_info.level = vk::CommandBufferLevel::ePrimary;
        cmd_alloc_info.commandPool = cmd_pool_;
        cmd_alloc_info.commandBufferCount = 2;
        main_cmds_ = device.allocateCommandBuffers(cmd_alloc_info);
    }

    RenderLoop::~RenderLoop()
    {
        for (int i = 0; i < 2; i++)
        {
            device_.destroyFence(frame_fence_[i]);
            device_.destroySemaphore(image_sems_[i]);
            device_.destroySemaphore(submit_sems_[i]);
        }

        device_.destroyCommandPool(cmd_pool_);
    }

    vk::Semaphore RenderLoop::get_curr_image_sem()
    {
        return image_sems_[frame_];
    }

    vk::Semaphore RenderLoop::get_curr_submit_sem()
    {
        return submit_sems_[frame_];
    }

    bool RenderLoop::wait_for_curr_frame()
    {
        try_vk(auto result = device_.waitForFences(frame_fence_[frame_], true, UINT64_MAX), //
               "",                                                                          //
               return false);

        return true;
    }

    vk::CommandBuffer RenderLoop::begin_cmd()
    {
        return main_cmds_[frame_];
    }

    void RenderLoop::end_cmd()
    {
        main_cmds_[frame_].end();
    }

    void RenderLoop::submit(vk::Queue graphics_queue, bool double_buffering)
    {
        vk::PipelineStageFlags wait_stages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        vk::SubmitInfo submit_info{};
        submit_info.setWaitDstStageMask(wait_stages);
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = image_sems_ + frame_;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = submit_sems_ + frame_;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &main_cmds_[frame_];
        graphics_queue.submit(submit_info, frame_fence_[frame_]);

        if (double_buffering)
        {
            frame_ = (frame_ + 1) % 2;
        }
    }

    void RenderPassInfoHolder::add_color_atchm(vk::ImageView view, vk::ImageLayout layout)
    {
        atchms_.push_back({});
        atchms_.back().imageView = view;
        atchms_.back().imageLayout = layout;
        atchms_.back().loadOp = vk::AttachmentLoadOp::eClear;
        atchms_.back().storeOp = vk::AttachmentStoreOp::eStore;
        atchms_.back().clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
    }

    void RenderPassInfoHolder::set_depth_atchm(vk::ImageView view, vk::ImageLayout layout, bool clear)
    {
        depth_.imageView = view;
        depth_.imageLayout = layout;
        depth_.loadOp = clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
        depth_.storeOp = vk::AttachmentStoreOp::eStore;
        depth_.clearValue.setDepthStencil({1.0f, 0u});
    }

    void RenderPassInfoHolder::set_stencil_atchm(vk::ImageView view, vk::ImageLayout layout, bool clear)
    {
        stencil_.imageView = view;
        stencil_.imageLayout = layout;
        stencil_.loadOp = clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
        stencil_.storeOp = vk::AttachmentStoreOp::eStore;
        stencil_.clearValue.setDepthStencil({1.0f, 0u});
    }

    vk::RenderingInfo RenderPassInfoHolder::create_rendering_info(vk::Rect2D extent, uint32_t layers)
    {
        vk::RenderingInfo info{};
        info.renderArea = extent;
        info.setColorAttachments(atchms_);
        info.layerCount = layers;
        info.pDepthAttachment = depth_.imageView ? &depth_ : nullptr;
        info.pStencilAttachment = stencil_.imageView ? &stencil_ : nullptr;
        return info;
    }
} // namespace proj