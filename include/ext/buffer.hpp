#ifndef EXT_BUFFER_HPP
#define EXT_BUFFER_HPP

#include "resources.hpp"

namespace proj
{
    namespace ext
    {
        typedef void(set_create_property_func_type)(vk::BufferCreateInfo& create_info);
        typedef void(set_allocate_property_func_type)(vma::AllocationCreateInfo& alloc_info);

        template <typename T>
        concept SetCreatePropertyFunc = requires(T func, vk::BufferCreateInfo& c) { func(c); };

        template <typename T>
        concept SetAllocatePropertyFunc = requires(T func, vma::AllocationCreateInfo& a) { func(a); };

        template <typename T>
        concept SetPropertyFunc = SetCreatePropertyFunc<T> || SetAllocatePropertyFunc<T>;

        class BufferCreator : private vma::Allocator
        {
          public:
            vk::BufferCreateInfo create_info_{};
            vma::AllocationCreateInfo alloc_info_{};

            template <SetPropertyFunc... Nt>
            BufferCreator(vma::Allocator allocator, Nt... funcs)
                : vma::Allocator(allocator)
            {
                (apply(funcs), ...);
            }

            Buffer create(vk::DeviceSize size);
            Buffer create(vk::DeviceSize size, vk::BufferViewCreateInfo view_info);

          private:
            template <SetPropertyFunc T>
            void apply(T func)
            {
                if constexpr (SetCreatePropertyFunc<T>)
                {
                    func(create_info_);
                }

                if constexpr (SetAllocatePropertyFunc<T>)
                {
                    func(alloc_info_);
                }
            }
        };

        void vertex(vk::BufferCreateInfo& create_info);
        void index(vk::BufferCreateInfo& create_info);
        void uniform(vk::BufferCreateInfo& create_info);
        void staging(vk::BufferCreateInfo& create_info);
        void trans_dst(vk::BufferCreateInfo& create_info);

        void host_seq(vma::AllocationCreateInfo& alloc_info);
        void device_local(vma::AllocationCreateInfo& alloc_info);

    }; // namespace ext
};     // namespace proj

#endif // EXT_BUFFER_HPP
