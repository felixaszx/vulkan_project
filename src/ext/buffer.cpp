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

        Buffer BufferCreator::create(vk::DeviceSize size, vk::BufferViewCreateInfo view_info)
        {
            create_info_.size = size;
            return Buffer(*this, create_info_, alloc_info_, view_info);
        };

        void vertex(vk::BufferCreateInfo& create_info)
        {
            create_info.usage |= vk::BufferUsageFlagBits::eVertexBuffer;
        }

        void index(vk::BufferCreateInfo& create_info)
        {
            create_info.usage |= vk::BufferUsageFlagBits::eIndexBuffer;
        }

        void uniform(vk::BufferCreateInfo& create_info)
        {
            create_info.usage |= vk::BufferUsageFlagBits::eUniformBuffer;
        }

        void staging(vk::BufferCreateInfo& create_info)
        {
            create_info.usage |= vk::BufferUsageFlagBits::eTransferSrc;
        }

        void trans_dst(vk::BufferCreateInfo& create_info)
        {
            create_info.usage |= vk::BufferUsageFlagBits::eTransferDst;
        }

        void host_seq(vma::AllocationCreateInfo& alloc_info)
        {
            alloc_info.usage = vma::MemoryUsage::eAutoPreferHost;
            alloc_info.flags |= vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
            alloc_info.requiredFlags |= vk::MemoryPropertyFlagBits::eHostCoherent;
        }

        void device_local(vma::AllocationCreateInfo& alloc_info)
        {
            alloc_info.usage = vma::MemoryUsage::eAutoPreferDevice;
        }
    }; // namespace ext
};     // namespace proj