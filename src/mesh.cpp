#include "mesh.hpp"

namespace proj
{

    MeshDataHolder::MeshDataHolder(vma::Allocator allocator, vk::Queue graphics, vk::CommandBuffer cmd, //
                                   const std::vector<glm::vec3>& positions,                             //
                                   const std::vector<glm::vec3>& normals,                               //
                                   const std::vector<glm::vec3>& uvs_,                                  //
                                   const std::vector<glm::vec3>& colors)
    {
        std::vector<glm::vec3> vert_buffer_storage;
        vert_buffer_storage.insert(vert_buffer_storage.end(), positions.begin(), positions.end());
        vert_buffer_storage.insert(vert_buffer_storage.end(), normals.begin(), normals.end());
        vert_buffer_storage.insert(vert_buffer_storage.end(), uvs_.begin(), uvs_.end());
        vert_buffer_storage.insert(vert_buffer_storage.end(), colors.begin(), colors.end());

        vk::BufferCreateInfo buffer_info{};
        buffer_info.size = (positions.size() + normals.size() + uvs_.size() + colors.size()) * sizeof(glm::vec3);
        buffer_info.usage = vk::BufferUsageFlagBits::eIndexBuffer |  //
                            vk::BufferUsageFlagBits::eVertexBuffer | //
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
        memcpy(staging.mapping(), vert_buffer_storage.data(),
               vert_buffer_storage.size() * sizeof(vert_buffer_storage[0]));
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

    std::vector<Mesh> MeshDataHolder::get_meshes(const std::vector<uint32_t>& meshes_indices_count, //
                                                 const std::vector<uint32_t>& meshes_vert_count)
    {

        std::vector<Mesh> meshes{};
        vk::BufferCreateInfo buffer_info{};
        buffer_info.size = meshes_indices_count.size() * sizeof(glm::mat4);
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

        size_t vert_offset;
        for (int i = 0; i < meshes_vert_count.size(); i++)
        {
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
}; // namespace proj