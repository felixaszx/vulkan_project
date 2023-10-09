#include "mesh.hpp"

namespace proj
{

    MeshDataHolder::MeshDataHolder(vma::Allocator allocator, vk::Queue graphics, vk::CommandBuffer cmd, //
                                   const std::vector<glm::vec3>& positions,                             //
                                   const std::vector<glm::vec3>& normals,                               //
                                   const std::vector<glm::vec3>& uvs,                                  //
                                   const std::vector<glm::vec3>& colors)
        : vert_count_(positions.size())
    {
        std::vector<glm::vec3> vert_buffer_storage;
        vert_buffer_storage.insert(vert_buffer_storage.end(), positions.begin(), positions.end());
        vert_buffer_storage.insert(vert_buffer_storage.end(), normals.begin(), normals.end());
        vert_buffer_storage.insert(vert_buffer_storage.end(), uvs.begin(), uvs.end());
        vert_buffer_storage.insert(vert_buffer_storage.end(), colors.begin(), colors.end());

        vk::BufferCreateInfo buffer_info{};
        buffer_info.size = vert_buffer_storage.size() * sizeof(glm::vec3);
        buffer_info.usage = vk::BufferUsageFlagBits::eVertexBuffer | //
                            vk::BufferUsageFlagBits::eTransferDst;
        vma::AllocationCreateInfo alloc_info{};
        alloc_info.usage = vma::MemoryUsage::eAutoPreferDevice;
        vert_buffer_.reset(new Buffer(allocator, buffer_info, alloc_info));

        vk::BufferCreateInfo staging_info{};
        staging_info.size = buffer_info.size;
        staging_info.usage = vk::BufferUsageFlagBits::eTransferSrc;
        alloc_info.usage = vma::MemoryUsage::eAutoPreferHost;
        alloc_info.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
        alloc_info.requiredFlags = vk::MemoryPropertyFlagBits::eHostCoherent;

        Buffer staging(allocator, staging_info, alloc_info);
        staging.map_memory();
        memcpy(staging.mapping(), vert_buffer_storage.data(), buffer_info.size);
        staging.unmap_memory();

        vk::CommandBufferBeginInfo begin{};
        begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        vk::BufferCopy region{};
        region.size = buffer_info.size;

        cmd.begin(begin);
        cmd.copyBuffer(staging, *vert_buffer_, region);
        cmd.end();

        vk::SubmitInfo submit_info{};
        submit_info.commandBufferCount = 1;
        submit_info.setCommandBuffers(cmd);

        graphics.submit(submit_info);
        graphics.waitIdle();
    }

    MeshDataHolder::~MeshDataHolder() {}

    std::vector<Mesh> MeshDataHolder::get_meshes(vk::Queue graphics, vk::CommandBuffer cmd,         //
                                                 const std::vector<uint32_t>& meshes_indices,       //
                                                 const std::vector<uint32_t>& meshes_indices_count, //
                                                 const std::vector<uint32_t>& meshes_vert_count,    //
                                                 uint32_t instance_count)
    {

        std::vector<Mesh> meshes{};
        vk::BufferCreateInfo buffer_info{};
        buffer_info.size = instance_count * sizeof(glm::mat4);
        buffer_info.usage = vk::BufferUsageFlagBits::eVertexBuffer;
        vma::AllocationCreateInfo alloc_info{};
        alloc_info.usage = vma::MemoryUsage::eAutoPreferHost;
        alloc_info.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
        alloc_info.requiredFlags = vk::MemoryPropertyFlagBits::eHostCoherent;
        matric_buffer_.reset(new Buffer(*vert_buffer_, buffer_info, alloc_info));
        matric_buffer_->map_memory();

        mesh_size_ = meshes_indices_count.size();
        instance_matrices_.resize(mesh_size_, glm::mat4(1.0f));
        update_instance_matrices();

        vk::BufferCreateInfo index_info{};
        index_info.size = meshes_indices.size() * sizeof(uint32_t);
        index_info.usage = vk::BufferUsageFlagBits::eIndexBuffer | //
                           vk::BufferUsageFlagBits::eTransferDst;
        vma::AllocationCreateInfo alloc_index{};
        alloc_index.usage = vma::MemoryUsage::eAutoPreferDevice;
        index_buffer_.reset(new Buffer(*vert_buffer_, index_info, alloc_index));

        vk::BufferCreateInfo staging_info{};
        staging_info.size = index_info.size;
        staging_info.usage = vk::BufferUsageFlagBits::eTransferSrc;
        Buffer staging(*index_buffer_, staging_info, alloc_info);
        staging.map_memory();
        memcpy(staging.mapping(), meshes_indices.data(), index_info.size);
        staging.unmap_memory();

        vk::CommandBufferBeginInfo begin{};
        begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        vk::BufferCopy region{};
        region.size = index_info.size;

        cmd.begin(begin);
        cmd.copyBuffer(staging, *index_buffer_, region);
        cmd.end();

        vk::SubmitInfo submit_info{};
        submit_info.commandBufferCount = 1;
        submit_info.setCommandBuffers(cmd);

        graphics.submit(submit_info);
        graphics.waitIdle();

        uint32_t vert_offset = 0;
        uint32_t indices_offset = 0;
        for (int i = 0; i < meshes_vert_count.size(); i++)
        {
            meshes.push_back(Mesh(*this, vert_offset, indices_offset, meshes_indices_count[i]));
            vert_offset += meshes_vert_count[i];
            indices_offset += meshes_indices_count[i];
        }

        return meshes;
    }

