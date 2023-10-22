#ifndef INCLUDE_RENDER_HPP
#define INCLUDE_RENDER_HPP

#include <thread>
#include <fstream>
#include "vk/vk.hpp"

namespace proj
{
    class ShaderModule : public vk::Device,       //
                         public vk::ShaderModule, //
                         public vk::PipelineShaderStageCreateInfo
    {
      private:
        const std::string entry_;
        void set_zero();

      public:
        ShaderModule(vk::Device device, const std::string& file_name, //
                     vk::ShaderStageFlagBits stage, const std::string& entry = "main");
        ShaderModule(ShaderModule&& shader);
        ~ShaderModule();
    };

    vk::Fence create_vk_fence(vk::Device device, bool signal = false);
    vk::Semaphore create_vk_semaphore(vk::Device device);
} // namespace proj

#endif // INCLUDE_RENDER_HPP
