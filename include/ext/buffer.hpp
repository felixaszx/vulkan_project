#ifndef EXT_BUFFER_HPP
#define EXT_BUFFER_HPP

#include "resources.hpp"

namespace proj
{
    namespace ext
    {
        template <typename T>
        concept SetAllocatePropertyFunc = requires(T func, vma::AllocationCreateInfo& a) { func(a); };

        template <typename T>
        concept SetBufferCreatePropertyFunc = requires(T func, vk::BufferCreateInfo& c) { func(c); };
        template <typename T>
        concept SetBufferPropertyFunc = SetBufferCreatePropertyFunc<T> || SetAllocatePropertyFunc<T>;

        class BufferCreator : private vma::Allocator
        {
          public:
            vk::BufferCreateInfo create_info_{};
            vma::AllocationCreateInfo alloc_info_{};

            template <SetBufferPropertyFunc... Nt>
            BufferCreator(vma::Allocator allocator, Nt&&... funcs)
                : vma::Allocator(allocator)
            {
                update_flags(std::forward<Nt>(funcs)...);
            }

            template <SetBufferPropertyFunc... Nt>
            void update_flags(Nt&&... funcs)
            {
                (apply(std::forward<Nt>(funcs)), ...);
            }

            Buffer create(vk::DeviceSize size);
            Buffer create(vk::DeviceSize size, const vk::BufferViewCreateInfo& view_info);

          private:
            template <SetBufferPropertyFunc T>
            void apply(T&& func)
            {
                if constexpr (SetBufferCreatePropertyFunc<T>)
                {
                    std::forward<T>(func)(create_info_);
                }

                if constexpr (SetAllocatePropertyFunc<T>)
                {
                    std::forward<T>(func)(alloc_info_);
                }
            }
        };

        // buffer create properties
        inline void vertex_buffer(vk::BufferCreateInfo& create_info)
        {
            create_info.usage ^= vk::BufferUsageFlagBits::eVertexBuffer;
        }

        inline void index_buffer(vk::BufferCreateInfo& create_info)
        {
            create_info.usage ^= vk::BufferUsageFlagBits::eIndexBuffer;
        }

        inline void uniform_buffer(vk::BufferCreateInfo& create_info)
        {
            create_info.usage ^= vk::BufferUsageFlagBits::eUniformBuffer;
        }

        inline void trans_src(vk::BufferCreateInfo& create_info)
        {
            create_info.usage ^= vk::BufferUsageFlagBits::eTransferSrc;
        }

        inline void trans_dst(vk::BufferCreateInfo& create_info)
        {
            create_info.usage ^= vk::BufferUsageFlagBits::eTransferDst;
        }

        // allocate properties
        inline void host_seq(vma::AllocationCreateInfo& alloc_info)
        {
            alloc_info.usage = vma::MemoryUsage::eAutoPreferHost;
            alloc_info.flags ^= vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
            alloc_info.requiredFlags ^= vk::MemoryPropertyFlagBits::eHostCoherent;
        }

        inline void device_local(vma::AllocationCreateInfo& alloc_info)
        {
            alloc_info.usage = vma::MemoryUsage::eAutoPreferDevice;
            alloc_info.flags = {};
            alloc_info.requiredFlags = {};
        }

    }; // namespace ext
};     // namespace proj

#endif // EXT_BUFFER_HPP
