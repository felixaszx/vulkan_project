#include "ext/image.hpp"

namespace proj
{
    namespace ext
    {
        Image ImageCreator::create(vk::Extent3D extent)
        {
            create_info_.extent = extent;
            return Image(*this, create_info_, alloc_info_);
        }

        Image ImageCreator::create(vk::Extent3D extent, const vk::ImageViewCreateInfo& view_info)
        {
            create_info_.extent = extent;
            return Image(*this, create_info_, alloc_info_, view_info);
        }

    }; // namespace ext
};     // namespace proj