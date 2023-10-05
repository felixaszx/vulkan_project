#include "window/window.hpp"
#include "vk/vk.hpp"
#include "vk/descriptor.hpp"

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
    

    std::vector<vk::DescriptorSetLayoutBinding> bindings[3]{};
    VkDescriptorSetLayoutBinding a;
    bindings[0].push_back({0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex});
    for (uint32_t i = 1; i < 7; i++)
    {
        bindings[0].push_back({i, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment});
    }
    for (uint32_t i = 0; i < 4; i++)
    {
        bindings[1].push_back({i, vk::DescriptorType::eInputAttachment, 1, vk::ShaderStageFlagBits::eFragment});
    }
    bindings[2].push_back({0, vk::DescriptorType::eInputAttachment, 1, vk::ShaderStageFlagBits::eFragment});

    DescriptorLayout* layouts[3]{};
    layouts[0] = new DescriptorLayout(device, bindings[0]);
    layouts[1] = new DescriptorLayout(device, bindings[1]);
    layouts[2] = new DescriptorLayout(device, bindings[2]);
    DescriptorPool pool(device, {layouts[0], layouts[1], layouts[2]});

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

    for (int i = 0; i < 3; i++)
    {
        delete layouts[i];
    }

    c.instance().destroySurfaceKHR(surface);

    return 0;
}