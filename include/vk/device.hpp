#ifndef VK_VK_HPP
#define VK_VK_HPP

#include <iostream>
#include <vector>
#include <format>
#include <vulkan/vulkan.h>

namespace proj
{
    class Device
    {
      private:
        inline static VkInstance instance = nullptr;
        inline static VkDebugUtilsMessengerEXT messenger = nullptr;
        inline static VkPhysicalDevice physical = nullptr;
        inline static VkDevice device = nullptr;
        inline static VkQueue graphics_queue = nullptr;
        inline static VkQueue compute_queue = nullptr;
        inline static VkQueue transfer_queue = nullptr;

      public:
        struct Config
        {
            // instance config
            bool validation = true;
            PFN_vkDebugUtilsMessengerCallbackEXT debug_cb = nullptr;
            std::vector<const char*> instance_exts = {};
            std::vector<const char*> instance_layers = {};

            // device config
            std::vector<const char*> device_exts = {};
            std::vector<const char*> device_layers = {};
            VkPhysicalDeviceFeatures device_features = {};
        };

        Device(const Config& config);
        ~Device();

        operator VkDevice() const { return device; }
        void create_logical_device(const Config& features);

        static inline VkDevice get() { return Device::device; }
        static inline VkInstance get_instance() { return Device::instance; }
        static inline VkPhysicalDevice get_physical() { return Device::physical; }
        static inline VkQueue get_graphics_queue() { return Device::graphics_queue; }
        static inline VkQueue get_present_queue() { return Device::graphics_queue; }
        static inline VkQueue get_compute_queue() { return Device::compute_queue; }
        static inline VkQueue get_transfer_queue() { return Device::transfer_queue; }
    };

    class Surface
    {
      private:
        VkSurfaceKHR surface = nullptr;

      public:
        Surface(VkSurfaceKHR new_surface);
        ~Surface();

        operator VkSurfaceKHR() const { return surface; }
    };

    class Semaphore
    {
      private:
        VkSemaphore sem = nullptr;

      public:
        Semaphore(const Semaphore&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;
        Semaphore& operator=(Semaphore&&) = delete;

        Semaphore();
        ~Semaphore();
    };

    class Fence
    {
      private:
        VkFence fence = nullptr;

      public:
        Fence(const Fence&) = delete;
        Fence(Fence&&) = delete;
        Fence& operator=(const Fence&) = delete;
        Fence& operator=(Fence&&) = delete;

        Fence(bool signal = true);
        ~Fence();
    };
}; // namespace proj

#endif // VK_VK_HPP
