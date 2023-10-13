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
            std::unique_ptr<Image> albedo_ = nullptr;
            std::unique_ptr<Image> specular_ = nullptr;
            std::unique_ptr<Image> ambient_ = nullptr;
            std::unique_ptr<Image> opacity_ = nullptr;
            std::unique_ptr<Image> normal_ = nullptr;
            std::unique_ptr<Image> emissive_ = nullptr;

            std::array<vk::DescriptorImageInfo, 6> infos_{};
            std::array<vk::DescriptorSetLayoutBinding, 6> bindings_{};

            template <MaterialCreateFunc... Nt>
            Material(vma::Allocator allocator,               //
                     vk::Queue queue, vk::CommandBuffer cmd, //
                     Nt&&... funcs)
                : image_creator_(allocator,           //
                                 ext::SampledImage(), //
                                 ext::device_local)
            {
                infos_.fill({{}, {}, vk::ImageLayout::eShaderReadOnlyOptimal});
                (std::forward<Nt>(funcs)(allocator, queue, cmd, *this), ...);
            }

            struct LoadAlbedo
            {
                vk::Sampler sampler_ = nullptr;
                const unsigned char* pixels_ = nullptr;
                const vk::Extent3D size_ = {};

                LoadAlbedo(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size);
                void operator()(vma::Allocator allocator,               //
                                vk::Queue queue, vk::CommandBuffer cmd, //
                                Material& a);
            };

            struct LoadSpecular
            {
                vk::Sampler sampler_ = nullptr;
                const unsigned char* pixels_ = nullptr;
                const vk::Extent3D size_ = {};

                LoadSpecular(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size);
                void operator()(vma::Allocator allocator,               //
                                vk::Queue queue, vk::CommandBuffer cmd, //
                                Material& a);
            };

            struct LoadAmbient
            {
                vk::Sampler sampler_ = nullptr;
                const unsigned char* pixels_ = nullptr;
                const vk::Extent3D size_ = {};

                LoadAmbient(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size);
                void operator()(vma::Allocator allocator,               //
                                vk::Queue queue, vk::CommandBuffer cmd, //
                                Material& a);
            };

            struct LoadOpacity
            {
                vk::Sampler sampler_ = nullptr;
                const unsigned char* pixels_ = nullptr;
                const vk::Extent3D size_ = {};

                LoadOpacity(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size);
                void operator()(vma::Allocator allocator,               //
                                vk::Queue queue, vk::CommandBuffer cmd, //
                                Material& a);
            };

            struct LoadNormal
            {
                vk::Sampler sampler_ = nullptr;
                const unsigned char* pixels_ = nullptr;
                const vk::Extent3D size_ = {};

                LoadNormal(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size);
                void operator()(vma::Allocator allocator,               //
                                vk::Queue queue, vk::CommandBuffer cmd, //
                                Material& a);
            };

            struct LoadEmissive
            {
                vk::Sampler sampler_ = nullptr;
                const unsigned char* pixels_ = nullptr;
                const vk::Extent3D size_ = {};

                LoadEmissive(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size);
                void operator()(vma::Allocator allocator,               //
                                vk::Queue queue, vk::CommandBuffer cmd, //
                                Material& a);
            };
        };

    }; // namespace render
};     // namespace proj

#endif // RENDER_MATERIAL_HPP
