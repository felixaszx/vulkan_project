#ifndef RENDER_MESH_HPP
#define RENDER_MESH_HPP

#include <memory>
#include "ext/buffer.hpp"
#include "vk/vk.hpp"
#include "glms.hpp"

namespace proj
{
    namespace render
    {
        class Mesh;
        class MeshData
        {
          private:
            uint32_t mesh_size_ = 0;
            uint64_t vert_count_ = 0;
            std::unique_ptr<Buffer> vert_buffer_ = nullptr;
            std::unique_ptr<Buffer> index_buffer_ = nullptr;
            std::unique_ptr<Buffer> matric_buffer_ = nullptr;

          public:
            std::vector<glm::mat4> instance_matrices_{};

            MeshData(vma::Allocator allocator, vk::Queue graphics, vk::CommandBuffer cmd, //
                     const std::vector<glm::vec3>& positions,                             //
                     const std::vector<glm::vec3>& normals,                               //
                     const std::vector<glm::vec3>& uvs,                                   //
                     const std::vector<glm::vec3>& colors);

            [[nodiscard]]
            std::vector<Mesh> get_meshes(vk::Queue graphics, vk::CommandBuffer cmd,         //
                                         const std::vector<uint32_t>& meshes_indices,       //
                                         const std::vector<uint32_t>& meshes_indices_count, //
                                         const std::vector<uint32_t>& meshes_vert_count,    //
                                         uint32_t instance_count = 1);
            void bind_data(vk::CommandBuffer cmd);
            void update_instance_matrices();
            uint32_t mesh_size() const { return mesh_size_; }

            static const std::vector<vk::VertexInputBindingDescription> vertex_bindings();
            static const std::vector<vk::VertexInputAttributeDescription> vertex_attributes();
        };

        class Mesh
        {
            friend MeshData;

          private:
            MeshData& data_;
            uint32_t first_vert = 0;
            uint32_t first_index = 0;
            uint32_t indices_count_ = 0;

            Mesh(MeshData& data, vk::DeviceSize first_vert, vk::DeviceSize first_index, uint32_t indices_count);

          public:
            void draw_instanced(vk::CommandBuffer cmd, uint32_t instance_count = 1);
        };
    }; // namespace render
};     // namespace proj

#endif // RENDER_MESH_HPP
