#ifndef INCLUDE_PIPELINE_LAYOUT_HPP
#define INCLUDE_PIPELINE_LAYOUT_HPP

#include <unordered_map>
#include "vk/vk.hpp"

namespace proj
{
    class DescriptorPool;
    class DescriptorLayout : vk::Device, //
                             public vk::DescriptorSetLayout
    {
        friend DescriptorPool;

      private:
        std::unordered_map<vk::DescriptorType, uint32_t> types_count_{};

      public:
        DescriptorLayout(vk::Device device, const std::vector<vk::DescriptorSetLayoutBinding>& bindings, //
                         vk::DescriptorSetLayoutCreateFlagBits flags = {});
        ~DescriptorLayout();
    };

    class DescriptorPool : vk::Device, //
                           public vk::DescriptorPool
    {
      private:
        std::unordered_map<vk::DescriptorType, uint32_t> types_count_{};
        std::vector<vk::DescriptorSet> sets_{};

      public:
        DescriptorPool(vk::Device device, const std::vector<DescriptorLayout*>& layouts);
        ~DescriptorPool();

        vk::DescriptorSet get_set(uint32_t index);
        std::vector<vk::DescriptorSet> get_sets();
        void update_sets(vk::WriteDescriptorSet& write, uint32_t set_index);
    };
}; // namespace proj

#endif // INCLUDE_PIPELINE_LAYOUT_HPP
