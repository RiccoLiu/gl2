#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <stb_image.h>

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "mesh.h"
#include "shader.h"

// 模型封装：负责从文件加载场景并转换为 Mesh 列表。
// - 拥有 textures_loaded 缓存中所有纹理的 GL 句柄，析构时统一释放。
// - Mesh 只引用纹理 id，不负责释放，避免双重释放。
// - 只可移动，不可拷贝。
class Model
{
public:
    explicit Model(const std::string& path)
    {
        loadModel(path);
    }

    ~Model()
    {
        for (const auto& [file, id] : texturesLoaded_)
        {
            if (id != 0)
                glDeleteTextures(1, &id);
        }
    }

    Model(const Model&)            = delete;
    Model& operator=(const Model&) = delete;

    Model(Model&&) noexcept            = default;
    Model& operator=(Model&&) noexcept = default;

    bool empty() const { return meshes_.empty(); }

    void draw(const Shader& shader) const
    {
        for (const Mesh& mesh : meshes_)
            mesh.draw(shader);
    }

private:
    void loadModel(const std::string& path)
    {
        directory_ = std::filesystem::path(path).parent_path().string();

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |
                                                          aiProcess_GenSmoothNormals |
                                                          aiProcess_FlipUVs |
                                                          aiProcess_CalcTangentSpace);

        if (scene == nullptr || scene->mRootNode == nullptr ||
            (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0)
        {
            throw std::runtime_error(std::string("Model: load failed [") + path + "]: " +
                                     importer.GetErrorString());
        }
        processNode(scene->mRootNode, scene);
    }

    void processNode(const aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes_.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
            processNode(node->mChildren[i], scene);
    }

    Mesh processMesh(const aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        vertices.reserve(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex;
            vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            if (mesh->HasNormals())
                vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            if (mesh->HasTextureCoords(0)) {
                vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
            } else {
                vertex.texCoords = glm::vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        std::vector<unsigned int> indices;
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        std::vector<Texture> textures;
        const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        auto append = [&](aiTextureType type, const std::string& typeName) {
            std::vector<Texture> found = loadMaterialTextures(material, type, typeName);
            textures.insert(textures.end(), std::make_move_iterator(found.begin()),
                            std::make_move_iterator(found.end()));
        };
        append(aiTextureType_DIFFUSE, "texture_diffuse");
        append(aiTextureType_SPECULAR, "texture_specular");

        return Mesh(std::move(vertices), std::move(indices), std::move(textures));
    }

    std::vector<Texture> loadMaterialTextures(const aiMaterial* material, aiTextureType type,
                                              const std::string& typeName)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
        {
            aiString str;
            material->GetTexture(type, i, &str);
            std::string file = (std::filesystem::path(directory_) / str.C_Str()).string();

            // 哈希查找代替线性扫描，同一纹理只加载一次。
            auto it = texturesLoaded_.find(file);
            if (it != texturesLoaded_.end())
            {
                textures.push_back({it->second, typeName, file});
                continue;
            }

            unsigned int id = textureFromFile(file);
            if (id == 0)
                continue; // 加载失败的纹理不进入缓存，也不绑定到网格。

            texturesLoaded_.emplace(file, id);
            textures.push_back({id, typeName, file});
        }
        return textures;
    }

    static unsigned int textureFromFile(const std::string& file)
    {
        int width = 0, height = 0, nrComponents = 0;
        unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrComponents, 0);
        if (data == nullptr)
        {
            std::cerr << "Texture: failed to load at path: " << file << '\n';
            return 0;
        }

        GLenum format = GL_RGBA; // 兜底：未知通道数按 RGBA 处理，避免传入未初始化的 format。
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;

        unsigned int textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return textureId;
    }

    std::vector<Mesh> meshes_;
    std::unordered_map<std::string, unsigned int> texturesLoaded_;
    std::string directory_;
};
