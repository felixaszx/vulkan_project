#ifndef VK_SWAPCHAIN_HPP
#define VK_SWAPCHAIN_HPP

#include <algorithm>
#include "device.hpp"

namespace proj
{
    class Swapchain
    {
      private:
        VkSwapchainKHR swapchain = nullptr;

      public:
        mutable std::vector<VkImage> images = {};
        mutable std::vector<VkImageView> image_views = {};
        mutable VkExtent2D extent = {};
        mutable VkFormat image_format = {};

        Swapchain(VkSurfaceKHR surface, uint32_t w, uint32_t h);
        ~Swapchain();

        operator VkSwapchainKHR() const { return swapchain; }
    };
}; // namespace proj

#endif // VK_SWAPCHAIN_HPP
