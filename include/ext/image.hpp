#ifndef EXT_IMAGE_HPP
#define EXT_IMAGE_HPP

#include "resources.hpp"

namespace proj
{
    namespace ext
    {
        struct ImageCreator2D : private vma::Allocator
        {
            vk::ImageCreateInfo create_info_{};
            vk::ImageViewCreateInfo view_info_{};
            vma::AllocationCreateInfo alloc_info_{};

            ImageCreator2D(vma::Allocator allocator);

            Image create(vk::Extent3D extent, bool create_view);
            Image operator()(vk::DeviceSize size, bool create_view);
        };
    }; // namespace ext
};     // namespace proj

#endif // EXT_IMAGE_HPP
