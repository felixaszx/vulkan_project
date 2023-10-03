#include "vk/descriptor.hpp"

namespace proj
{
    DescriptorLayout::DescriptorLayout(vk::Device device,
                                       const std::vector<vk::DescriptorSetLayoutBinding>& bindings, //
                                       vk::DescriptorSetLayoutCreateFlagBits flags)
        : DeviceObj(device)
    {
        for (const vk::DescriptorSetLayoutBinding& binding : bindings)
        {
            auto result = type_count_.find(binding.descriptorType);
            if (result == type_count_.end())
            {
                type_count_.insert({binding.descriptorType, 1});
            }
            else
            {
                result->second++;
            }
        }

        vk::DescriptorSetLayoutCreateInfo create_info{};
        create_info.flags = flags;
        create_info.setBindings(bindings);
        try_vk(static_cast<vk::DescriptorSetLayout&>(*this) = device.createDescriptorSetLayout(create_info));
    }

    DescriptorLayout::~DescriptorLayout()
    {
        device_.destroyDescriptorSetLayout(*this);
    }

    DescriptorPool::DescriptorPool(vk::Device device, const std::vector<DescriptorLayout*>& layouts)
        : DeviceObj(device)
    {
        for (const auto& layout : layouts)
        {
            const std::unordered_map<vk::DescriptorType, uint32_t>& types = layout->type_count_;
            for (const auto& type : types)
            {
                auto tmp_type = type_count_.find(type.first);
                if (tmp_type == type_count_.end())
                {
                    type_count_.insert(type);
                }
                else
                {
                    tmp_type->second += type.second;
                }
            }
        }

        std::vector<vk::DescriptorPoolSize> sizes(type_count_.size());
        auto tmp_type = type_count_.begin();
        for (auto& size : sizes)
        {
            size.type = tmp_type->first;
            size.descriptorCount = tmp_type->second;
            tmp_type++;
        }

        vk::DescriptorPoolCreateInfo create_info{};
        create_info.setPoolSizes(sizes);
        create_info.maxSets = layouts.size();
        try_vk(static_cast<vk::DescriptorPool&>(*this) = device.createDescriptorPool(create_info));

        std::vector<vk::DescriptorSetLayout> llayouts;
        llayouts.reserve(layouts.size());
        for (auto layout : layouts)
        {
            llayouts.push_back(*layout);
        }

        vk::DescriptorSetAllocateInfo alloc_info{};
        alloc_info.descriptorPool = *this;
        alloc_info.descriptorSetCount = layouts.size();
        alloc_info.setSetLayouts(llayouts);
        try_vk(static_cast<std::vector<vk::DescriptorSet>&>(*this) = device.allocateDescriptorSets(alloc_info));
    }

    DescriptorPool::~DescriptorPool()
    {
        device_.destroyDescriptorPool(*this);
    }

    void DescriptorPool::update_sets(vk::WriteDescriptorSet& write, uint32_t set)
    {
        write.dstSet = this->std::vector<vk::DescriptorSet>::at(set);
        device_.updateDescriptorSets(write, {});
    }
}; // namespace proj