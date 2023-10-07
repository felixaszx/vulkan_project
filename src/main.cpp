#include "window/window.hpp"
#include "vk/vk.hpp"
#include "render.hpp"
#include "resources.hpp"

int main(int argc, char* argv[])
{
    using namespace proj;
    Window w(1920, 1080);
    auto exts = w.get_instance_exts();

    Contex c(exts);

    vk::SurfaceKHR surface = w.create_surface(c);

    DeviceCreator device_c{};
    DeviceDetail device_detail = device_c.create_device(c);
    vk::Device device = device_detail.device_;
    vma::Allocator allocator = device_detail.allocator_;

    Swapchain swapchian(c.instance(), device_detail, surface, {1920, 1080});
    
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

    swapchian.destroy_image_views();
    swapchian.destory();
    c.instance().destroySurfaceKHR(surface);

    return 0;
}