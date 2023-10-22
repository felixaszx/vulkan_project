#include "render/pipelines.hpp"

namespace proj
{
    namespace render
    {
        void DefferedPipelineSingleton::load_subpass_info()
        {
            for (uint32_t i = 0; i < 5; i++)
            {
                atchm_refes_[0][i].attachment = i;
                atchm_refes_[0][i].layout = vk::ImageLayout::eColorAttachmentOptimal;
                atchm_refes_[1][i].attachment = i;
                atchm_refes_[1][i].layout = vk::ImageLayout::eShaderReadOnlyOptimal;
            }
            atchm_refes_[0][4].layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            atchm_refes_[1][4].attachment = 5;
            atchm_refes_[1][4].layout = vk::ImageLayout::eColorAttachmentOptimal;
            ;

            subpasses_[0].pColorAttachments = atchm_refes_[0];
            subpasses_[0].colorAttachmentCount = 4;
            subpasses_[0].pDepthStencilAttachment = atchm_refes_[0] + 4;

            subpasses_[1].pColorAttachments = atchm_refes_[1] + 4;
            subpasses_[1].colorAttachmentCount = 1;
            subpasses_[1].pInputAttachments = atchm_refes_[1];
            subpasses_[1].inputAttachmentCount = 4;

            dependencies_[0].srcSubpass = 0;
            dependencies_[0].dstSubpass = 1;
            dependencies_[0].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | //
                                            vk::PipelineStageFlagBits::eLateFragmentTests;
            dependencies_[0].dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
            dependencies_[0].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | //
                                             vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            dependencies_[0].dstAccessMask = vk::AccessFlagBits::eInputAttachmentRead | //
                                             vk::AccessFlagBits::eDepthStencilAttachmentRead;

            blend_states_[0].resize(4);
            blend_states_[1].resize(1);
            for (int i = 0; i < 4; i++)
            {
                blend_states_[0][i].colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                     vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
            }
            blend_states_[1][0].colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                 vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        }

        void DefferedPipelineSingleton::load_render_pass()
        {
            for (int i = 0; i < 6; i++)
            {
                atchm_des_[i].samples = vk::SampleCountFlagBits::e1;
                atchm_des_[i].format = vk::Format::eR32G32B32A32Sfloat;
                atchm_des_[i].initialLayout = vk::ImageLayout::eUndefined;
                atchm_des_[i].finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                atchm_des_[i].loadOp = vk::AttachmentLoadOp::eClear;
                atchm_des_[i].storeOp = vk::AttachmentStoreOp::eStore;
                atchm_des_[i].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
                atchm_des_[i].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            }
            atchm_des_[4].format = vk::Format::eD32Sfloat;

            vk::RenderPassCreateInfo render_pass_info{};
            render_pass_info.setSubpasses(subpasses_);
            render_pass_info.setAttachments(atchm_des_);
            render_pass_info.setDependencies(dependencies_);
            render_pass_ = this->createRenderPass(render_pass_info);
        }

        DefferedPipelineSingleton::DefferedPipelineSingleton(vk::Device device)
            : vk::Device(device)
        {
            load_subpass_info();
            load_render_pass();
        }

        DefferedPipelineSingleton::~DefferedPipelineSingleton()
        {
            this->destroyFramebuffer(framebuffer_);
            this->destroyRenderPass(render_pass_);
        }

        void DefferedPipelineSingleton::create_framebuffer(vma::Allocator allocator, vk::Extent2D extent)
        {
            ext::ImageCreator atchm_creator(allocator,          // add
                                            ext::color_atchm(), // add
                                            ext::device_local,  // add
                                            ext::image_input_atchm);
            images_n_views_.reserve(6);
            for (int i = 0; i < 6; i++)
            {
                atchm_creator.update_flags(ext::image_format(atchm_des_[i].format)); // set
                if (i == 4)
                {
                    atchm_creator.update_flags(ext::color_atchm(),          // remove
                                               ext::depth_stencil_atchm()); // add
                    images_n_views_.push_back(atchm_creator.create(vk::Extent3D(extent, 1)));
                    atchm_creator.update_flags(ext::color_atchm(),          // add
                                               ext::depth_stencil_atchm()); // remove
                    continue;
                }
                images_n_views_.push_back(atchm_creator.create(vk::Extent3D(extent, 1)));
            }

            vk::ImageView views[6]{};
            for (int i = 0; i < 6; i++)
            {
                vk::ImageViewCreateInfo view_info{};
                view_info.viewType = vk::ImageViewType::e2D;
                view_info.format = atchm_des_[i].format;
                view_info.subresourceRange.aspectMask =
                    (i != 4 ? vk::ImageAspectFlagBits::eColor : vk::ImageAspectFlagBits::eDepth);
                view_info.subresourceRange.levelCount = 1;
                view_info.subresourceRange.layerCount = 1;
                images_n_views_[i].create_image_view(view_info);
                views[i] = images_n_views_[i];
            }

            vk::FramebufferCreateInfo framebuffer_info{};
            framebuffer_info.setAttachments(views);
            framebuffer_info.renderPass = render_pass_;
            framebuffer_info.layers = 1;
            framebuffer_info.width = extent.width;
            framebuffer_info.height = extent.height;
            framebuffer_ = allocator.getAllocatorInfo().device.createFramebuffer(framebuffer_info);
            generate_pipeline_layout();
        }

        void DefferedPipelineSingleton::generate_pipeline_layout()
        {
            vk::DescriptorSetLayoutBinding bindings[11]{};
            bindings[0] = {0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex};
            for (int i = 1; i < 7; i++)
            {
                bindings[i] = {static_cast<uint32_t>(i), vk::DescriptorType::eCombinedImageSampler, //
                               1, vk::ShaderStageFlagBits::eFragment};
            }
            for (int i = 7; i < 11; i++)
            {
                bindings[i] = {static_cast<uint32_t>(i), vk::DescriptorType::eInputAttachment, //
                               1, vk::ShaderStageFlagBits::eFragment};
            }
            des_layouts_[0].reset(new DescriptorLayout(*this, {bindings, bindings + 7}));
            des_layouts_[1].reset(new DescriptorLayout(*this, {bindings, bindings + 7}));

            for (int i = 0; i < 2; i++)
            {
                vk::PipelineLayoutCreateInfo layout_info{};
                layout_info.pSetLayouts = des_layouts_[i].get();
                layout_info.setLayoutCount = 1;
                layouts_[i] = this->createPipelineLayout(layout_info);
            }

            vk::PipelineColorBlendAttachmentState blend_attachment_state{};
            blend_attachment_state.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                    vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
            std::vector<vk::PipelineColorBlendAttachmentState> blend_attachment_states(4, blend_attachment_state);
            color_blends_[0].setAttachments(blend_attachment_states);

            blend_attachment_state.blendEnable = true;
            blend_attachment_state.colorBlendOp = vk::BlendOp::eAdd;
            blend_attachment_state.srcColorBlendFactor = vk::BlendFactor::eOne;
            blend_attachment_state.dstColorBlendFactor = vk::BlendFactor::eOne;
            color_blends_[1].setAttachments(blend_attachment_states[0]);

            multisample_state_.rasterizationSamples = vk::SampleCountFlagBits::e1;
        }

    }; // namespace render
};     // namespace proj
