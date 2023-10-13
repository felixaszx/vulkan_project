#include "render.hpp"

namespace proj
{
    RenderLoop::RenderLoop(vk::Device device, uint32_t graphic_queue_index)
        : device_(device)
    {
        for (int i = 0; i < 2; i++)
        {
            vk::FenceCreateInfo fence_info{};
            fence_info.flags = vk::FenceCreateFlagBits::eSignaled;
            frame_fence_[i] = device.createFence(fence_info);

            vk::SemaphoreCreateInfo sem_info{};
            image_sems_[i] = device.createSemaphore(sem_info);
            submit_sems_[i] = device.createSemaphore(sem_info);
        }

        vk::CommandPoolCreateInfo cmd_pool_info{};
        cmd_pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        cmd_pool_info.queueFamilyIndex = graphic_queue_index;
        cmd_pool_ = device.createCommandPool(cmd_pool_info);

        vk::CommandBufferAllocateInfo cmd_alloc_info{};
        cmd_alloc_info.level = vk::CommandBufferLevel::ePrimary;
        cmd_alloc_info.commandPool = cmd_pool_;
        cmd_alloc_info.commandBufferCount = 2;
        main_cmds_ = device.allocateCommandBuffers(cmd_alloc_info);
    }

    RenderLoop::~RenderLoop()
    {
        for (int i = 0; i < 2; i++)
        {
            device_.destroyFence(frame_fence_[i]);
            device_.destroySemaphore(image_sems_[i]);
            device_.destroySemaphore(submit_sems_[i]);
        }

        device_.destroyCommandPool(cmd_pool_);
    }

    vk::Semaphore RenderLoop::get_curr_image_sem()
    {
        return image_sems_[frame_];
    }

    vk::Semaphore RenderLoop::get_curr_submit_sem()
    {
        return submit_sems_[frame_];
    }

    bool RenderLoop::wait_for_last_frame()
    {
        try_vk(auto result = device_.waitForFences(frame_fence_[frame_], true, UINT64_MAX), //
               "",                                                                          //
               return false);
        device_.resetFences(frame_fence_[frame_]);
        main_cmds_[frame_].reset();

        return true;
    }

    vk::CommandBuffer RenderLoop::get_cmd()
    {
        return main_cmds_[frame_];
    }

    void RenderLoop::end_cmd()
    {
        main_cmds_[frame_].end();
    }

    void RenderLoop::submit(vk::Queue graphics_queue, bool double_buffering)
    {
        vk::PipelineStageFlags wait_stages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        vk::SubmitInfo submit_info{};
        submit_info.setWaitDstStageMask(wait_stages);
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = image_sems_ + frame_;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = submit_sems_ + frame_;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &main_cmds_[frame_];
        graphics_queue.submit(submit_info, frame_fence_[frame_]);

        if (double_buffering)
        {
            frame_ = (frame_ + 1) % 2;
        }
    }

    void RenderPassInfoHolder::add_color_atchm(vk::ImageView view, vk::ImageLayout layout, //
                                               bool clear_on_load, vk::ClearColorValue clear_color)
    {
        atchms_.push_back({});
        atchms_.back().imageView = view;
        atchms_.back().imageLayout = layout;
        atchms_.back().loadOp = clear_on_load ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
        atchms_.back().storeOp = vk::AttachmentStoreOp::eStore;
        atchms_.back().clearValue.color = clear_color;
    }

    void RenderPassInfoHolder::set_depth_atchm(vk::ImageView view, vk::ImageLayout layout, bool clear_on_load)
    {
        depth_.imageView = view;
        depth_.imageLayout = layout;
        depth_.loadOp = clear_on_load ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
        depth_.storeOp = vk::AttachmentStoreOp::eStore;
        depth_.clearValue.setDepthStencil({1.0f, 0u});
    }

    void RenderPassInfoHolder::set_stencil_atchm(vk::ImageView view, vk::ImageLayout layout, bool clear_on_load)
    {
        stencil_.imageView = view;
        stencil_.imageLayout = layout;
        stencil_.loadOp = clear_on_load ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
        stencil_.storeOp = vk::AttachmentStoreOp::eStore;
        stencil_.clearValue.setDepthStencil({1.0f, 0u});
    }

