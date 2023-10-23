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
#include "render/pipelines.hpp"
#include "render/camera.hpp"

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

    vk::CommandPoolCreateInfo pool_info{};
    pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    pool_info.queueFamilyIndex = device_detail.queue_index_.graphics_;
    vk::CommandPool pool = device.createCommandPool(pool_info);

    vk::CommandBufferAllocateInfo cmd_info{};
    cmd_info.level = vk::CommandBufferLevel::ePrimary;
    cmd_info.commandBufferCount = 1;
    cmd_info.commandPool = pool;
    vk::CommandBuffer cmd = device.allocateCommandBuffers(cmd_info)[0];

    Swapchain swapchian(c.instance(), device_detail, surface, {1920, 1080});
    swapchian.layout_transition(cmd, device_detail.queue_.graphics_, vk::ImageLayout::ePresentSrcKHR);

    std::vector<ShaderModule> vert;
    std::vector<ShaderModule> frag;
    for (int i = 0; i < 3; i++)
    {
        vert.push_back(ShaderModule(device, std::format("res/shader/vert{}.spv", i), //
                                    vk::ShaderStageFlagBits::eVertex));
        frag.push_back(ShaderModule(device, std::format("res/shader/frag{}.spv", i), //
                                    vk::ShaderStageFlagBits::eFragment));
    }

    render::DefferedPipelineSingleton deffered_pipeline(device);
    deffered_pipeline.create_framebuffer(allocator, {1920, 1080});

    MeshDataLoader mesh_loader("res/model/cube/cube.obj");
    render::MeshData mesh_data(allocator, device_detail.queue_.graphics_, cmd, //
                               mesh_loader.positions_,                         //
                               mesh_loader.normals_,                           //
                               mesh_loader.uvs_,                               //
                               mesh_loader.colors_);
    std::vector<render::Mesh> meshes = mesh_data.get_meshes(device_detail.queue_.graphics_, cmd, //
                                                            mesh_loader.meshes_indices_,         //
                                                            mesh_loader.meshes_indices_count_,   //
                                                            mesh_loader.meshes_vert_count_);
    vk::Pipeline graphics_pipeline[2]{};
    {
        auto vert_attib = render::MeshData::vertex_attributes();
        auto vert_binding = render::MeshData::vertex_bindings();

        vk::PipelineVertexInputStateCreateInfo input_state{};
        input_state.setVertexAttributeDescriptions(vert_attib);
        input_state.setVertexBindingDescriptions(vert_binding);

        vk::PipelineViewportStateCreateInfo viewport_state{};
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizer_state{};
        rasterizer_state.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer_state.polygonMode = vk::PolygonMode::eFill;
        rasterizer_state.lineWidth = 1.0f;

        vk::PipelineDepthStencilStateCreateInfo depth_state{};
        depth_state.depthTestEnable = true;
        depth_state.depthWriteEnable = true;
        depth_state.depthCompareOp = vk::CompareOp::eLess;

        vk::PipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.topology = vk::PrimitiveTopology::eTriangleList;

        vk::PipelineMultisampleStateCreateInfo multisample_state{};
        multisample_state.rasterizationSamples = vk::SampleCountFlagBits::e1;

        vk::PipelineDynamicStateCreateInfo dynamic_states{};
        vk::DynamicState dss[] = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        dynamic_states.setDynamicStates(dss);

        vk::GraphicsPipelineCreateInfo pipeline_info{};
        vk::PipelineShaderStageCreateInfo stages0[] = {vert[0], frag[0]};
        pipeline_info.setStages(stages0);
        pipeline_info.pVertexInputState = &input_state;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer_state;
        pipeline_info.pMultisampleState = &multisample_state;
        pipeline_info.pColorBlendState = deffered_pipeline.color_blends_;
        pipeline_info.pDynamicState = &dynamic_states;
        pipeline_info.pDepthStencilState = &depth_state;
        pipeline_info.layout = deffered_pipeline.layouts_[0];
        pipeline_info.renderPass = deffered_pipeline.render_pass_;
        pipeline_info.subpass = 0;
        graphics_pipeline[0] = device.createGraphicsPipeline(nullptr, pipeline_info).value;
    }
    {
        vk::PipelineVertexInputStateCreateInfo input_state{};

        vk::PipelineViewportStateCreateInfo viewport_state{};
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizer_state{};
        rasterizer_state.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer_state.polygonMode = vk::PolygonMode::eFill;
        rasterizer_state.lineWidth = 1.0f;

        vk::PipelineDepthStencilStateCreateInfo depth_state{};
        depth_state.depthTestEnable = false;
        depth_state.depthWriteEnable = false;

        vk::PipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.topology = vk::PrimitiveTopology::eTriangleList;

        vk::PipelineMultisampleStateCreateInfo multisample_state{};
        multisample_state.rasterizationSamples = vk::SampleCountFlagBits::e1;

        vk::PipelineDynamicStateCreateInfo dynamic_states{};
        vk::DynamicState dss[] = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        dynamic_states.setDynamicStates(dss);

        vk::GraphicsPipelineCreateInfo pipeline_info{};
        vk::PipelineShaderStageCreateInfo stages[] = {vert[1], frag[1]};
        pipeline_info.setStages(stages);
        pipeline_info.pVertexInputState = &input_state;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer_state;
        pipeline_info.pMultisampleState = &multisample_state;
        pipeline_info.pColorBlendState = deffered_pipeline.color_blends_ + 1;
        pipeline_info.pDynamicState = &dynamic_states;
        pipeline_info.pDepthStencilState = &depth_state;
        pipeline_info.layout = deffered_pipeline.layouts_[1];
        pipeline_info.renderPass = deffered_pipeline.render_pass_;
        pipeline_info.subpass = 1;
        graphics_pipeline[1] = device.createGraphicsPipeline(nullptr, pipeline_info).value;
    }

    DescriptorPool des_pool(device, {deffered_pipeline.des_layouts_[0].get(), deffered_pipeline.des_layouts_[1].get()});
    vk::DescriptorSet des_sets[2] = {des_pool.get_set(0), des_pool.get_set(1)};

    vk::Semaphore image_aquired = create_vk_semaphore(device);
    vk::Semaphore submit_finish = create_vk_semaphore(device);
    vk::Fence frame_fence = create_vk_fence(device, true);

    vk::SamplerCreateInfo sampler_info{};
    sampler_info.anisotropyEnable = true;
    sampler_info.maxAnisotropy = 4;
    sampler_info.borderColor = vk::BorderColor::eFloatOpaqueBlack;
    sampler_info.maxLod = 1000.0f;
    sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
    sampler_info.magFilter = vk::Filter::eLinear;
    sampler_info.minFilter = vk::Filter::eLinear;
    vk::Sampler sampelr = device.createSampler(sampler_info);

    int ww, h, chan;
    unsigned char* pixels = stbi_load("res/textures/elysia.png", &ww, &h, &chan, STBI_rgb_alpha);
    render::Material mat(allocator, device_detail.queue_.graphics_, cmd,
                         render::Material::load_mipmapped(sampelr, pixels, vk::Extent3D(ww, h, chan)));

    render::Camera camera(allocator, {1920, 1080});

    vk::WriteDescriptorSet write_set;
    write_set.descriptorCount = 1;
    write_set.descriptorType = vk::DescriptorType::eUniformBuffer;
    write_set.pBufferInfo = &camera;
    des_pool.update_sets(write_set, 0);
    write_set.descriptorCount = 1;
    write_set.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    write_set.dstBinding = 1;
    write_set.pImageInfo = mat.infos_.data();
    des_pool.update_sets(write_set, 0);

    vk::DescriptorImageInfo input_atchm[4]{};
    for (int i = 0; i < 4; i++)
    {
        input_atchm[i].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        input_atchm[i].imageView = deffered_pipeline.images_n_views_[i];
    }
    write_set.descriptorCount = 4;
    write_set.descriptorType = vk::DescriptorType::eInputAttachment;
    write_set.pImageInfo = input_atchm;
    write_set.dstBinding = 0;
    des_pool.update_sets(write_set, 1);

    mesh_data.instance_matrices_[0] =
        glm::translate(glm::mat4(1.0f), camera.get_front() + camera.get_front() + camera.get_front());

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

        camera.update_matrices();
        uint32_t image_index =
            device.acquireNextImageKHR(swapchian, std::numeric_limits<uint64_t>::max(), image_aquired).value;
        try_vk(auto result = device.waitForFences(frame_fence, true, UINT64_MAX));
        device.resetFences(frame_fence);

        vk::ClearValue clear_value[6];
        clear_value[0] = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clear_value[1] = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clear_value[2] = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clear_value[3] = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clear_value[4].setDepthStencil({1.0f, 0});
        clear_value[5] = {{0.0f, 0.0f, 0.0f, 1.0f}};
        vk::RenderPassBeginInfo render_pass_info{};
        render_pass_info.renderPass = deffered_pipeline.render_pass_;
        render_pass_info.framebuffer = deffered_pipeline.framebuffer_;
        render_pass_info.renderArea.extent = vk::Extent2D{1920, 1080};
        render_pass_info.setClearValues(clear_value);

        vk::Viewport viewport{};
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        viewport.width = 1920;
        viewport.height = 1080;
        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D(0, 0);
        scissor.extent = vk::Extent2D{1920, 1080};

        cmd.reset();
        vk::CommandBufferBeginInfo begin_cmd;
        cmd.begin(begin_cmd);
        cmd.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline[0]);
        cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, deffered_pipeline.layouts_[0], 0, des_sets[0], {});
        cmd.setViewport(0, viewport);
        cmd.setScissor(0, scissor);

        mesh_data.bind_data(cmd);
        for (auto& mesh : meshes)
        {
            mesh.draw_instanced(cmd);
        }

        cmd.nextSubpass(vk::SubpassContents::eInline);
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline[1]);
        cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, deffered_pipeline.layouts_[1], 0, des_sets[1], {});
        cmd.setViewport(0, viewport);
        cmd.setScissor(0, scissor);
        cmd.draw(6, 1, 0, 0);
        cmd.endRenderPass();
        cmd.end();

        vk::PipelineStageFlags wait_stages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        vk::SubmitInfo submit_info{};
        submit_info.setWaitDstStageMask(wait_stages);
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &image_aquired;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &submit_finish;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &cmd;
        device_detail.queue_.graphics_.submit(submit_info, frame_fence);

        vk::PresentInfoKHR present{};
        present.swapchainCount = 1;
        present.pSwapchains = &swapchian;
        present.waitSemaphoreCount = 1;
        present.pWaitSemaphores = &submit_finish;
        present.pImageIndices = &image_index;
        auto result = device_detail.queue_.graphics_.presentKHR(present);
    }

    device.waitIdle();
    device.destroySampler(sampelr);
    device.destroySemaphore(image_aquired);
    device.destroySemaphore(submit_finish);
    device.destroyFence(frame_fence);
    device.destroyPipeline(graphics_pipeline[0]);
    device.destroyPipeline(graphics_pipeline[1]);

    device.waitIdle();
    device.destroyCommandPool(pool);

    swapchian.destroy_image_views();
    swapchian.destory();
    c.instance().destroySurfaceKHR(surface);

    return 0;
}