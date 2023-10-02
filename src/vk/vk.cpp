#include "vk/vk.hpp"

namespace proj
{
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

    VkContex::VkContex(std::vector<const char*>& instance_exts, bool enable_debug)
    {
        if (enable_debug)
        {
            instance_exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        const std::vector<const char*> VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};

        vk::ApplicationInfo app_info{};
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_3;

        vk::DebugUtilsMessengerCreateInfoEXT debug_utils_creat_info{};
        debug_utils_creat_info.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | //
                                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        debug_utils_creat_info.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | //
                                             vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |    //
                                             vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        debug_utils_creat_info.pfnUserCallback = debug_cb;

        vk::InstanceCreateInfo instance_create_info{};
        instance_create_info.pApplicationInfo = &app_info;
        instance_create_info.setPEnabledExtensionNames(instance_exts);

        if (enable_debug)
        {
            instance_create_info.setPEnabledLayerNames(VALIDATION_LAYERS);
            instance_create_info.pNext = &debug_utils_creat_info;
        }

        try
        {
            instance_ = vk::createInstance(instance_create_info);
        }
        catch (const std::exception& e)
        {
            std::cerr << std::format("[Vulkan exception, instance is not created] {}\n\n", e.what());
        }

        if (enable_debug)
        {
            auto load_func =
                (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT");
            if (load_func(instance_, (VkDebugUtilsMessengerCreateInfoEXT*)&debug_utils_creat_info, nullptr,
                          (VkDebugUtilsMessengerEXT*)&message_) != VK_SUCCESS)
            {

                throw std::runtime_error("Do not create validation layers\n");
            }
        }
    }

    VkContex::~VkContex()
    {
        if (message_)
        {
            auto load_func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                instance_, "vkDestroyDebugUtilsMessengerEXT");
            load_func(instance_, message_, nullptr);
        }
        instance_.destroy();
    }

    vk::Instance VkContex::instance() const
    {
        return instance_;
    }

    VkDeviceDetail::~VkDeviceDetail()
    {
        device_.destroy();
    }
}; // namespace proj