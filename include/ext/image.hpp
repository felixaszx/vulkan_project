#ifndef EXT_IMAGE_HPP
#define EXT_IMAGE_HPP

#include "buffer.hpp"
#include "resources.hpp"

namespace proj
{
    namespace ext
    {
        template <typename T>
        concept SetImageCreatePropertyFunc = requires(T func, vk::ImageCreateInfo& c) { func(c); };
        template <typename T>
        concept SetImagePropertyFunc = SetImageCreatePropertyFunc<T> || SetAllocatePropertyFunc<T>;

        class ImageCreator : private vma::Allocator
        {
          public:
            vk::ImageCreateInfo create_info_{};
            vma::AllocationCreateInfo alloc_info_{};

            template <SetImagePropertyFunc... Nt>
            ImageCreator(vma::Allocator allocator, Nt&&... funcs)
                : vma::Allocator(allocator)
            {
                create_info_.mipLevels = 1;
                create_info_.arrayLayers = 1;
                create_info_.tiling = vk::ImageTiling::eOptimal;
                create_info_.initialLayout = vk::ImageLayout::eUndefined;
                create_info_.sharingMode = vk::SharingMode::eExclusive;
                create_info_.samples = vk::SampleCountFlagBits::e1;
                update_flags(std::forward<Nt>(funcs)...);
            }

            template <SetImagePropertyFunc... Nt>
            void update_flags(Nt&&... funcs)
            {
                (apply(std::forward<Nt>(funcs)), ...);
            }

            Image create(vk::Extent3D extent);
            Image create(vk::Extent3D extent, const vk::ImageViewCreateInfo& view_info);

          private:
            template <SetImagePropertyFunc T>
            void apply(T&& func)
            {
                if constexpr (SetImageCreatePropertyFunc<T>)
                {
                    std::forward<T>(func)(create_info_);
                }

                if constexpr (SetAllocatePropertyFunc<T>)
                {
                    std::forward<T>(func)(alloc_info_);
                }
            }
        };

        // image create properties
        inline void image_transfer_dst(vk::ImageCreateInfo& create_info)
        {
            create_info.usage ^= vk::ImageUsageFlagBits::eTransferDst;
        }

        inline void image_transfer_src(vk::ImageCreateInfo& create_info)
        {
            create_info.usage ^= vk::ImageUsageFlagBits::eTransferSrc;
        }

        // properties funtors
        struct sample_counts
        {
            vk::SampleCountFlagBits sample_ = vk::SampleCountFlagBits::e1;

            sample_counts(vk::SampleCountFlagBits sample)
                : sample_(sample)
            {
            }

            void operator()(vk::ImageCreateInfo& create_info) { create_info.samples = sample_; }
        };

        struct levels_and_layers
        {
            uint32_t levels_ = 1;
            uint32_t layers_ = 1;

            levels_and_layers(uint32_t levels, uint32_t layers = 1)
                : levels_(levels),
                  layers_(layers)
            {
            }

            void operator()(vk::ImageCreateInfo& create_info)
            {
                create_info.mipLevels = levels_;
                create_info.arrayLayers = layers_;
            }
        };

        struct image_format
        {
            vk::Format format_ = {};

            image_format(vk::Format format)
                : format_(format)
            {
            }

            void operator()(vk::ImageCreateInfo& create_info) { create_info.format = format_; }
        };

        struct color_atchm
        {
            vk::ImageType type_ = vk::ImageType::e2D;

            color_atchm(vk::ImageType type = vk::ImageType::e2D)
                : type_(type)
            {
            }

            void operator()(vk::ImageCreateInfo& create_info)
            {
                create_info.imageType = type_;
                create_info.usage ^= vk::ImageUsageFlagBits::eColorAttachment;
            }
        };

        struct depth_stencil_atchm
        {
            vk::ImageType type_ = vk::ImageType::e2D;

            depth_stencil_atchm(vk::ImageType type = vk::ImageType::e2D)
                : type_(type)
            {
            }

            void operator()(vk::ImageCreateInfo& create_info)
            {
                create_info.imageType = type_;
                create_info.usage ^= vk::ImageUsageFlagBits::eDepthStencilAttachment;
            }
        };

        struct image_sampled
        {
            vk::ImageType type_ = vk::ImageType::e2D;

            image_sampled(vk::ImageType type = vk::ImageType::e2D)
                : type_(type)
            {
            }

            void operator()(vk::ImageCreateInfo& create_info)
            {
                create_info.imageType = type_;
                create_info.usage ^= vk::ImageUsageFlagBits::eSampled;
            }
        };

    }; // namespace ext
};     // namespace proj

#endif // EXT_IMAGE_HPP
