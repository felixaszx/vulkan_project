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
        class DefferedPipelineSingleton
        {
          private:
            void load_subpass_info();
            void load_render_pass();

          public:
            vk::RenderPass render_pass_ = nullptr;
            vk::Framebuffer framebufer_ = nullptr;

            vk::SubpassDescription subpasses_[3]{};
            vk::SubpassDependency dependencies_[2]{};

            vk::AttachmentDescription atchm_des_[6]{};
            vk::AttachmentReference atchm_refes_[5][3]{};

            DefferedPipelineSingleton();
            ~DefferedPipelineSingleton();
        };
    }; // namespace render
};     // namespace proj

#endif // RENDER_PIPELINES_HPP
