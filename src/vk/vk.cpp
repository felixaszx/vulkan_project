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

    Contex::Contex(std::vector<const char*>& instance_exts, bool enable_debug)
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

        try_vk(instance_ = vk::createInstance(instance_create_info), "instance is not created");

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

    Contex::~Contex()
    {
        if (message_)
        {
            auto load_func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                instance_, "vkDestroyDebugUtilsMessengerEXT");
            load_func(instance_, message_, nullptr);
        }
        instance_.destroy();
    }

    vk::Instance Contex::instance() const
    {
        return instance_;
    }

    Contex::operator vk::Instance() const
    {
        return instance_;
    }

    DeviceDetail::operator vk::Device() const
    {
        return device_;
    }

    DeviceDetail::~DeviceDetail()
    {
        device_.destroy();
    }

    DeviceDetail DeviceCreator::create_device(vk::Instance instance)
    {
        DeviceDetail detail;
        std::vector<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();

        detail.physical_ = physical_devices[device_index];
        std::vector<vk::QueueFamilyProperties> queue_properties = detail.physical_.getQueueFamilyProperties();

        uint32_t queue_indices[3] = {};
        uint32_t queue_count = 0;

        for (uint32_t index = 0; index < queue_properties.size(); index++)
        {
            if (queue_properties[index].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                queue_count++;
                queue_indices[0] = index;
                break;
            }
        }
        queue_indices[1] = queue_indices[0];
        queue_indices[2] = queue_indices[0];

        for (uint32_t index = 0; index < queue_properties.size(); index++)
        {
            if (queue_properties[index].queueFlags & vk::QueueFlagBits::eCompute &&  //
                queue_properties[index].queueFlags & vk::QueueFlagBits::eTransfer && //
                index != queue_indices[0])
            {
                queue_count++;
                queue_indices[1] = index;
                break;
            }
        }

        for (uint32_t index = 0; index < queue_properties.size(); index++)
        {
            if (queue_properties[index].queueFlags & vk::QueueFlagBits::eTransfer && //
                index != queue_indices[0] &&                                         //
                index != queue_indices[1])
            {
                queue_count++;
                queue_indices[2] = index;
                break;
            }
        }

        vk::PhysicalDeviceProperties properties = detail.physical_.getProperties();
        std::cout << std::format("Vulkan Physical Device: {} is selected\n\n", properties.deviceName.data());

        if (!queue_count)
        {
            std::cerr << std::format("This device do not support any queue that the engine required.\n\n");
        }

        float queue_priority = 1.0f;
        std::vector<vk::DeviceQueueCreateInfo> queue_create_infos{};
        for (int i = 0; i < queue_count; i++)
        {
            if (i > 0 && queue_indices[i] == queue_indices[i - 1])
            {
                break;
            }
            
            vk::DeviceQueueCreateInfo queue_create_info{};
            queue_create_info.queueFamilyIndex = queue_indices[i];
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }

        feature_.samplerAnisotropy = true;
        feature_.fillModeNonSolid = true;
        feature_.geometryShader = true;

        device_ext_names_.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        if (debug)
        {
            device_layer_names_.push_back("VK_LAYER_KHRONOS_validation");
        }

        vk::DeviceCreateInfo device_create_info{};
        device_create_info.pEnabledFeatures = &feature_;
        device_create_info.setQueueCreateInfos(queue_create_infos);
        device_create_info.setPEnabledExtensionNames(device_ext_names_);
        device_create_info.setPEnabledLayerNames(device_layer_names_);

        try_vk(detail.device_ = detail.physical_.createDevice(device_create_info), "device is not created");
        try_vk(detail.queue_.graphics_ = detail.device_.getQueue(queue_indices[0], 0), "do not get graphic queue");
        try_vk(detail.queue_.compute_ = detail.device_.getQueue(queue_indices[1], 0), "do not get compute queue");
        try_vk(detail.queue_.transfer_ = detail.device_.getQueue(queue_indices[2], 0), "do not get transfer queue");

        return detail;
    }
}; // namespace proj