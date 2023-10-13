#include "ext/image.hpp"

namespace proj
{
    namespace ext
    {
        Image ImageCreator::create(vk::Extent3D extent)
        {
            create_info_.extent = extent;
            return Image(*this, create_info_, alloc_info_);
        }

        Image ImageCreator::create(vk::Extent3D extent, const vk::ImageViewCreateInfo& view_info)
        {
            create_info_.extent = extent;
            return Image(*this, create_info_, alloc_info_, view_info);
        }

        ImageSampleCount::ImageSampleCount(vk::SampleCountFlagBits sample)
            : sample_(sample)
        {
        }

        void ImageSampleCount::operator()(vk::ImageCreateInfo& create_info)
        {
            create_info.samples = sample_;
        }

        ColorAtchm::ColorAtchm(vk::ImageType type)
            : type_(type)
        {
        }

        void ColorAtchm::operator()(vk::ImageCreateInfo& create_info)
        {
            create_info.imageType = type_;
            create_info.mipLevels = 1;
            create_info.arrayLayers = 1;
            create_info.format = vk::Format::eR32G32B32A32Sfloat;
            create_info.tiling = vk::ImageTiling::eOptimal;
            create_info.initialLayout = vk::ImageLayout::eUndefined;
            create_info.usage = vk::ImageUsageFlagBits::eColorAttachment;
            create_info.sharingMode = vk::SharingMode::eExclusive;
            create_info.samples = vk::SampleCountFlagBits::e1;
        }

        DepthStencilAtchm::DepthStencilAtchm(vk::ImageType type)
            : type_(type)
        {
        }

        void DepthStencilAtchm::operator()(vk::ImageCreateInfo& create_info)
        {
            create_info.imageType = type_;
            create_info.mipLevels = 1;
            create_info.arrayLayers = 1;
            create_info.format = vk::Format::eD32SfloatS8Uint;
            create_info.tiling = vk::ImageTiling::eOptimal;
            create_info.initialLayout = vk::ImageLayout::eUndefined;
            create_info.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
            create_info.sharingMode = vk::SharingMode::eExclusive;
            create_info.samples = vk::SampleCountFlagBits::e1;
        }

        MipLevelsAndLayers::MipLevelsAndLayers(uint32_t levels, uint32_t layers)
            : levels_(levels),
              layers_(layers)
        {
        }

        void MipLevelsAndLayers::operator()(vk::ImageCreateInfo& create_info)
        {
            create_info.mipLevels = levels_;
            create_info.arrayLayers = layers_;
        }

        ImageFormat::ImageFormat(vk::Format format)
            : format_(format)
        {
        }

        void ImageFormat::operator()(vk::ImageCreateInfo& create_info)
        {
            create_info.format = format_;
        }
    }; // namespace ext
};     // namespace proj