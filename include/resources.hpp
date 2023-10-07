#ifndef INCLUDE_RESOURCES_HPP
#define INCLUDE_RESOURCES_HPP

#include "vk/vk.hpp"

namespace proj
{
    class Image : vk::Image,        //
                  vk::ImageView,    //
                  vk::DeviceMemory, //
                  vma::Allocation,  //
                  vma::Allocator
    {
      private:
        vk::Format image_format_ = {};

      public:
        Image(vma::Allocator allocator,              //
              const vk::ImageCreateInfo& image_info, //
              const vma::AllocationCreateInfo& alloc_info);
        Image(vma::Allocator allocator,                    //
              const vk::ImageCreateInfo& image_info,       //
              const vma::AllocationCreateInfo& alloc_info, //
              vk::ImageViewCreateInfo view_info);
        ~Image();

        void create_image_view(vk::ImageViewCreateInfo view_info);
        void destroy_image_view();

        vk::Format image_format() const { return image_format_; }
    };

    class Buffer : vk::Buffer,       //
                   vk::BufferView,   //
                   vk::DeviceMemory, //
                   vma::Allocation,  //
                   vma::Allocator
    {
      private:
        void* mapping_ = nullptr;

      public:
        const size_t size_ = 0;

        Buffer(vma::Allocator allocator,                //
               const vk::BufferCreateInfo& buffer_info, //
               const vma::AllocationCreateInfo& alloc_info);
        Buffer(vma::Allocator allocator,                    //
               const vk::BufferCreateInfo& buffer_info,     //
               const vma::AllocationCreateInfo& alloc_info, //
               vk::BufferViewCreateInfo view_info);
        ~Buffer();

        void create_buffer_view(vk::BufferViewCreateInfo view_info);
        void destroy_buffer_view();

        void* map_memory();
        void unmap_memory();
        void* mapping() const { return mapping_; }
    };
}; // namespace proj

#endif // INCLUDE_RESOURCES_HPP
