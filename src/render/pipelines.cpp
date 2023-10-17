#include "render/pipelines.hpp"

namespace proj
{
    namespace render
    {
        void DefferedPipelineSingleton::load_subpass_info()
        {
            for (uint32_t i = 0; i < 5; i++)
            {
                atchm_refes_[i][0].attachment = i;
                atchm_refes_[i][0].layout = vk::ImageLayout::eColorAttachmentOptimal;
                atchm_refes_[i][1].attachment = i;
                atchm_refes_[i][1].layout = vk::ImageLayout::eColorAttachmentOptimal;
            }
            atchm_refes_[4][0].layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            atchm_refes_[4][1].attachment = 5;
            atchm_refes_[4][1].layout = vk::ImageLayout::eColorAttachmentOptimal;

            dependencies_[0].srcSubpass = 0;
            dependencies_[0].dstSubpass = 1;
            dependencies_[0].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            dependencies_[0].dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
            dependencies_[0].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | //
                                             vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            dependencies_[0].dstAccessMask = vk::AccessFlagBits::eInputAttachmentRead;

            dependencies_[1].srcSubpass = 1;
            dependencies_[1].dstSubpass = 2;
            dependencies_[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            dependencies_[1].dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
            dependencies_[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
            dependencies_[1].dstAccessMask = vk::AccessFlagBits::eInputAttachmentRead;
        }

        void DefferedPipelineSingleton::load_render_pass() {}

        DefferedPipelineSingleton::DefferedPipelineSingleton() {}

    }; // namespace render
};     // namespace proj
