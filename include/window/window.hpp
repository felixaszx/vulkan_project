#ifndef WINDOW_WINDOW_HPP
#define WINDOW_WINDOW_HPP

#include <string>
#include <vector>
#include <memory>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

namespace proj
{
    class Window
    {
      private:
        inline static SDL_Window* win_ = nullptr;

      public:
        Window(uint32_t width, uint32_t height, const std::string& title = "");
        ~Window();

        vk::SurfaceKHR create_surface(vk::Instance instance);

        static std::vector<const char*> get_instance_exts();
        static inline SDL_Window* handle() { return Window::win_; }
    };
}; // namespace proj

#endif // WINDOW_WINDOW_HPP
