#include "vk/device.hpp"

namespace proj
{
    Device::Device(const Config& config)
    {
        if (instance)
        {
            return;
        }

        VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
        app_info.pApplicationName = "";
        app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
        app_info.pEngineName = "";
        app_info.engineVersion = VK_MAKE_VERSION(0, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_3;

        VkDebugUtilsMessengerCreateInfoEXT debug_utils_creat_info = {
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
        debug_utils_creat_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | //
                                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        debug_utils_creat_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |                //
                                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |            //
                                             VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT | //
                                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debug_utils_creat_info.pfnUserCallback = config.debug_cb;

        VkInstanceCreateInfo instance_info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};

        std::vector<const char*> exts = config.instance_exts;
        std::vector<const char*> layers = config.instance_layers;
        if (config.validation)
        {
            exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            layers.push_back("VK_LAYER_KHRONOS_validation");
            instance_info.pNext = &debug_utils_creat_info;
        }
        instance_info.pApplicationInfo = &app_info;
        instance_info.enabledExtensionCount = exts.size();
        instance_info.ppEnabledExtensionNames = exts.data();
        instance_info.enabledLayerCount = layers.size();
        instance_info.ppEnabledLayerNames = layers.data();

        VkResult result = vkCreateInstance(&instance_info, nullptr, &instance);
        if (result != VK_SUCCESS)
        {
            std::cerr << std::format("Do not create Vulkan Instance with error code: {}\n", (uint32_t)result);
            return;
        }

        if (config.validation)
        {
            PFN_vkCreateDebugUtilsMessengerEXT func =
                (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            VkResult debug_create_result = func(instance, &debug_utils_creat_info, nullptr, &messenger);
            if (debug_create_result != VK_SUCCESS)
            {
                std::cerr << std::format("Do not create Vulkan Debug Utils Messenger with error code: {}\n",
                                         (uint32_t)result);
                return;
            }
        }

        create_logical_device(config);
    }

    Device::~Device()
    {
        vkDestroyDevice(device, nullptr);
        PFN_vkDestroyDebugUtilsMessengerEXT destroy_debug =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        destroy_debug(instance, messenger, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    void Device::create_logical_device(const Config& config)
    {
        uint32_t physical_deviec_count = 0;
        vkEnumeratePhysicalDevices(instance, &physical_deviec_count, nullptr);
        std::vector<VkPhysicalDevice> physical_devices(physical_deviec_count);
        vkEnumeratePhysicalDevices(instance, &physical_deviec_count, physical_devices.data());

        VkPhysicalDevice selected = physical_devices[0];
        for (const VkPhysicalDevice& device : physical_devices)
        {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(device, &properties);
            std::cout << std::format("\nPhysical Device: {}\nApi version: {}", //
                                     properties.deviceName,                    //
                                     properties.apiVersion);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                selected = device;
                physical = selected;
                std::cout << std::format("\nPhysical Device: {} is selected", //
                                         properties.deviceName);
            }
        }

        uint32_t graphics_queue_family = -1;
        uint32_t transfer_queue_family = -1;
        uint32_t compute_queue_family = -1;
        uint32_t queue_properties_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(selected, &queue_properties_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_properties(queue_properties_count);
        vkGetPhysicalDeviceQueueFamilyProperties(selected, &queue_properties_count, queue_properties.data());

        for (uint32_t index = 0; index < queue_properties.size(); index++)
        {
            if (queue_properties[index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphics_queue_family = index;
                compute_queue_family = graphics_queue_family;
                transfer_queue_family = graphics_queue_family;
                break;
            }
        }

        for (uint32_t index = 0; index < queue_properties.size(); index++)
        {
            if (queue_properties[index].queueFlags & (VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT) &&
                index != graphics_queue_family)
            {
                compute_queue_family = index;
                transfer_queue_family = graphics_queue_family;
                break;
            }
        }

        for (uint32_t index = 0; index < queue_properties.size(); index++)
        {
            if (queue_properties[index].queueFlags & (VK_QUEUE_TRANSFER_BIT) && index != graphics_queue_family &&
                index != compute_queue_family)
            {
                transfer_queue_family = index;
                break;
            }
        }

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos{};
        float queue_priority = 1.0f;

        VkDeviceQueueCreateInfo g_queue_create_info{};
        g_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        g_queue_create_info.queueFamilyIndex = graphics_queue_family;
        g_queue_create_info.queueCount = 1;
        g_queue_create_info.pQueuePriorities = &queue_priority;
        VkDeviceQueueCreateInfo c_queue_create_info{};
        c_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        c_queue_create_info.queueFamilyIndex = compute_queue_family;
        c_queue_create_info.queueCount = 1;
        c_queue_create_info.pQueuePriorities = &queue_priority;
        VkDeviceQueueCreateInfo t_queue_create_info{};
        t_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        t_queue_create_info.queueFamilyIndex = transfer_queue_family;
        t_queue_create_info.queueCount = 1;
        t_queue_create_info.pQueuePriorities = &queue_priority;

        queue_create_infos.push_back(g_queue_create_info);
        queue_create_infos.push_back(c_queue_create_info);
        queue_create_infos.push_back(t_queue_create_info);

        std::vector<const char*> exts = config.device_exts;
        std::vector<const char*> layers = config.device_layers;

        if (config.validation)
        {
            layers.push_back("VK_LAYER_KHRONOS_validation");
        }

        VkDeviceCreateInfo device_create_info{};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pEnabledFeatures = &config.device_features;
        device_create_info.queueCreateInfoCount = queue_create_infos.size();
        device_create_info.pQueueCreateInfos = queue_create_infos.data();
        device_create_info.enabledExtensionCount = exts.size();
        device_create_info.ppEnabledExtensionNames = exts.data();
        device_create_info.enabledLayerCount = layers.size();
        device_create_info.ppEnabledLayerNames = layers.data();

        VkResult result = vkCreateDevice(selected, &device_create_info, nullptr, &device);
        if (result != VK_SUCCESS)
        {
            std::cerr << std::format("Do not create Vulkan Device with error code: {}\n", (uint32_t)result);
            return;
        }

        vkGetDeviceQueue(device, graphics_queue_family, 0, &graphics_queue);
        vkGetDeviceQueue(device, compute_queue_family, 0, &compute_queue);
        vkGetDeviceQueue(device, transfer_queue_family, 0, &transfer_queue);
    }

    Surface::Surface(VkSurfaceKHR new_surface)
        : surface(new_surface)
    {
    }

    Surface::~Surface()
    {
        vkDestroySurfaceKHR(Device::get_instance(), surface, nullptr);
    }

    Semaphore::Semaphore()
    {
        VkSemaphoreCreateInfo create_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        vkCreateSemaphore(Device::get(), &create_info, nullptr, &sem);
    }

    Semaphore::~Semaphore()
    {
        vkDestroySemaphore(Device::get(), sem, nullptr);
    }

    Fence::Fence(bool signal)
    {
        VkFenceCreateInfo fence_info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fence_info.flags = signal ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        vkCreateFence(Device::get(), &fence_info, nullptr, &fence);
    }

    Fence::~Fence()
    {
        vkDestroyFence(Device::get(), fence, nullptr);
    }
}; // namespace proj