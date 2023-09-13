#ifndef WINDOW_WINDOW_HPP
#define WINDOW_WINDOW_HPP

#include <iostream>
#include <string>
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>

namespace proj
{
    class Window
    {
      private:
        inline static SDL_Window* win_ = nullptr;

      public:
        Window(uint32_t width, uint32_t height, const std::string& title = "");
        ~Window();

        static std::vector<const char*> get_instance_exts();
        static inline SDL_Window*       handle() { return Window::win_; }
    };
}; // namespace proj

#endif // WINDOW_WINDOW_HPP
