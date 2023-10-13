#ifndef RENDER_MATERIAL_HPP
#define RENDER_MATERIAL_HPP

#include <memory>
#include "stb/stb_image.h"
#include "ext/image.hpp"
#include "vk/vk.hpp"
#include "glms.hpp"
#include "pipeline_layout.hpp"

namespace proj
{
    namespace render
    {
        struct Material;
        template <typename T>
        concept MaterialCreateFunc = requires(T func,                                 //
                                              vma::Allocator allocator,               //
                                              vk::Queue queue, vk::CommandBuffer cmd, //
                                              Material& a)                            //
        { func(allocator, queue, cmd, a); };

        struct Material
        {
            ext::ImageCreator image_creator_;
            std::vector<Image> textures_{};
            std::vector<vk::DescriptorImageInfo> infos_{};

            template <MaterialCreateFunc... Nt>
            Material(vma::Allocator allocator,               //
                     vk::Queue queue, vk::CommandBuffer cmd, //
                     Nt&&... funcs)
                : image_creator_(allocator,                                    //
                                 ext::image_sampled(),                         //
                                 ext::image_transfer_dst,                      //
                                 ext::image_transfer_src,                      //
                                 ext::image_format(vk::Format::eR8G8B8A8Srgb), //
                                 ext::device_local)
            {
                (std::forward<Nt>(funcs)(allocator, queue, cmd, *this), ...);
            }

            struct load_general
            {
                vk::Sampler sampler_ = nullptr;
                const unsigned char* pixels_ = nullptr;
                const vk::Extent3D size_ = {};

                load_general(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size);
                void operator()(vma::Allocator allocator,               //
                                vk::Queue queue, vk::CommandBuffer cmd, //
                                Material& a);
            };

            struct load_mipmapped
            {
                vk::Sampler sampler_ = nullptr;
                const unsigned char* pixels_ = nullptr;
                const vk::Extent3D size_ = {};

                load_mipmapped(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size);
                void operator()(vma::Allocator allocator,               //
                                vk::Queue queue, vk::CommandBuffer cmd, //
                                Material& a);
            };
        };

    }; // namespace render
};     // namespace proj

#endif // RENDER_MATERIAL_HPP
