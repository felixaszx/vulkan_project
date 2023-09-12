#ifndef VK_VK_HPP
#define VK_VK_HPP

#include <iostream>
#include <vector>
#include <format>
#include <vulkan/vulkan.h>

namespace rhi
{
    struct Vk
    {
        //
        //
        //
        //
        //
        //
        // Device creation
        //
        //
        //
        //
        //
        //

        struct DeviceConfig
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

        using DeviceConfigType = DeviceConfig;
        using QueueType = VkQueue;

        inline static VkInstance instance = nullptr;
        inline static VkDebugUtilsMessengerEXT messenger = nullptr;
        inline static VkDevice device = nullptr;
        inline static VkQueue graphics_queue = nullptr;
        inline static VkQueue compute_queue = nullptr;
        inline static VkQueue transfer_queue = nullptr;

        static void create_logical_device(const DeviceConfig& features);
        static void config_init_device(const DeviceConfig& config);
        static void destroy_device();
        static QueueType get_graphics_queue() { return graphics_queue; }
        static QueueType get_transfer_queue() { return transfer_queue; }
        static QueueType get_compute_queue() { return compute_queue; }

        //
        //
        //
        //
        //
        //
        // Command Buffers and Command Pool
        //
        //
        //
        //
        //
        //

        using CmdPoolType = VkCommandPool;
        using CmdType = VkCommandBuffer;
    };

}; // namespace rhi

#endif // VK_VK_HPP
