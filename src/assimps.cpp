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
            positions_.resize(mesh->mNumVertices);
            normals_.resize(mesh->mNumVertices);
            uvs_.resize(mesh->mNumVertices);
            colors_.resize(mesh->mNumVertices);
            memcpy(positions_.data(), mesh->mVertices, mesh->mNumVertices * sizeof(glm::vec3));
            memcpy(normals_.data(), mesh->mNormals, mesh->mNumVertices * sizeof(glm::vec3));

            if (mesh->mTextureCoords[0] == nullptr)
            {
                std::fill(uvs_.begin(), uvs_.end(), glm::vec3(0.0f, 0.0f, 0.0f));
            }

            if (mesh->mColors[0] == nullptr)
            {
                std::fill(colors_.begin(), colors_.end(), glm::vec3(1.0f, 1.0f, 1.0f));
            }
        }
    }
}; // namespace proj