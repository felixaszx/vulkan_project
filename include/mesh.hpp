#ifndef INCLUDE_MESH_HPP
#define INCLUDE_MESH_HPP

#include <memory>
#include "resources.hpp"
#include "glms.hpp"

namespace proj
{
    class Mesh;
    class MeshDataHolder
    {
        friend Mesh;

      private:
        uint32_t mesh_size_ = 0;
        std::unique_ptr<Buffer> vert_buffer_ = nullptr;
        std::unique_ptr<Buffer> matric_buffer_ = nullptr;

      public:
        std::vector<glm::mat4> instance_matrices_{};

        MeshDataHolder(vma::Allocator allocator, vk::Queue graphics, vk::CommandBuffer cmd, //
                       const std::vector<glm::vec3>& positions,                             //
                       const std::vector<glm::vec3>& normals,                               //
                       const std::vector<glm::vec3>& uvs_,                                  //
                       const std::vector<glm::vec3>& colors);
        ~MeshDataHolder();

        std::vector<Mesh> get_meshes(const std::vector<uint32_t>& meshes_indices_count, //
                                     const std::vector<uint32_t>& meshes_vert_count);

        static std::vector<vk::VertexInputBindingDescription> vertex_bindings();
        static std::vector<vk::VertexInputAttributeDescription> vertex_attributes();

        uint32_t mesh_size() const { return mesh_size_; }
        void update_instance_matrices();
    };

    class Mesh
    {
        friend MeshDataHolder;

      private:
        MeshDataHolder& holder_;
        vk::DeviceSize vert_offset_ = 0;
        vk::DeviceSize index_offset_ = 0;
        uint32_t indices_count_ = 0;

        Mesh(MeshDataHolder& holder, vk::DeviceSize vert_offset, vk::DeviceSize index_offset, uint32_t indices_count_);

      public:
        void draw_instanced(vk::CommandBuffer cmd, uint32_t instance_count = 1);
    };
}; // namespace proj

#endif // INCLUDE_MESH_HPP
