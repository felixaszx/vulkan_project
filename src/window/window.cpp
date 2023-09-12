#include "window/window.hpp"

proj::Window::Window(uint32_t width, uint32_t height, const std::string& title)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    win = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

proj::Window::~Window()
{
    glfwDestroyWindow(win);
    glfwTerminate();
}

std::vector<const char*> proj::Window::required_vk_instance_exts()
{
    uint32_t glfwExtCount = 0;
    const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    std::vector<const char*> exts(glfwExts, glfwExts + glfwExtCount);
    return exts;
}

VkSurfaceKHR proj::Window::get_surface(VkInstance instance)
{
    VkSurfaceKHR surface = nullptr;
    std::cout<<glfwCreateWindowSurface(instance, win, nullptr, &surface);

    return surface;
} 
