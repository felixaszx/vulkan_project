#define STB_IMAGE_IMPLEMENTATION
#include "render/material.hpp"

namespace proj
{
    namespace render
    {
        void create_image_from_pixels(vma::Allocator allocator,                                  //
                                      vk::Queue queue, vk::CommandBuffer cmd,                    //
                                      ext::ImageCreator& creator, std::unique_ptr<Image>& image, //
                                      uint32_t mip_levels, std::unique_ptr<Buffer>& staging,     //
                                      const unsigned char* pixels, vk::Extent3D size)
        {
            using namespace ext;

            // create the texture image
            vk::ImageViewCreateInfo view_info{};
            view_info.viewType = vk::ImageViewType::e2D;
            view_info.format = creator.create_info_.format;
            view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            view_info.subresourceRange.levelCount = mip_levels;
            view_info.subresourceRange.layerCount = 1;
            image.reset(new Image(creator.create(vk::Extent3D(size.width, size.height, 1), view_info)));

            BufferCreator staging_creator(allocator, host_seq, trans_src);
            staging.reset(new Buffer(staging_creator.create(size.width * size.height * size.depth)));
            memcpy(staging->map_memory(), pixels, staging->size_);

            // copy and layout transition infos
            vk::ImageMemoryBarrier barrier{};
            barrier.image = *image;
            barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            barrier.subresourceRange.levelCount = mip_levels;
            barrier.subresourceRange.layerCount = 1;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            vk::BufferImageCopy region{};
            region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.layerCount = 1;
            region.imageExtent = vk::Extent3D(size.width, size.height, 1);

            cmd.reset();
            vk::CommandBufferBeginInfo begin{};
            begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            cmd.begin(begin);

            cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, //
                                {}, {}, {}, barrier);
            cmd.copyBufferToImage(*staging, *image, vk::ImageLayout::eTransferDstOptimal, region);
        }

        void layout_to_shader(vk::Queue queue, vk::CommandBuffer cmd, vk::Image image, uint32_t mip_levels)
        {
            vk::ImageMemoryBarrier barrier{};
            barrier.image = image;
            barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.layerCount = 1;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
            barrier.subresourceRange.baseMipLevel = mip_levels - 1;
            cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, //
                                {}, {}, {}, barrier);

