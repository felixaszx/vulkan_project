#ifndef INCLUDE_RENDER_HPP
#define INCLUDE_RENDER_HPP

#include <thread>
#include <fstream>
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
        RenderLoop(vk::Device device, uint32_t graphic_queue_index);
        ~RenderLoop();

        [[nodiscard]]
        vk::CommandBuffer get_cmd();
        vk::Semaphore get_curr_image_sem();
        vk::Semaphore get_curr_submit_sem();
        bool wait_for_curr_frame();
        void submit(vk::Queue graphics_queue, bool double_buffering = false);
        void end_cmd();
    };

    class RenderPassInfoHolder
    {
      private:
        std::vector<vk::RenderingAttachmentInfo> atchms_{};
        vk::RenderingAttachmentInfo depth_{};
        vk::RenderingAttachmentInfo stencil_{};

      public:
        void add_color_atchm(vk::ImageView view, //
                             vk::ImageLayout layout = vk::ImageLayout::eColorAttachmentOptimal,
                             bool clear_on_load = false, vk::ClearColorValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f});
        void set_depth_atchm(vk::ImageView view,                                                //
                             vk::ImageLayout layout = vk::ImageLayout::eDepthAttachmentOptimal, //
                             bool clear_on_load = false);
        void set_stencil_atchm(vk::ImageView view,                                                  //
                               vk::ImageLayout layout = vk::ImageLayout::eStencilAttachmentOptimal, //
                               bool clear_on_load = false);
        vk::RenderingInfo create_rendering_info(vk::Extent2D extent, uint32_t layers = 1);
    };

    class ShaderModule : public vk::Device,       //
                         public vk::ShaderModule, //
                         public vk::PipelineShaderStageCreateInfo
    {
      private:
        const std::string entry_;
        void set_zero();

      public:
        ShaderModule(vk::Device device, const std::string& file_name, //
                     vk::ShaderStageFlagBits stage, const std::string& entry = "main");
        ShaderModule(ShaderModule&& shader);
        ~ShaderModule();
    };

    struct GraphicsPipelineBuilder
    {
        vk::PipelineVertexInputStateCreateInfo input_state_{};
        vk::PipelineViewportStateCreateInfo viewport_state_{};
        vk::PipelineRasterizationStateCreateInfo rasterizer_state_{};
        vk::PipelineColorBlendStateCreateInfo blend_state_{};
        vk::PipelineDepthStencilStateCreateInfo depth_state_{};
        vk::PipelineInputAssemblyStateCreateInfo input_assembly_{};
        vk::PipelineMultisampleStateCreateInfo multisample_state_{};
        vk::PipelineDynamicStateCreateInfo dynamic_states_{};
        vk::PipelineRenderingCreateInfo rendering_info_{};

        ShaderModule* vert_ = nullptr;
        ShaderModule* geom_ = nullptr;
        ShaderModule* frag_ = nullptr;

        GraphicsPipelineBuilder();

        vk::Pipeline create_graphics_pipeline(vk::Device device, vk::PipelineLayout layout);
    };
} // namespace proj

#endif // INCLUDE_RENDER_HPP
