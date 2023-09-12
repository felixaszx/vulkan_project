#ifndef WINDOW_WINDOW_HPP
#define WINDOW_WINDOW_HPP

#include <string>
#include <vector>
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace proj
{
    class Window
    {
      private:
        GLFWwindow* win = nullptr;

      public:
        Window(uint32_t width, uint32_t height, const std::string& title = "");
        ~Window();

        operator GLFWwindow*() const { return win; }
        std::vector<const char*> required_vk_instance_exts();
        VkSurfaceKHR get_surface(VkInstance instance);
    };
}; // namespace proj

#endif // WINDOW_WINDOW_HPP
