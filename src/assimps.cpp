#include "assimps.hpp"

namespace proj
{

    MeshDataLoader::MeshDataLoader(const std::string& file_path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(file_path.c_str(), aiProcess_Triangulate | //
                                                                        aiProcess_GenNormals);

        meshes_indices_count_.reserve(scene->mNumMeshes);
        meshes_vert_count_.reserve(scene->mNumMeshes);

        for (int m = 0; m < scene->mNumMeshes; m++)
        {
            aiMesh* mesh = scene->mMeshes[m];
            meshes_indices_count_.push_back(mesh->mNumFaces * 3);

            for (int f = 0; f < mesh->mNumFaces; f++)
            {
                meshes_indices_.push_back(mesh->mFaces[f].mIndices[0]);
                meshes_indices_.push_back(mesh->mFaces[f].mIndices[1]);
                meshes_indices_.push_back(mesh->mFaces[f].mIndices[2]);
            }

            meshes_vert_count_.push_back(mesh->mNumVertices);

            size_t curr_size = positions_.size();
            positions_.resize(curr_size + mesh->mNumVertices);
            normals_.resize(curr_size + mesh->mNumVertices);
            uvs_.resize(curr_size + mesh->mNumVertices);
            colors_.resize(curr_size + mesh->mNumVertices);

            memcpy(positions_.data() + curr_size, mesh->mVertices, mesh->mNumVertices * sizeof(glm::vec3));
            memcpy(normals_.data() + curr_size, mesh->mNormals, mesh->mNumVertices * sizeof(glm::vec3));

            if (mesh->mTextureCoords[0] == nullptr)
            {
                std::fill(uvs_.begin() + curr_size, uvs_.end(), //
                          glm::vec3(0.0f, 0.0f, 0.0f));
            }
            else
            {
                memcpy(uvs_.data() + curr_size, mesh->mTextureCoords[0], mesh->mNumVertices * sizeof(glm::vec3));
            }

            if (mesh->mColors[0] == nullptr)
            {
                std::fill(colors_.begin() + curr_size, colors_.end(), //
                          glm::vec3(1.0f, 1.0f, 1.0f));
            }
            else
            {
                memcpy(uvs_.data() + curr_size, mesh->mColors[0], mesh->mNumVertices * sizeof(glm::vec3));
            }
        }
    }
}; // namespace proj