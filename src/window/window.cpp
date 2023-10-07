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

    void KEY::set_state(int key, int state)
    {
        if (KEY_MAP_CURR_STATE[key] < STATE_DOWN)
        {
            KEY_MAP_PREV_STATE[key] = KEY_MAP_CURR_STATE[key];
            KEY_MAP_CURR_STATE[key] = state;
        }
        else if (KEY_MAP_CURR_STATE[key] >= STATE_DOWN && state == STATE_DOWN)
        {
            KEY_MAP_PREV_STATE[key] = KEY_MAP_CURR_STATE[key];
            KEY_MAP_CURR_STATE[key] = STATE_HOLD;
        }
        else
        {
            KEY_MAP_PREV_STATE[key] = KEY_MAP_CURR_STATE[key];
            KEY_MAP_CURR_STATE[key] = state;
        }
    }

    int KEY::get_state(int key)
    {
        return KEY_MAP_CURR_STATE[key];
    }

    int KEY::advance_state(int key)
    {
        if (KEY_MAP_PREV_STATE[key] == STATE_DOWN && KEY_MAP_CURR_STATE[key] == STATE_UP)
        {
            return STATE_QUICK_UP;
        }

        if (KEY_MAP_PREV_STATE[key] == STATE_DOWN && KEY_MAP_CURR_STATE[key] == STATE_HOLD)
        {
            return STATE_JUST_HOLD;
        }

        if (KEY_MAP_PREV_STATE[key] == STATE_UP && KEY_MAP_CURR_STATE[key] == STATE_DOWN)
        {
            return STATE_JUST_DOWN;
        }

        if (KEY_MAP_PREV_STATE[key] == STATE_HOLD && KEY_MAP_CURR_STATE[key] == STATE_UP)
        {
            return STATE_SLOW_UP;
        }

        if (KEY_MAP_PREV_STATE[key] == STATE_NONE && KEY_MAP_CURR_STATE[key] == STATE_NONE)
        {
            return STATE_NEVER_PRESS;
        }

        return STATE_NONE;
    }
}; // namespace proj