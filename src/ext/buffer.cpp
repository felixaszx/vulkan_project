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

        void vertex_buffer(vk::BufferCreateInfo& create_info)
        {
            create_info.usage ^= vk::BufferUsageFlagBits::eVertexBuffer;
        }

        void index_buffer(vk::BufferCreateInfo& create_info)
        {
            create_info.usage ^= vk::BufferUsageFlagBits::eIndexBuffer;
        }

        void uniform_buffer(vk::BufferCreateInfo& create_info)
        {
            create_info.usage ^= vk::BufferUsageFlagBits::eUniformBuffer;
        }

        void trans_src(vk::BufferCreateInfo& create_info)
        {
            create_info.usage ^= vk::BufferUsageFlagBits::eTransferSrc;
        }

        void trans_dst(vk::BufferCreateInfo& create_info)
        {
            create_info.usage ^= vk::BufferUsageFlagBits::eTransferDst;
        }

        void host_seq(vma::AllocationCreateInfo& alloc_info)
        {
            alloc_info.usage = vma::MemoryUsage::eAutoPreferHost;
            alloc_info.flags ^= vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
            alloc_info.requiredFlags ^= vk::MemoryPropertyFlagBits::eHostCoherent;
        }

        void device_local(vma::AllocationCreateInfo& alloc_info)
        {
            alloc_info.usage = vma::MemoryUsage::eAutoPreferDevice;
            alloc_info.flags = {};
            alloc_info.requiredFlags = {};
        }
    }; // namespace ext
};     // namespace proj