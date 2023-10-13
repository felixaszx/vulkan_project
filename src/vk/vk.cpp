#define VMA_IMPLEMENTATION
#include "vk/vk.hpp"

#include <iostream>
#include <format>

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
        allocator_.destroy();
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
                detail.queue_index_.graphics_ = index;
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
                detail.queue_index_.compute_ = index;
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
                detail.queue_index_.transfer_ = index;
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
        device_ext_names_.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        if (debug)
        {
            device_layer_names_.push_back("VK_LAYER_KHRONOS_validation");
        }

        vk::PhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature{};
        dynamic_rendering_feature.dynamicRendering = true;

        vk::DeviceCreateInfo device_create_info{};
        device_create_info.pNext = &dynamic_rendering_feature;
        device_create_info.pEnabledFeatures = &feature_;
        device_create_info.setQueueCreateInfos(queue_create_infos);
        device_create_info.setPEnabledExtensionNames(device_ext_names_);
        device_create_info.setPEnabledLayerNames(device_layer_names_);

        try_vk(detail.device_ = detail.physical_.createDevice(device_create_info), "device is not created");
        try_vk(detail.queue_.graphics_ = detail.device_.getQueue(queue_indices[0], 0), "do not get graphic queue");
        try_vk(detail.queue_.compute_ = detail.device_.getQueue(queue_indices[1], 0), "do not get compute queue");
        try_vk(detail.queue_.transfer_ = detail.device_.getQueue(queue_indices[2], 0), "do not get transfer queue");

        vma::VulkanFunctions vma_function{};
        vma_function.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vma_function.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
        vma_function.vkGetDeviceBufferMemoryRequirements = &vkGetDeviceBufferMemoryRequirements;
        vma_function.vkGetDeviceImageMemoryRequirements = &vkGetDeviceImageMemoryRequirements;

        vma::AllocatorCreateInfo vma_create_info{};
        vma_create_info.vulkanApiVersion = VK_API_VERSION_1_3;
        vma_create_info.pVulkanFunctions = &vma_function;
        vma_create_info.instance = instance;
        vma_create_info.physicalDevice = detail.physical_;
        vma_create_info.device = detail.device_;

        try_vk(detail.allocator_ = vma::createAllocator(vma_create_info), "Do not create allocator");

        return detail;
    }

    Swapchain::Swapchain(vk::Instance instance, DeviceDetail& device_detail, vk::SurfaceKHR surface,
                         vk::Extent2D extent)
        : device_(device_detail.device_)
    {
        std::vector<vk::SurfaceFormatKHR> surface_formats = device_detail.physical_.getSurfaceFormatsKHR(surface);
        std::vector<vk::PresentModeKHR> present_modes = device_detail.physical_.getSurfacePresentModesKHR(surface);
        vk::SurfaceCapabilitiesKHR capabilities = device_detail.physical_.getSurfaceCapabilitiesKHR(surface);

        vk::SurfaceFormatKHR selected_format = surface_formats[0];
        vk::PresentModeKHR selected_present_mode = present_modes[0];

        for (vk::SurfaceFormatKHR& format : surface_formats)
        {
            if (format.format == vk::Format::eR8G8B8A8Srgb && //
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                selected_format = format;
                break;
            }
        }

        for (vk::PresentModeKHR& present_mode : present_modes)
        {
            if (present_mode == vk::PresentModeKHR::eMailbox)
            {
                selected_present_mode = present_mode;
                break;
            }
        }

        uint32_t image_count = 2;

        vk::SwapchainCreateInfoKHR swapchain_create_info{};
        swapchain_create_info.surface = surface;
        swapchain_create_info.minImageCount = image_count;
        swapchain_create_info.imageFormat = selected_format.format;
        swapchain_create_info.imageColorSpace = selected_format.colorSpace;
        swapchain_create_info.imageExtent = extent;
        swapchain_create_info.imageArrayLayers = 1;
        swapchain_create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        swapchain_create_info.preTransform = capabilities.currentTransform;
        swapchain_create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapchain_create_info.presentMode = selected_present_mode;
        swapchain_create_info.clipped = VK_TRUE;
        swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

        static_cast<vk::SwapchainKHR&>(*this) = device_.createSwapchainKHR(swapchain_create_info);
        images_ = device_.getSwapchainImagesKHR(*this);
        images_.reserve(image_count);
        image_format_ = selected_format.format;

        views_.resize(images_.size());
        for (int i = 0; i < images_.size(); i++)
        {
            vk::ImageViewCreateInfo create_info{};
            create_info.image = images_[i];
            create_info.viewType = vk::ImageViewType::e2D;
            create_info.format = image_format_;
            create_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;
            views_[i] = device_.createImageView(create_info);
        }
    }

    void Swapchain::destory()
    {
        if (views_[0])
        {
            destroy_image_views();
        }

        device_.destroySwapchainKHR(*this);
    }

    void Swapchain::destroy_image_views()
    {
        for (auto& view : views_)
        {
            device_.destroyImageView(view);
            view = nullptr;
        }
    }

    void Swapchain::layout_transition(vk::CommandBuffer cmd, vk::Queue graphics, //
                                      vk::ImageLayout target_layout)
    {
        vk::CommandBufferBeginInfo begin{};
        begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        cmd.begin(begin);
        for (auto image : images_)
        {
            vk::ImageMemoryBarrier barrier{};
            barrier.image = image;
            barrier.newLayout = target_layout;
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe, //
                                {}, {}, {}, barrier);
        }
        cmd.end();

        vk::SubmitInfo submit{};
        submit.setCommandBuffers(cmd);
        graphics.submit(submit);
        graphics.waitIdle();
        cmd.reset();
    }
}; // namespace proj