#include "window/window.hpp"

namespace proj
{

    Window::Window(uint32_t width, uint32_t height, const std::string& title)
    {
        if (win_)
        {
            return;
        }

        SDL_Init(SDL_INIT_VIDEO);
        SDL_Vulkan_LoadLibrary(nullptr);
        win_ = SDL_CreateWindow(title.c_str(),                                    //
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, //
                                width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
    }

    Window::~Window()
    {
        SDL_DestroyWindow(win_);
        SDL_Vulkan_UnloadLibrary();
        SDL_Quit();
    }

    vk::SurfaceKHR Window::create_surface(vk::Instance instance)
    {
        VkSurfaceKHR surface = nullptr;
        SDL_Vulkan_CreateSurface(handle(), instance, &surface);
        return surface;
    }

    std::vector<const char*> Window::get_instance_exts()
    {
        uint32_t ext_count = 0;
        SDL_Vulkan_GetInstanceExtensions(win_, &ext_count, nullptr);
        std::vector<const char*> exts(ext_count);
        SDL_Vulkan_GetInstanceExtensions(win_, &ext_count, exts.data());

        return exts;
    }
}; // namespace proj