#include <iostream>
#include <fstream>
#include <string>

#include "vk/device.hpp"
#include "vk/swapchain.hpp"
#include "window/window.hpp"

VkBool32 VKAPI_CALL debug_cb(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    auto type = [messageSeverity]()
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            {
                return "WARNING";
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            {
                return "ERROR";
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            {
                return "INFO";
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            {
                return "VERBOSE";
            }
            default:
            {
                return "UNDEFINE";
            }
        }
    };
    std::cerr << std::format("\n[Vulkan Validation Layer: {}] {}\n\n", type(), pCallbackData->pMessage);

    return VK_FALSE;
}

using namespace proj;

int main(int argc, char** argv)
{
    Window w(1920, 1080);

    Device::Config config;
    config.instance_exts = w.required_vk_instance_exts();
    config.device_exts.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    config.debug_cb = debug_cb;
    Device device(config);

    Surface surface(w.get_surface(device.get_instance()));
    VkQueue c = Device::get_graphics_queue();

    Fence f;
    Semaphore s;

    Swapchain swapschain(surface, 1920, 1080);
    return EXIT_SUCCESS;
}