    std::vector<vk::VertexInputBindingDescription> MeshDataHolder::vertex_bindings()
    {
        std::vector<vk::VertexInputBindingDescription> binding(5);

        for (int i = 0; i < 4; i++)
        {
            binding[i].binding = i;
            binding[i].stride = sizeof(glm::vec3);
            binding[i].inputRate = vk::VertexInputRate::eVertex;
        }

        binding[4].binding = 4;
        binding[4].stride = sizeof(glm::mat4);
        binding[4].inputRate = vk::VertexInputRate::eInstance;

        return binding;
    }

    std::vector<vk::VertexInputAttributeDescription> MeshDataHolder::vertex_attributes()
    {
        std::vector<vk::VertexInputAttributeDescription> attributes(8);
        for (uint32_t i = 0; i < 4; i++)
        {
            attributes[i].binding = 0;
            attributes[i].location = i;
            attributes[i].format = vk::Format::eR32G32B32Sfloat;
        }

        for (uint32_t i = 4; i < 8; i++)
        {
            attributes[i].binding = 1;
            attributes[i].location = i;
            attributes[i].format = vk::Format::eR32G32B32A32Sfloat;
            attributes[i].offset = (i - 4) * sizeof(glm::vec4);
        }

        return attributes;
    }

    void MeshDataHolder::update_instance_matrices()
    {
        memcpy(matric_buffer_->mapping(), instance_matrices_.data(), sizeof(glm::mat4) * instance_matrices_.size());
    }

    bool MeshDataHolder::bind_data(vk::CommandBuffer cmd)
    {
        if (binded_)
        {
            return false;
        }
        binded_ = true;

        vk::Buffer vertex_buffers[5] = {*vert_buffer_, *vert_buffer_, *vert_buffer_, *vert_buffer_, *matric_buffer_};

        vk::DeviceSize vert_offsets[5]{};
        for (int i = 0; i < 4; i++)
        {
            vert_offsets[i] = i * vert_count_ * sizeof(glm::vec3);
        }

        cmd.bindVertexBuffers(0, vertex_buffers, vert_offsets);
        cmd.bindIndexBuffer(*index_buffer_, 0, vk::IndexType::eUint32);

        return true;
    }

    bool MeshDataHolder::unbind_data()
    {
        if (!binded_)
        {
            return false;
        }

        binded_ = false;
        return true;
    }

    Mesh::Mesh(MeshDataHolder& holder, vk::DeviceSize first_vert, vk::DeviceSize first_index, uint32_t indices_count)
        : holder_(holder),
          first_vert(first_vert),
          first_index(first_index),
          indices_count_(indices_count)
    {
    }

    void Mesh::draw_instanced(vk::CommandBuffer cmd, uint32_t instance_count)
    {
        holder_.bind_data(cmd);
        if (instance_count <= holder_.instance_matrices_.size())
        {
            cmd.drawIndexed(indices_count_, instance_count, first_index, first_vert, 0);
            return;
        }

        cmd.drawIndexed(indices_count_, holder_.instance_matrices_.size(), first_index, first_vert, 0);
        return;
    }
}; // namespace proj