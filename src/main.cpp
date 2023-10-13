#include "window/window.hpp"
#include "vk/vk.hpp"
#include "render.hpp"
#include "resources.hpp"
#include "assimps.hpp"
#include "pipeline_layout.hpp"
#include "ext/buffer.hpp"
#include "ext/image.hpp"
#include "render/mesh.hpp"

int main(int argc, char* argv[])
{
    using namespace proj;
    Window w(1024, 768);
    auto exts = w.get_instance_exts();

    const int a = 1;

    Contex c(exts);

    vk::SurfaceKHR surface = w.create_surface(c);

    DeviceCreator device_c{};
    DeviceDetail device_detail = device_c.create_device(c);
    vk::Device device = device_detail.device_;
    vma::Allocator allocator = device_detail.allocator_;

    vk::CommandPoolCreateInfo pool_info{};
    pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    vk::CommandPool pool = device.createCommandPool(pool_info);

    vk::CommandBufferAllocateInfo cmd_info{};
    cmd_info.level = vk::CommandBufferLevel::ePrimary;
    cmd_info.commandBufferCount = 1;
    cmd_info.commandPool = pool;
    vk::CommandBuffer cmd = device.allocateCommandBuffers(cmd_info)[0];

    Swapchain swapchian(c.instance(), device_detail, surface, {1024, 768});
    swapchian.layout_transition(cmd, device_detail.queue_.graphics_, vk::ImageLayout::ePresentSrcKHR);

    MeshDataLoader mesh_loader("res/model/sponza/sponza.obj");
    render::MeshData mesh_data(allocator, device_detail.queue_.graphics_, cmd, //
                               mesh_loader.positions_,                         //
                               mesh_loader.normals_,                           //
                               mesh_loader.uvs_,                               //
                               mesh_loader.colors_);
    std::vector<render::Mesh> meshes = mesh_data.get_meshes(device_detail.queue_.graphics_, cmd, //
                                                      mesh_loader.meshes_indices_,         //
                                                      mesh_loader.meshes_indices_count_,   //
                                                      mesh_loader.meshes_vert_count_,      //
                                                      10);

    ext::ImageCreator image_creator(device_detail.allocator_,            //
                                    ext::ColorAtchm(vk::ImageType::e2D), //
                                    ext::device_local);
    Image image(image_creator.create({1024, 768, 1}));

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

    device.waitIdle();
    device.destroyCommandPool(pool);

    swapchian.destroy_image_views();
    swapchian.destory();
    c.instance().destroySurfaceKHR(surface);

    return 0;
}