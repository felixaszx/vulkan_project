#include "render/mesh.hpp"

namespace proj
{
    namespace render
    {
        MeshData::MeshData(vma::Allocator allocator, vk::Queue graphics, vk::CommandBuffer cmd, //
                           const std::vector<glm::vec3>& positions,                             //
                           const std::vector<glm::vec3>& normals,                               //
                           const std::vector<glm::vec3>& uvs,                                   //
                           const std::vector<glm::vec3>& colors)
            : vert_count_(positions.size())
        {
            using namespace ext;
            // add all vertices to storage
            std::vector<glm::vec3> vert_buffer_storage;
            vert_buffer_storage.insert(vert_buffer_storage.end(), positions.begin(), positions.end());
            vert_buffer_storage.insert(vert_buffer_storage.end(), normals.begin(), normals.end());
            vert_buffer_storage.insert(vert_buffer_storage.end(), uvs.begin(), uvs.end());
            vert_buffer_storage.insert(vert_buffer_storage.end(), colors.begin(), colors.end());

            // factory types and create buffer
            BufferCreator vert_buffer_creator(allocator, vertex_buffer, device_local, trans_dst);
            BufferCreator staging_buffer_creator(allocator, trans_src, host_seq);
            vert_buffer_.reset(new Buffer(vert_buffer_creator.create(vert_buffer_storage.size() * sizeof(glm::vec3))));
            Buffer staging(staging_buffer_creator.create(vert_buffer_storage.size() * sizeof(glm::vec3)));
            // copy to staging
            memcpy(staging.map_memory(), vert_buffer_storage.data(), staging.size_);

            // copy buffer
            vk::CommandBufferBeginInfo begin{};
            begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            vk::BufferCopy region{};
            region.size = staging.size_;

            cmd.begin(begin);
            cmd.copyBuffer(staging, *vert_buffer_, region);
            cmd.end();

            vk::SubmitInfo submit_info{};
            submit_info.commandBufferCount = 1;
            submit_info.setCommandBuffers(cmd);
            graphics.submit(submit_info);

            graphics.waitIdle();
            cmd.reset();
        }

        std::vector<Mesh> MeshData::get_meshes(vk::Queue graphics, vk::CommandBuffer cmd,         //
                                               const std::vector<uint32_t>& meshes_indices,       //
                                               const std::vector<uint32_t>& meshes_indices_count, //
                                               const std::vector<uint32_t>& meshes_vert_count,    //
                                               uint32_t instance_count)
        {
            using namespace ext;
            size_t index_size = meshes_indices.size() * sizeof(uint32_t);
            // factory types and create buffers
            BufferCreator matric_buffer_creator(*vert_buffer_, vertex_buffer, host_seq);
            BufferCreator index_buffer_creator(*vert_buffer_, index_buffer, device_local, trans_dst);
            BufferCreator staging_buffer_creator(*vert_buffer_, trans_src, host_seq);
            index_buffer_.reset(new Buffer(index_buffer_creator.create(index_size)));
            matric_buffer_.reset(new Buffer(matric_buffer_creator.create(instance_count * sizeof(glm::mat4))));
            Buffer staging(staging_buffer_creator.create(meshes_indices.size() * sizeof(uint32_t)));

            // set basic info
            mesh_size_ = meshes_indices_count.size();
            instance_matrices_.resize(mesh_size_, glm::mat4(1.0f));
            matric_buffer_->map_memory();
            update_instance_matrices();

            // copy index to statging then copy to index buffer
            memcpy(staging.map_memory(), meshes_indices.data(), index_size);
            vk::CommandBufferBeginInfo begin{};
            begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            vk::BufferCopy region{};
            region.size = index_size;

            cmd.begin(begin);
            cmd.copyBuffer(staging, *index_buffer_, region);
            cmd.end();

            vk::SubmitInfo submit_info{};
            submit_info.setCommandBuffers(cmd);

            graphics.submit(submit_info);
            graphics.waitIdle();
            cmd.reset();

            // fill mesh info
            uint32_t vert_offset = 0;
            uint32_t indices_offset = 0;
            std::vector<Mesh> meshes{};
            for (int i = 0; i < meshes_vert_count.size(); i++)
            {
                meshes.push_back(Mesh(*this, vert_offset, indices_offset, meshes_indices_count[i]));
                vert_offset += meshes_vert_count[i];
                indices_offset += meshes_indices_count[i];
            }

            return meshes;
        }

        void MeshData::update_instance_matrices()
        {
            memcpy(matric_buffer_->mapping(), instance_matrices_.data(), sizeof(glm::mat4) * instance_matrices_.size());
        }

        void MeshData::bind_data(vk::CommandBuffer cmd)
        {
            update_instance_matrices();
            vk::Buffer vertex_buffers[5] = {*vert_buffer_, *vert_buffer_, *vert_buffer_, *vert_buffer_,
                                            *matric_buffer_};

            vk::DeviceSize vert_offsets[5]{};
            for (int i = 0; i < 4; i++)
            {
                vert_offsets[i] = i * vert_count_ * sizeof(glm::vec3);
            }

            cmd.bindVertexBuffers(0, vertex_buffers, vert_offsets);
            cmd.bindIndexBuffer(*index_buffer_, 0, vk::IndexType::eUint32);
        }

        const std::vector<vk::VertexInputBindingDescription> MeshData::vertex_bindings()
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

        const std::vector<vk::VertexInputAttributeDescription> MeshData::vertex_attributes()
        {
            std::vector<vk::VertexInputAttributeDescription> attributes(8);
            for (uint32_t i = 0; i < 4; i++)
            {
                attributes[i].binding = i;
                attributes[i].location = i;
                attributes[i].format = vk::Format::eR32G32B32Sfloat;
            }

            for (uint32_t i = 4; i < 8; i++)
            {
                attributes[i].binding = 4;
                attributes[i].location = i;
                attributes[i].format = vk::Format::eR32G32B32A32Sfloat;
                attributes[i].offset = (i - 4) * sizeof(glm::vec4);
            }

            return attributes;
        }

        Mesh::Mesh(MeshData& data, vk::DeviceSize first_vert, vk::DeviceSize first_index, uint32_t indices_count)
            : data_(data),
              first_vert(first_vert),
              first_index(first_index),
              indices_count_(indices_count)
        {
        }

        void Mesh::draw_instanced(vk::CommandBuffer cmd, uint32_t instance_count)
        {
            if (instance_count <= data_.instance_matrices_.size())
            {
                cmd.drawIndexed(indices_count_, instance_count, first_index, first_vert, 0);
                return;
            }
            cmd.drawIndexed(indices_count_, data_.instance_matrices_.size(), first_index, first_vert, 0);
        }
    }; // namespace render
};     // namespace proj