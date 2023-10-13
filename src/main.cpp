#include "window/window.hpp"
#include "vk/vk.hpp"
#include "render.hpp"
#include "resources.hpp"
#include "assimps.hpp"
#include "pipeline_layout.hpp"
#include "ext/buffer.hpp"
#include "ext/image.hpp"
#include "render/mesh.hpp"
#include "render/material.hpp"

int main(int argc, char* argv[])
{
    stbi_set_flip_vertically_on_load(true);
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
    pool_info.queueFamilyIndex = device_detail.queue_index_.graphics_;
    vk::CommandPool pool = device.createCommandPool(pool_info);

    vk::CommandBufferAllocateInfo cmd_info{};
    cmd_info.level = vk::CommandBufferLevel::ePrimary;
    cmd_info.commandBufferCount = 1;
    cmd_info.commandPool = pool;
    vk::CommandBuffer cmd = device.allocateCommandBuffers(cmd_info)[0];

    Swapchain swapchian(c.instance(), device_detail, surface, {1024, 768});
    swapchian.layout_transition(cmd, device_detail.queue_.graphics_, vk::ImageLayout::ePresentSrcKHR);

    int x, y, chan;
    auto pixels = stbi_load("res/textures/ayaka.png", &x, &y, &chan, STBI_rgb_alpha);

    vk::SamplerCreateInfo sampler_cinfo{};
    sampler_cinfo.anisotropyEnable = true;
    sampler_cinfo.maxAnisotropy = 4;
    sampler_cinfo.borderColor = vk::BorderColor::eFloatOpaqueBlack;
    sampler_cinfo.maxLod = 1000.0f;
    sampler_cinfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    sampler_cinfo.magFilter = vk::Filter::eLinear;
    sampler_cinfo.minFilter = vk::Filter::eLinear;
    vk::Sampler sampler = device.createSampler(sampler_cinfo);

    render::Material mat(allocator, device_detail.queue_.graphics_, cmd, //
                         render::Material::LoadEmissive(sampler, pixels, {uint32_t(x), uint32_t(y), uint32_t(chan)}));

    RenderLoop loop(device, device_detail.queue_index_.graphics_);
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

            loop.wait_for_last_frame();
            uint32_t i = device.acquireNextImageKHR(swapchian, UINT64_MAX, loop.get_curr_image_sem()).value;

            vk::CommandBufferBeginInfo begin{};
            loop.get_cmd().begin(begin);
            loop.end_cmd();
            loop.submit(device_detail.queue_.graphics_);

            uint32_t a[] = {i};
            vk::Semaphore wait_sems[] = {loop.get_curr_submit_sem()};
            vk::PresentInfoKHR present_info{};
            present_info.setWaitSemaphores(wait_sems);
            present_info.pSwapchains = &swapchian;
            present_info.swapchainCount = 1;
            present_info.setImageIndices(a);
            auto k = device_detail.queue_.graphics_.presentKHR(present_info);
        }
    }

    device.waitIdle();
    device.destroyCommandPool(pool);

    swapchian.destroy_image_views();
    swapchian.destory();
    c.instance().destroySurfaceKHR(surface);

    return 0;
}