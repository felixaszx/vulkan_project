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
        // properties funtors
        struct ImageSampleCount
        {
            vk::SampleCountFlagBits sample_ = vk::SampleCountFlagBits::e1;
            ImageSampleCount(vk::SampleCountFlagBits sample);
            void operator()(vk::ImageCreateInfo& create_info);
        };

        struct MipLevelsAndLayers
        {
            uint32_t levels_ = 1;
            uint32_t layers_ = 1;
            MipLevelsAndLayers(uint32_t levels, uint32_t layers = 1);
            void operator()(vk::ImageCreateInfo& create_info);
        };

        struct ImageFormat
        {
            vk::Format format_ = {};
            ImageFormat(vk::Format format);
            void operator()(vk::ImageCreateInfo& create_info);
        };

        struct ColorAtchm
        {
            vk::ImageType type_ = vk::ImageType::e2D;
            ColorAtchm(vk::ImageType type = vk::ImageType::e2D);
            void operator()(vk::ImageCreateInfo& create_info);
        };

        struct DepthStencilAtchm
        {
            vk::ImageType type_ = vk::ImageType::e2D;
            DepthStencilAtchm(vk::ImageType type = vk::ImageType::e2D);
            void operator()(vk::ImageCreateInfo& create_info);
        };

    }; // namespace ext
};     // namespace proj

#endif // EXT_IMAGE_HPP
