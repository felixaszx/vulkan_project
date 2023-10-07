#ifndef VK_VK_HPP
#define VK_VK_HPP

#include <iostream>
#include <format>
#include <vulkan/vulkan.hpp>

#define GET_TRY_VK_FUNC(_0, _1, _2, NAME, ...) NAME
#define try_vk0(statement)                                               \
    try                                                                  \
    {                                                                    \
        statement;                                                       \
    }                                                                    \
    catch (const std::exception& e)                                      \
    {                                                                    \
        std::cerr << std::format("[Vulkan exception] {}\n\n", e.what()); \
    }
#define try_vk1(statement, msg)                                                   \
    try                                                                           \
    {                                                                             \
        statement;                                                                \
    }                                                                             \
    catch (const std::exception& e)                                               \
    {                                                                             \
        std::cerr << std::format("[Vulkan exception, {}] {}\n\n", msg, e.what()); \
    }
#define try_vk2(statement, msg, ret)                                              \
    try                                                                           \
    {                                                                             \
        statement;                                                                \
    }                                                                             \
    catch (const std::exception& e)                                               \
    {                                                                             \
        ret;                                                                      \
    }
#define try_vk(...) GET_TRY_VK_FUNC(__VA_ARGS__, try_vk2, try_vk1, try_vk0)(__VA_ARGS__);

namespace proj
{
    class Contex
    {
      private:
        vk::Instance instance_ = nullptr;
        vk::DebugUtilsMessengerEXT message_ = nullptr;

      public:
        Contex(std::vector<const char*>& instance_exts, bool enable_debug = true);
        ~Contex();

        vk::Instance instance() const;
        operator vk::Instance() const;
    };

    struct DeviceCreator;
    class DeviceDetail
    {
        friend DeviceCreator;

      private:
        DeviceDetail(){};

      public:
        vk::Device device_ = nullptr;
        vk::PhysicalDevice physical_ = nullptr;

        operator vk::Device() const;

        struct
        {
            vk::Queue graphics_ = nullptr;
            vk::Queue transfer_ = nullptr;
            vk::Queue compute_ = nullptr;
        } queue_;

        ~DeviceDetail();
    };

    struct DeviceCreator
    {
        bool debug = true;
        uint32_t device_index = 0;
        vk::PhysicalDeviceFeatures feature_{};
        std::vector<const char*> device_ext_names_{};
        std::vector<const char*> device_layer_names_{};

        DeviceDetail create_device(vk::Instance instance);
    };

    class Swapchain : vk::SwapchainKHR
    {
      private:
        vk::Device device_ = nullptr;

      public:
        std::vector<vk::Image> images_{};
        std::vector<vk::ImageView> views_{};
        vk::Format image_format_ = {};

        Swapchain(vk::Instance instance, DeviceDetail& device_detail, vk::SurfaceKHR surface, vk::Extent2D extent);
        void destory();

        void destroy_image_views();
    };
}; // namespace proj

#endif // VK_VK_HPP
