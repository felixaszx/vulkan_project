#ifndef VK_VK_HPP
#define VK_VK_HPP

#include <iostream>
#include <format>
#include <vulkan/vulkan.hpp>

namespace proj
{
    class VkContex
    {
      private:
        vk::Instance instance_ = nullptr;
        vk::DebugUtilsMessengerEXT message_ = nullptr;

      public:
        VkContex(std::vector<const char*>& instance_exts, bool enable_debug = true);
        ~VkContex();

        vk::Instance instance() const;
    };

    struct VkDeviceDetail
    {
        vk::Device device_ = nullptr;

        struct
        {
            vk::Queue graphics_ = nullptr;
            vk::Queue present_ = nullptr;
            vk::Queue transfer_ = nullptr;
            vk::Queue compute_ = nullptr;
        } queue_;
        
        VkDeviceDetail() = delete;
        ~VkDeviceDetail();
    };

    struct VkDeviceCreator
    {
        bool debug = true;
        uint32_t device_index = 0;
        std::vector<const char*> device_ext_names_{};
        std::vector<const char*> device_layer_names_{};

        VkDeviceDetail create_device();
    };
}; // namespace proj

#endif // VK_VK_HPP
