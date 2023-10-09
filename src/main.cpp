#include "window/window.hpp"
#include "vk/vk.hpp"
#include "render.hpp"
#include "resources.hpp"
#include "mesh.hpp"

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

    vk::CommandPoolCreateInfo pool_info{};
    pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    vk::CommandPool pool = device.createCommandPool(pool_info);

    vk::CommandBufferAllocateInfo cmd_info{};
    cmd_info.level = vk::CommandBufferLevel::ePrimary;
    cmd_info.commandBufferCount = 1;
    cmd_info.commandPool = pool;
    vk::CommandBuffer cmd = device.allocateCommandBuffers(cmd_info)[0];

    std::vector<glm::vec3> test_vec3_arr(10);
    MeshDataHolder mesh_test(allocator, device_detail.queue_.graphics_, cmd, //
                             test_vec3_arr, test_vec3_arr, test_vec3_arr, test_vec3_arr);

    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                {
                    running = false;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    KEY::set_state(e.key.keysym.scancode, KEY::STATE_DOWN);
                    break;
                }
                case SDL_KEYUP:
                {
                    KEY::set_state(e.key.keysym.scancode, KEY::STATE_UP);
                    break;
                }
                default:
                {
                }
            }

            if (KEY::advance_state(KEY::ESCAPE) == KEY::STATE_QUICK_UP)
            {
                running = false;
            }
        }
    }

    device.destroyCommandPool(pool);

    swapchian.destroy_image_views();
    swapchian.destory();
    c.instance().destroySurfaceKHR(surface);

    return 0;
}