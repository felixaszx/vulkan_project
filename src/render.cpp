#include "render.hpp"

namespace proj
{
    void ShaderModule::set_zero()
    {
        static_cast<vk::Device&>(*this) = nullptr;
        static_cast<vk::ShaderModule&>(*this) = nullptr;
        static_cast<vk::PipelineShaderStageCreateInfo&>(*this) = vk::PipelineShaderStageCreateInfo();
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
        this->pName = entry_.c_str();
        shader.set_zero();
    }

    ShaderModule::~ShaderModule()
    {
        if (static_cast<vk::Device>(*this))
        {
            this->destroyShaderModule(*this);
        }
    }

    vk::Fence create_vk_fence(vk::Device device, bool signal)
    {
        vk::FenceCreateInfo create_info{};
        if (signal)
        {
            create_info.flags = vk::FenceCreateFlagBits::eSignaled;
        }
        return device.createFence(create_info);
    }

    vk::Semaphore create_vk_semaphore(vk::Device device)
    {
        vk::SemaphoreCreateInfo create_info{};
        return device.createSemaphore(create_info);
    }
} // namespace proj