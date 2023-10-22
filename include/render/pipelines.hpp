#ifndef RENDER_PIPELINES_HPP
#define RENDER_PIPELINES_HPP

#include "vk/vk.hpp"
#include "pipeline_layout.hpp"
#include "render.hpp"
#include "ext/image.hpp"

namespace proj
{
    namespace render
    {
        class DefferedPipelineSingleton : vk::Device
        {
            //  fbo layout
            //  0  |  1  |  2  |  3  |  4  |  5
            //  c  |  c  |  c  |  c  |  d  |  c  // first subpass
            //  i  |  i  |  i  |  i  |  d  |  c  // second subpass
          private:
            void load_subpass_info();
            void load_render_pass();

          public:
            vk::RenderPass render_pass_ = nullptr;
            vk::Framebuffer framebuffer_ = nullptr;

            vk::SubpassDescription subpasses_[2]{};
            vk::SubpassDependency dependencies_[1]{};

            vk::AttachmentDescription atchm_des_[6]{};
            vk::AttachmentReference atchm_refes_[2][5]{};

            std::vector<Image> images_n_views_{};
            std::vector<vk::PipelineColorBlendAttachmentState> blend_states_[2];

            DefferedPipelineSingleton(vk::Device device);
            ~DefferedPipelineSingleton();

            void create_framebuffer(vma::Allocator allocator, vk::Extent2D extent);

            // pipeline details
            vk::Pipeline pipelines_[2]{};
            vk::PipelineLayout layouts_[2]{};
            std::unique_ptr<DescriptorLayout> des_layouts_[2]{};

            void generate_pipeline_layout();
        };
    }; // namespace render
};     // namespace proj

#endif // RENDER_PIPELINES_HPP