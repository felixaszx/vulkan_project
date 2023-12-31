#define STB_IMAGE_IMPLEMENTATION
#include "render/material.hpp"

namespace proj
{
    namespace render
    {
        void create_image_from_pixels(vma::Allocator allocator,                              //
                                      vk::Queue queue, vk::CommandBuffer cmd,                //
                                      ext::ImageCreator& creator, Material& mat,             //
                                      uint32_t mip_levels, std::unique_ptr<Buffer>& staging, //
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
            mat.textures_.push_back(Image(creator.create(vk::Extent3D(size.width, size.height, 1), view_info)));

            BufferCreator staging_creator(allocator, host_seq, trans_src);
            staging.reset(new Buffer(staging_creator.create(size.width * size.height * size.depth)));
            memcpy(staging->map_memory(), pixels, staging->size_);

            // copy and layout transition infos
            vk::ImageMemoryBarrier barrier{};
            barrier.image = mat.textures_.back();
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
            cmd.copyBufferToImage(*staging, mat.textures_.back(), vk::ImageLayout::eTransferDstOptimal, region);
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

        Material::load_general::load_general(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size)
            : sampler_(sampler),
              pixels_(pixels),
              size_(size)
        {
        }

        void Material::load_general::operator()(vma::Allocator allocator,               //
                                               vk::Queue queue, vk::CommandBuffer cmd, //
                                               Material& a)
        {
            using namespace ext;
            a.image_creator_.update_flags(levels_and_layers(1));

            std::unique_ptr<Buffer> staging = nullptr;
            create_image_from_pixels(allocator, queue, cmd, a.image_creator_, a, 1, staging, pixels_, size_);
            layout_to_shader(queue, cmd, a.textures_.back(), 1);
            a.infos_.push_back(
                vk::DescriptorImageInfo(sampler_, a.textures_.back(), vk::ImageLayout::eShaderReadOnlyOptimal));
        }

        Material::load_mipmapped::load_mipmapped(vk::Sampler sampler, unsigned char* pixels, vk::Extent3D size)
            : sampler_(sampler),
              pixels_(pixels),
              size_(size)
        {
        }

        void Material::load_mipmapped::operator()(vma::Allocator allocator,               //
                                               vk::Queue queue, vk::CommandBuffer cmd, //
                                               Material& a)
        {
            using namespace ext;
            uint32_t mip_levels = std::floor(std::log2(std::max(size_.width, size_.height))) + 1;
            a.image_creator_.update_flags(levels_and_layers(mip_levels));

            std::unique_ptr<Buffer> staging = nullptr;
            create_image_from_pixels(allocator, queue, cmd, a.image_creator_, a, mip_levels, staging, pixels_, size_);

            if (mip_levels > 1)
            {
                generate_mipmaps(cmd, a.textures_.back(), {size_.width, size_.height}, mip_levels);
            }

            layout_to_shader(queue, cmd, a.textures_.back(), mip_levels);
            a.infos_.push_back(
                vk::DescriptorImageInfo(sampler_, a.textures_.back(), vk::ImageLayout::eShaderReadOnlyOptimal));
        }
    }; // namespace render
};     // namespace proj