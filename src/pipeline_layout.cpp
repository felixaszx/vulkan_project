#include "pipeline_layout.hpp"

namespace proj
{

    DescriptorLayout::DescriptorLayout(vk::Device device,
                                       const std::vector<vk::DescriptorSetLayoutBinding>& bindings, //
                                       vk::DescriptorSetLayoutCreateFlagBits flags)
        : vk::Device(device)
    {
        for (const vk::DescriptorSetLayoutBinding& binding : bindings)
        {
            auto result = types_count_.find(binding.descriptorType);
            if (result == types_count_.end())
            {
                types_count_.insert({binding.descriptorType, 1});
            }
            else
            {
                result->second++;
            }
        }

        vk::DescriptorSetLayoutCreateInfo create_info{};
        create_info.flags = flags;
        create_info.setBindings(bindings);
        static_cast<vk::DescriptorSetLayout&>(*this) = device.createDescriptorSetLayout(create_info);
    }

    DescriptorLayout::~DescriptorLayout()
    {
        this->destroyDescriptorSetLayout(*this);
    }

    DescriptorPool::DescriptorPool(vk::Device device, const std::vector<DescriptorLayout*>& layouts)
        : vk::Device(device)
    {
        for (const auto layout : layouts)
        {
            const std::unordered_map<vk::DescriptorType, uint32_t>& types = layout->types_count_;
            for (const auto& type : types)
            {
                auto tmp_type = types_count_.find(type.first);
                if (tmp_type == types_count_.end())
                {
                    types_count_.insert(type);
                }
                else
                {
                    tmp_type->second += type.second;
                }
            }
        }

        std::vector<vk::DescriptorPoolSize> sizes(types_count_.size());
        auto tmp_type = types_count_.begin();
        for (auto& size : sizes)
        {
            size.type = tmp_type->first;
            size.descriptorCount = tmp_type->second;
            tmp_type++;
        }

        vk::DescriptorPoolCreateInfo create_info{};
        create_info.setPoolSizes(sizes);
        create_info.maxSets = layouts.size();
        static_cast<vk::DescriptorPool&>(*this) = device.createDescriptorPool(create_info);

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
        sets_ = device.allocateDescriptorSets(alloc_info);
    }

    DescriptorPool::~DescriptorPool()
    {
        this->destroyDescriptorPool(*this);
    }

    vk::DescriptorSet DescriptorPool::get_set(uint32_t index)
    {
        return sets_[index];
    }

    std::vector<vk::DescriptorSet> DescriptorPool::get_sets()
    {
        return sets_;
    }

    void DescriptorPool::update_sets(vk::WriteDescriptorSet& write, uint32_t set_index)
    {
        write.dstSet = sets_[set_index];
        this->updateDescriptorSets(write, {});
    }
}; // namespace proj