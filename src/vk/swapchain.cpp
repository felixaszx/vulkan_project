#include "vk/swapchain.hpp"

proj::Swapchain::Swapchain(VkSurfaceKHR surface, uint32_t w, uint32_t h)
{
    uint32_t surface_formats_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device::get_physical(), surface, &surface_formats_count, nullptr);
    std::vector<VkSurfaceFormatKHR> surface_formats(surface_formats_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device::get_physical(), surface, &surface_formats_count,
                                         surface_formats.data());

    uint32_t present_modes_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device::get_physical(), surface, &present_modes_count, nullptr);
    std::vector<VkPresentModeKHR> present_modes(present_modes_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device::get_physical(), surface, &present_modes_count,
                                              present_modes.data());

    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device::get_physical(), surface, &capabilities);

    VkSurfaceFormatKHR selected_format = surface_formats[0];
    VkPresentModeKHR selected_present_mode = VK_PRESENT_MODE_FIFO_KHR;

    for (const VkSurfaceFormatKHR& format : surface_formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && //
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            selected_format = format;
            break;
        }
    }

    for (const VkPresentModeKHR& present_mode : present_modes)
    {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            selected_present_mode = present_mode;
            break;
        }
    }

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        extent = capabilities.currentExtent;
    }
    else
    {
        int width, height;
        extent.width = w;
        extent.height = h;
        extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, //
                                   capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }
    uint32_t image_count = (capabilities.minImageCount + 1 > capabilities.maxImageCount) //
                               ? capabilities.maxImageCount                              //
                               : capabilities.minImageCount + 1;

    VkSwapchainCreateInfoKHR swapchain_create_info{};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = selected_format.format;
    swapchain_create_info.imageColorSpace = selected_format.colorSpace;
    swapchain_create_info.imageExtent = extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.preTransform = capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = selected_present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    vkCreateSwapchainKHR(Device::get(), &swapchain_create_info, nullptr, &swapchain);
    image_format = selected_format.format;
    images.resize(image_count);
    vkGetSwapchainImagesKHR(Device::get(), swapchain, &image_count, images.data());

    image_views.resize(images.size());

    for (uint32_t i = 0; i < image_views.size(); i++)
    {
        VkImageViewCreateInfo imageview_create_info{};
        imageview_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageview_create_info.image = images[i];
        imageview_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageview_create_info.format = image_format;

        imageview_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageview_create_info.subresourceRange.baseMipLevel = 0;
        imageview_create_info.subresourceRange.levelCount = 1;
        imageview_create_info.subresourceRange.baseArrayLayer = 0;
        imageview_create_info.subresourceRange.layerCount = 1;

        vkCreateImageView(Device::get(), &imageview_create_info, nullptr, &image_views[i]);
    }
}

proj::Swapchain::~Swapchain()
{
    vkDeviceWaitIdle(Device::get());

    for (VkImageView& image_view : image_views)
    {
        vkDestroyImageView(Device::get(), image_view, nullptr);
    }

    vkDestroySwapchainKHR(Device::get(), swapchain, nullptr);
}
