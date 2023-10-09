#ifndef INCLUDE_ASSIMPS_HPP
#define INCLUDE_ASSIMPS_HPP

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "glms.hpp"

namespace proj
{
    struct MeshDataLoader
    {
        std::vector<glm::vec3> positions_{};
        std::vector<glm::vec3> normals_{};
        std::vector<glm::vec3> uvs_{};
        std::vector<glm::vec3> colors_{};
        std::vector<uint32_t> meshes_indices_{};

        std::vector<uint32_t> meshes_indices_count_{};
        std::vector<uint32_t> meshes_vert_count_{};

        MeshDataLoader(const std::string& file_path);
    };

}; // namespace proj

#endif // INCLUDE_ASSIMPS_HPP
