#include "resources.hpp"

namespace proj
{

    //
    //
    // Image class
    //
    //

    void Image::set_zero()
    {
        memset(this, 0x0, sizeof(Image));
    }

    Image::Image(vma::Allocator allocator,              //
                 const vk::ImageCreateInfo& image_info, //
                 const vma::AllocationCreateInfo& alloc_info)
        : vma::Allocator(allocator)
    {
        vma::AllocationInfo finish_info{};
        auto result = allocator.createImage(image_info, alloc_info, finish_info);
        static_cast<vk::Image&>(*this) = result.first;
        static_cast<vk::DeviceMemory&>(*this) = finish_info.deviceMemory;
        static_cast<vma::Allocation&>(*this) = result.second;
    }

    Image::Image(vma::Allocator allocator,                    //
                 const vk::ImageCreateInfo& image_info,       //
                 const vma::AllocationCreateInfo& alloc_info, //
                 vk::ImageViewCreateInfo view_info)
        : Image(allocator, image_info, alloc_info)
    {
        create_image_view(view_info);
    }

    Image::Image(Image&& image)
        : vk::Image(image),
          vk::ImageView(image),
          vk::DeviceMemory(image),
          vma::Allocator(image),
          vma::Allocation(image)
    {
        image.set_zero();
    }

    Image::~Image()
    {
        if (static_cast<vma::Allocator&>(*this))
        {
            destroy_image_view();
            static_cast<vma::Allocator&>(*this).destroyImage(*this, *this);
        }
    }

    void Image::create_image_view(vk::ImageViewCreateInfo view_info)
    {
        view_info.image = *this;
        static_cast<vk::ImageView&>(*this) = static_cast<vma::Allocator&>(*this)
                                                 .getAllocatorInfo() //
                                                 .device.createImageView(view_info);
    }

    void Image::destroy_image_view()
    {
        if (static_cast<vk::ImageView>(*this))
        {
            static_cast<vma::Allocator&>(*this)
                .getAllocatorInfo() //
                .device.destroyImageView(*this);
            static_cast<vk::ImageView&>(*this) = nullptr;
        }
    }

    //
    //
    // Buffer class
    //
    //

    void Buffer::set_zero()
    {
        memset(this, 0x0, sizeof(Buffer));
    }

    Buffer::Buffer(vma::Allocator allocator,                //
                   const vk::BufferCreateInfo& buffer_info, //
                   const vma::AllocationCreateInfo& alloc_info)
        : vma::Allocator(allocator),
          size_(buffer_info.size)
    {
        vma::AllocationInfo finish_info{};
        auto result = allocator.createBuffer(buffer_info, alloc_info, finish_info);
        static_cast<vk::Buffer&>(*this) = result.first;
        static_cast<vk::DeviceMemory&>(*this) = finish_info.deviceMemory;
        static_cast<vma::Allocation&>(*this) = result.second;
    }

    Buffer::Buffer(vma::Allocator allocator,                    //
                   const vk::BufferCreateInfo& buffer_info,     //
                   const vma::AllocationCreateInfo& alloc_info, //
                   vk::BufferViewCreateInfo view_info)
        : Buffer(allocator, buffer_info, alloc_info)
    {
        create_buffer_view(view_info);
    }

    Buffer::Buffer(Buffer&& buffer)
        : vk::Buffer(buffer),
          vk::BufferView(buffer),
          vk::DeviceMemory(buffer),
          vma::Allocation(buffer),
          vma::Allocator(buffer),
          size_(buffer.size_),
          mapping_(buffer.mapping_)
    {
        buffer.set_zero();
    }

    Buffer::~Buffer()
    {
        if (static_cast<vma::Allocator>(*this))
        {
            unmap_memory();
            destroy_buffer_view();
            static_cast<vma::Allocator&>(*this).destroyBuffer(*this, *this);
        }
    }

    void Buffer::create_buffer_view(vk::BufferViewCreateInfo view_info)
    {
        view_info.buffer = *this;
        static_cast<vk::BufferView&>(*this) = static_cast<vma::Allocator&>(*this)
                                                  .getAllocatorInfo() //
                                                  .device.createBufferView(view_info);
    }

    void Buffer::destroy_buffer_view()
    {
        if (static_cast<vk::BufferView&>(*this))
        {
            static_cast<vma::Allocator&>(*this)
                .getAllocatorInfo() //
                .device.destroyBufferView(*this);
            static_cast<vk::BufferView&>(*this) = nullptr;
        }
    }

    void* Buffer::map_memory()
    {
        if (!mapping_)
        {
            mapping_ = static_cast<vma::Allocator&>(*this).mapMemory(*this);
            return mapping_;
        }
        return mapping_;
    }

    void Buffer::unmap_memory()
    {
        if (mapping_)
        {
            static_cast<vma::Allocator&>(*this).unmapMemory(*this);
            mapping_ = nullptr;
        }
    }
}; // namespace proj