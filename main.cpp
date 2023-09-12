#include <iostream>
#include <fstream>
#include <string>

#include "rhi/device.hpp"
#include "rhi/vk/vk.hpp"

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
    std::cerr << std::format("[Vulkan Validation Layer: {}] {}\n\n", type(), pCallbackData->pMessage);

    return VK_FALSE;
}

int main(int argc, char** argv)
{
    rhi::Vk::DeviceConfig config;
    config.debug_cb = debug_cb;
    rhi::Device<rhi::Vk> device(config);
    auto c = device.get_compute_queue();
    return EXIT_SUCCESS;
}