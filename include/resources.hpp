#ifndef INCLUDE_RESOURCES_HPP
#define INCLUDE_RESOURCES_HPP

#include "vk/vk.hpp"

namespace proj
{
    class Image : public vk::Image,        //
                  public vk::ImageView,    //
                  public vk::DeviceMemory, //
                  public vma::Allocation,  //
                  public vma::Allocator
    {
      private:
        void set_zero();

      public:
        Image(vma::Allocator allocator,              //
              const vk::ImageCreateInfo& image_info, //
              const vma::AllocationCreateInfo& alloc_info);
        Image(vma::Allocator allocator,                    //
              const vk::ImageCreateInfo& image_info,       //
              const vma::AllocationCreateInfo& alloc_info, //
              vk::ImageViewCreateInfo view_info);
        Image(Image&& image);
        ~Image();

        void create_image_view(vk::ImageViewCreateInfo view_info);
        void destroy_image_view();
    };

    class Buffer : public vk::Buffer,       //
                   public vk::BufferView,   //
                   public vk::DeviceMemory, //
                   public vma::Allocation,  //
                   public vma::Allocator
    {
      private:
        void* mapping_ = nullptr;
        void set_zero();

      public:
        const vk::DeviceSize size_ = 0;

        Buffer(vma::Allocator allocator,                //
               const vk::BufferCreateInfo& buffer_info, //
               const vma::AllocationCreateInfo& alloc_info);
        Buffer(vma::Allocator allocator,                    //
               const vk::BufferCreateInfo& buffer_info,     //
               const vma::AllocationCreateInfo& alloc_info, //
               vk::BufferViewCreateInfo view_info);
        Buffer(Buffer&& buffer);
        ~Buffer();

        void create_buffer_view(vk::BufferViewCreateInfo view_info);
        void destroy_buffer_view();

        void* map_memory();
        void unmap_memory();
        void* mapping() const { return mapping_; }
    };
}; // namespace proj

#endif // INCLUDE_RESOURCES_HPP