    vk::RenderingInfo RenderPassInfoHolder::create_rendering_info(vk::Extent2D extent, uint32_t layers)
    {
        vk::RenderingInfo info{};
        info.renderArea.extent = extent;
        info.setColorAttachments(atchms_);
        info.layerCount = layers;
        info.pDepthAttachment = depth_.imageView ? &depth_ : nullptr;
        info.pStencilAttachment = stencil_.imageView ? &stencil_ : nullptr;
        return info;
    }

    void ShaderModule::set_zero()
    {
        memset(this, 0x0, sizeof(ShaderModule));
    }

    ShaderModule::ShaderModule(vk::Device device, const std::string& file_name, //
                               vk::ShaderStageFlagBits stage, const std::string& entry)
        : entry_(entry),
          vk::Device(device)
    {
        std::ifstream file(file_name, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file\n");
        }
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        vk::ShaderModuleCreateInfo create_info{};
        create_info.pCode = reinterpret_cast<uint32_t*>(buffer.data());
        create_info.codeSize = fileSize;
        static_cast<vk::ShaderModule&>(*this) = this->createShaderModule(create_info);

        static_cast<vk::PipelineShaderStageCreateInfo&>(*this) = vk::PipelineShaderStageCreateInfo{};
        this->pName = entry_.c_str();
        this->module = *this;
        this->stage = stage;
    }

    ShaderModule::ShaderModule(ShaderModule&& shader)
        : vk::Device(shader),
          vk::ShaderModule(shader),
          vk::PipelineShaderStageCreateInfo(shader),
          entry_(shader.entry_)
    {
        shader.set_zero();
        this->pName = entry_.c_str();
    }

    ShaderModule::~ShaderModule()
    {
        if (static_cast<vk::Device>(*this))
        {
            this->destroyShaderModule(*this);
        }
    }

    GraphicsPipelineBuilder::GraphicsPipelineBuilder()
    {
        // view port default
        viewport_state_.viewportCount = 1;
        viewport_state_.scissorCount = 1;
        // rasterizer default
        rasterizer_state_.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer_state_.polygonMode = vk::PolygonMode::eFill;
        rasterizer_state_.lineWidth = 1.0f;
        // depth default
        depth_state_.depthTestEnable = true;
        depth_state_.depthWriteEnable = true;
        depth_state_.depthCompareOp = vk::CompareOp::eLessOrEqual;
        // input asm default
        input_assembly_.topology = vk::PrimitiveTopology::eTriangleList;
        // sampling default
        multisample_state_.rasterizationSamples = vk::SampleCountFlagBits::e1;
    }

    vk::Pipeline GraphicsPipelineBuilder::create_graphics_pipeline(vk::Device device, vk::PipelineLayout layout)
    {
        int stages_count = geom_ ? 3 : 2;
        vk::PipelineShaderStageCreateInfo stages[3]{};
        stages[0] = *vert_;
        stages[1] = *frag_;
        if (stages_count == 3)
        {
            stages[2] = *geom_;
        }

        vk::GraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.pStages = stages;
        pipeline_info.stageCount = stages_count;
        pipeline_info.pVertexInputState = &input_state_;
        pipeline_info.pInputAssemblyState = &input_assembly_;
        pipeline_info.pViewportState = &viewport_state_;
        pipeline_info.pRasterizationState = &rasterizer_state_;
        pipeline_info.pMultisampleState = &multisample_state_;
        pipeline_info.pColorBlendState = &blend_state_;
        pipeline_info.pDynamicState = &dynamic_states_;
        pipeline_info.pDepthStencilState = &depth_state_;
        pipeline_info.layout = layout;
        pipeline_info.pNext = &rendering_info_;

        try_vk(return device.createGraphicsPipelines({}, {pipeline_info}).value[0], //
                      "",                                                           //
                      return nullptr);
    }
} // namespace proj