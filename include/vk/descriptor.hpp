#ifndef VK_DESCRIPTOR_HPP
#define VK_DESCRIPTOR_HPP

#include <unordered_map>
#include "vk.hpp"

namespace proj
{
    class DescriptorPool;
    class DescriptorLayout : public vk::DescriptorSetLayout, //
                             public DeviceObj
    {
        friend DescriptorPool;

      private:
        std::unordered_map<vk::DescriptorType, uint32_t> type_count_{};

      public:
        DescriptorLayout(vk::Device device, const std::vector<vk::DescriptorSetLayoutBinding>& bindings, //
                         vk::DescriptorSetLayoutCreateFlagBits flags = {});
        ~DescriptorLayout();
    };

    class DescriptorPool : public vk::DescriptorPool,             //
                           public std::vector<vk::DescriptorSet>, //
                           public DeviceObj
    {
      private:
        std::unordered_map<vk::DescriptorType, uint32_t> type_count_{};

      public:
        DescriptorPool(vk::Device device, const std::vector<DescriptorLayout*>& layouts);
        ~DescriptorPool();

        void update_sets(vk::WriteDescriptorSet& write, uint32_t set);
    };
}; // namespace proj

#endif // VK_DESCRIPTOR_HPP
