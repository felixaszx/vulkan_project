#ifndef INCLUDE_RENDER_HPP
#define INCLUDE_RENDER_HPP

#include <thread>
#include "vk/vk.hpp"

namespace proj
{
    class RenderLoop
    {
      private:
        vk::Device device_ = nullptr;

        std::atomic_int frame_ = 0;
        vk::Fence frame_fence_[2];
        vk::Semaphore image_sems_[2];
        vk::Semaphore submit_sems_[2];

        vk::CommandPool cmd_pool_ = nullptr;
        std::vector<vk::CommandBuffer> main_cmds_{2};

      public:
        RenderLoop(vk::Device device);
        ~RenderLoop();

        vk::Semaphore get_curr_image_sem();
        vk::Semaphore get_curr_submit_sem();
        bool wait_for_curr_frame();

        vk::CommandBuffer begin_cmd();
        void submit(vk::Queue graphics_queue, bool double_buffering = false);
        void end_cmd();
    };

    class RenderPassInfoHolder
    {
      private:
        std::vector<vk::RenderingAttachmentInfo> atchms_{};
        vk::RenderingAttachmentInfo depth_stencil_{};

      public:
        void add_color_atchm(vk::ImageView view, //
                             vk::ImageLayout layout = vk::ImageLayout::eColorAttachmentOptimal);
        void set_depth_stencil_atchm(vk::ImageView view,
                                     vk::ImageLayout layout = vk::ImageLayout::eDepthStencilAttachmentOptimal);
        vk::RenderingInfo create_render_pass_info(vk::Rect2D extent, uint32_t layers = 1);
    };
} // namespace proj

#endif // INCLUDE_RENDER_HPP
