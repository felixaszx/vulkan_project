#include <window/window.hpp>
#include <vk/vk.hpp>

int main(int argc, char* argv[])
{
    using namespace proj;
    std::unique_ptr w = std::make_unique<Window>(1920, 1080);
    auto exts = w->get_instance_exts();

   std::unique_ptr c  =  std::make_unique<VkContex>(exts);

   uint32_t physicalDeviceCount;
   vkEnumeratePhysicalDevices(c->instance(), &physicalDeviceCount, nullptr);
   std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
   vkEnumeratePhysicalDevices(c->instance(), &physicalDeviceCount, physicalDevices.data());
   VkPhysicalDevice physicalDevice = physicalDevices[0];

   uint32_t queueFamilyCount;
   vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
   std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
   vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

   VkSurfaceKHR surface = w->create_surface(c->instance());

   uint32_t graphicsQueueIndex = UINT32_MAX;
   uint32_t presentQueueIndex = UINT32_MAX;
   VkBool32 support;
   uint32_t i = 0;
   for (VkQueueFamilyProperties queueFamily : queueFamilies)
   {
       if (graphicsQueueIndex == UINT32_MAX && queueFamily.queueCount > 0 &&
           queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
           graphicsQueueIndex = i;
       if (presentQueueIndex == UINT32_MAX)
       {
           vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &support);
           if (support)
               presentQueueIndex = i;
       }
       ++i;
   }

   float queuePriority = 1.0f;
   VkDeviceQueueCreateInfo queueInfo = {
       VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // sType
       nullptr,                                    // pNext
       0,                                          // flags
       graphicsQueueIndex,                         // graphicsQueueIndex
       1,                                          // queueCount
       &queuePriority,                             // pQueuePriorities
   };

   VkPhysicalDeviceFeatures deviceFeatures = {};
   const char* deviceExtensionNames[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
   VkDeviceCreateInfo createInfo = {
       VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, // sType
       nullptr,                              // pNext
       0,                                    // flags
       1,                                    // queueCreateInfoCount
       &queueInfo,                           // pQueueCreateInfos
       0,                                    // enabledLayerCount
       nullptr,                              // ppEnabledLayerNames
       1,                                    // enabledExtensionCount
       deviceExtensionNames,                 // ppEnabledExtensionNames
       &deviceFeatures,                      // pEnabledFeatures
   };
   VkDevice device;
   vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);

   VkQueue graphicsQueue;
   vkGetDeviceQueue(device, graphicsQueueIndex, 0, &graphicsQueue);

   VkQueue presentQueue;
   vkGetDeviceQueue(device, presentQueueIndex, 0, &presentQueue);

   SDL_Log("Initialized with errors: %s", SDL_GetError());

   bool running = true;
   while (running)
   {
       SDL_Event windowEvent;
       while (SDL_PollEvent(&windowEvent))
           if (windowEvent.type == SDL_QUIT)
           {
               running = false;
               break;
           }
   }

   vkDestroyDevice(device, nullptr);
   c->instance().destroySurfaceKHR(surface);

   SDL_Log("Cleaned up with errors: %s", SDL_GetError());

   return 0;
}