            cmd.end();
            vk::SubmitInfo submit{};
            submit.setCommandBuffers(cmd);
            queue.submit(submit);
            queue.waitIdle();
            cmd.reset();
        }

        void generate_mipmaps(vk::CommandBuffer cmd, vk::Image image, vk::Extent2D size, uint32_t level_count)
        {
            int32_t mip_w = size.width;
            int32_t mip_h = size.height;
            for (int i = 1; i < level_count; i++)
            {
                vk::ImageMemoryBarrier barrier{};
                barrier.image = image;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
                barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
                barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
                barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
                barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
                barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
                barrier.subresourceRange.levelCount = 1;
                barrier.subresourceRange.layerCount = 1;
                barrier.subresourceRange.baseMipLevel = i - 1;
                cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, //
                                    {}, {}, {}, barrier);
                vk::ImageBlit blit{};
                blit.srcOffsets[1] = vk::Offset3D(mip_w, mip_h, 1);
                blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = 1;
                blit.dstOffsets[1] = vk::Offset3D(mip_w > 1 ? mip_w / 2 : 1, //
                                                  mip_h > 1 ? mip_h / 2 : 1, //
                                                  1);
                blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                blit.dstSubresource.mipLevel = i;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = 1;
                cmd.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, //
                              image, vk::ImageLayout::eTransferDstOptimal, //
                              blit, vk::Filter::eLinear);

                barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
                barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
                barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
                cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, //
                                    {}, {}, {}, barrier);

                mip_w > 1 ? mip_w /= 2 : mip_w;
                mip_h > 1 ? mip_h /= 2 : mip_h;
            }
        }

        Material::LoadAlbedo::LoadAlbedo(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size)
            : sampler_(sampler),
              pixels_(pixels),
              size_(size)
        {
        }

        void Material::LoadAlbedo::operator()(vma::Allocator allocator,               //
                                              vk::Queue queue, vk::CommandBuffer cmd, //
                                              Material& a)
        {
            using namespace ext;
            uint32_t mip_levels = std::floor(std::log2(std::max(size_.width, size_.height))) + 1;
            a.image_creator_.update_flags(MipLevelsAndLayers(mip_levels));

            std::unique_ptr<Buffer> staging = nullptr;
            create_image_from_pixels(allocator, queue, cmd, a.image_creator_, a.albedo_, mip_levels, staging, pixels_,
                                     size_);

            if (mip_levels > 1)
            {
                generate_mipmaps(cmd, *a.albedo_, {size_.width, size_.height}, mip_levels);
            }

            layout_to_shader(queue, cmd, *a.albedo_, mip_levels);
            a.infos_[0].sampler = sampler_;
            a.infos_[0].imageView = *a.albedo_;
        }

        Material::LoadSpecular::LoadSpecular(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size)
            : sampler_(sampler),
              pixels_(pixels),
              size_(size)
        {
        }

        void Material::LoadSpecular::operator()(vma::Allocator allocator,               //
                                                vk::Queue queue, vk::CommandBuffer cmd, //
                                                Material& a)
        {
            using namespace ext;
            uint32_t mip_levels = std::floor(std::log2(std::max(size_.width, size_.height))) + 1;
            a.image_creator_.update_flags(MipLevelsAndLayers(mip_levels));

            std::unique_ptr<Buffer> staging = nullptr;
            create_image_from_pixels(allocator, queue, cmd, a.image_creator_, a.specular_, mip_levels, staging, pixels_,
                                     size_);

            if (mip_levels > 1)
            {
                generate_mipmaps(cmd, *a.specular_, {size_.width, size_.height}, mip_levels);
            }

            layout_to_shader(queue, cmd, *a.specular_, mip_levels);
            a.infos_[1].sampler = sampler_;
            a.infos_[1].imageView = *a.specular_;
        }

        Material::LoadAmbient::LoadAmbient(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size)
            : sampler_(sampler),
              pixels_(pixels),
              size_(size)
        {
        }

        void Material::LoadAmbient::operator()(vma::Allocator allocator,               //
                                               vk::Queue queue, vk::CommandBuffer cmd, //
                                               Material& a)
        {
            using namespace ext;
            uint32_t mip_levels = std::floor(std::log2(std::max(size_.width, size_.height))) + 1;
            a.image_creator_.update_flags(MipLevelsAndLayers(mip_levels));

            std::unique_ptr<Buffer> staging = nullptr;
            create_image_from_pixels(allocator, queue, cmd, a.image_creator_, a.ambient_, mip_levels, staging, pixels_,
                                     size_);

            if (mip_levels > 1)
            {
                generate_mipmaps(cmd, *a.ambient_, {size_.width, size_.height}, mip_levels);
            }

            layout_to_shader(queue, cmd, *a.ambient_, mip_levels);
            a.infos_[2].sampler = sampler_;
            a.infos_[2].imageView = *a.ambient_;
        }

        Material::LoadOpacity::LoadOpacity(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size)
            : sampler_(sampler),
              pixels_(pixels),
              size_(size)
        {
        }

        void Material::LoadOpacity::operator()(vma::Allocator allocator,               //
                                               vk::Queue queue, vk::CommandBuffer cmd, //
                                               Material& a)
        {
            using namespace ext;
            a.image_creator_.update_flags(MipLevelsAndLayers(1));

            std::unique_ptr<Buffer> staging = nullptr;
            create_image_from_pixels(allocator, queue, cmd, a.image_creator_, a.opacity_, 1, staging, pixels_, size_);
            layout_to_shader(queue, cmd, *a.opacity_, 1);
            a.infos_[3].sampler = sampler_;
            a.infos_[3].imageView = *a.opacity_;
        }

        Material::LoadNormal::LoadNormal(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size)
            : sampler_(sampler),
              pixels_(pixels),
              size_(size)
        {
        }

        void Material::LoadNormal::operator()(vma::Allocator allocator,               //
                                              vk::Queue queue, vk::CommandBuffer cmd, //
                                              Material& a)
        {
            using namespace ext;
            a.image_creator_.update_flags(MipLevelsAndLayers(1));

            std::unique_ptr<Buffer> staging = nullptr;
            create_image_from_pixels(allocator, queue, cmd, a.image_creator_, a.normal_, 1, staging, pixels_, size_);
            layout_to_shader(queue, cmd, *a.normal_, 1);
            a.infos_[4].sampler = sampler_;
            a.infos_[4].imageView = *a.normal_;
        }

        Material::LoadEmissive::LoadEmissive(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size)
            : sampler_(sampler),
              pixels_(pixels),
              size_(size)
        {
        }

        void Material::LoadEmissive::operator()(vma::Allocator allocator,               //
                                                vk::Queue queue, vk::CommandBuffer cmd, //
                                                Material& a)
        {
            using namespace ext;
            a.image_creator_.update_flags(MipLevelsAndLayers(1));

            std::unique_ptr<Buffer> staging = nullptr;
            create_image_from_pixels(allocator, queue, cmd, a.image_creator_, a.emissive_, 1, staging, pixels_, size_);
            layout_to_shader(queue, cmd, *a.emissive_, 1);
            a.infos_[5].sampler = sampler_;
            a.infos_[5].imageView = *a.emissive_;
        }
    }; // namespace render
};     // namespace proj