#include "ext/buffer.hpp"

namespace proj
{
    namespace ext
    {
        Buffer BufferCreator::create(vk::DeviceSize size)
        {
            create_info_.size = size;
            return Buffer(*this, create_info_, alloc_info_);
        }

        Buffer BufferCreator::create(vk::DeviceSize size, const vk::BufferViewCreateInfo& view_info)
        {
            create_info_.size = size;
            return Buffer(*this, create_info_, alloc_info_, view_info);
        };

    }; // namespace ext
};     // namespace proj