#include <limits>
#include <map>

// Using the Open Asset Import Library ("assimp" - http://assimp.sourceforge.net/ )
//#include <assimp/assimp.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "AssetLoader.h"
#include "Mesh.h"
#include "Bounds.h"

bool AssetLoader::loadMeshData(const std::string & filename,
                               MeshDataGroup & meshDatas,
                               std::vector<std::shared_ptr<ImageBase>> & textures)
{
    printf("AssetLoader::loadMeshData(%s, ...)\n", filename.c_str());

    if(meshDataCache.contains(filename)) {
        printf("Found mesh data cache entry for %s\n", filename.c_str());
        meshDatas = meshDataCache.get(filename);
        return true;
    }

    std::string basePath("");
    auto lastSlash = filename.find_last_of("/");
    if( lastSlash != std::string::npos ) {
        basePath = filename.substr(0, lastSlash);
    }

    Assimp::Importer importer;
    
    const aiScene * scene = importer.ReadFile( filename,
                                               aiProcess_Triangulate
                                               | aiProcess_FindInvalidData
                                               | aiProcess_GenSmoothNormals
                                               //| aiProcess_GenNormals
                                               | aiProcess_FixInfacingNormals
                                             );
    
    if( !scene ) {
        fprintf( stderr, "Failed to load %s\n", filename.c_str() );
        return false;
    }
    
    printf( "Loaded scene %s\n", filename.c_str() );
    printf( " - # meshes -> %u\n", scene->mNumMeshes );

    printf( "Scene:"
            " HasMeshes = %d (%u)"
            " HasMaterials = %d (%u)"
            " HasTextures = %d (%u)"
            "\n",
            (int) scene->HasMeshes(), scene->mNumMeshes,
            (int) scene->HasMaterials(), scene->mNumMaterials,
            (int) scene->HasTextures(), scene->mNumTextures
          );

    // Load textures
    std::vector< std::string > diffuseTexturePaths;
    std::map<unsigned int, GLuint> materialToDiffuseTextureIndex;
    std::map<unsigned int, GLuint> diffuseTextureIndexToTextureID;

    // Find paths to each texture
    for( unsigned int materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++ ) {
        printf("Material %3u\n", materialIndex);
        auto & material = scene->mMaterials[materialIndex];
        int texIndex = 0;
        aiString aipath;
        while( AI_SUCCESS == material->GetTexture( aiTextureType_DIFFUSE, texIndex, &aipath ) ) {
            std::string path( aipath.C_Str() );
            std::replace(path.begin(), path.end(), '\\', '/');
            printf( "  diffuse %d : %s\n", texIndex, path.c_str() );
            materialToDiffuseTextureIndex[materialIndex] = diffuseTexturePaths.size();
            diffuseTexturePaths.push_back( path );
            texIndex++;
        }
    }

    // Load texture images
    unsigned int textureIndex = 0;
    for( const auto & path : diffuseTexturePaths ) {
        std::shared_ptr<ImageBase> texImage = loadImage(basePath + "/" + path);
        textures.push_back(texImage);
        GLuint texID = texImage->uploadGL();
        // FIXME - should really map material index -> texture index -> texture id
        //         because we could have multiple textures per material
        diffuseTextureIndexToTextureID[textureIndex] = texID;
        textureIndex++;
    }

    // Extra mesh features
    for( unsigned int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index ) {
        aiMesh * aimesh = scene->mMeshes[mesh_index];
        bool has_uv = aimesh->GetNumUVChannels() > 0 && aimesh->mNumUVComponents[0] >= 2;

        printf( "Mesh[%u] Has Positions=%d(%u) Faces=%d(%u) Normals=%d Bones=%d NumUV=%d (%d) Material=%u Name='%s'\n", mesh_index, 
                (int) aimesh->HasPositions(), aimesh->mNumVertices,
                (int) aimesh->HasFaces(), aimesh->mNumFaces,
                (int) aimesh->HasNormals(),
                (int) aimesh->HasBones(),
                (int) aimesh->GetNumUVChannels(), (int) aimesh->mNumUVComponents[0],
                aimesh->mMaterialIndex,
                aimesh->mName.C_Str()
              );

        meshDatas.emplace_back(std::make_shared<MeshData>());
        auto & meshData = *meshDatas.back();

        meshData.vertices.resize( aimesh->mNumVertices );
        meshData.normals.resize( aimesh->mNumVertices );
        meshData.indices.resize( aimesh->mNumFaces * 3 );
        if( has_uv ) {
            meshData.textureUVCoords.resize( aimesh->mNumVertices );
        }
        if(materialToDiffuseTextureIndex.find(aimesh->mMaterialIndex) != materialToDiffuseTextureIndex.end()) {
            auto textureIndex = materialToDiffuseTextureIndex[aimesh->mMaterialIndex];
            auto textureId = diffuseTextureIndexToTextureID[textureIndex];
            meshData.setTexture(textureId);
        }

        for( unsigned int vi = 0; vi < aimesh->mNumVertices; ++vi ) {
            const auto v = aimesh->mVertices[vi];
            const auto n = aimesh->mNormals[vi];

            meshData.vertices[vi].set( v.x, v.y, v.z );
            meshData.normals[vi].set( n.x, n.y, n.z );

            if( has_uv ) {
                const auto tc = aimesh->mTextureCoords[0][vi];
                meshData.textureUVCoords[vi] = { tc.x, 1.0f - tc.y };
            }
        }

        uint32_t index = 0;
        for( unsigned int ti = 0; ti < aimesh->mNumFaces; ++ti ) {
            const auto t = aimesh->mFaces[ti];

            if(t.mNumIndices != 3) {
                printf("WARNING: Face is not a triangle!\n");
            }

            meshData.indices[index++] = t.mIndices[0];
            meshData.indices[index++] = t.mIndices[1];
            meshData.indices[index++] = t.mIndices[2];
        }
    }

    printf("Adding mesh data to mesh data cache for %s\n", filename.c_str());
    meshDataCache.add(filename, meshDatas);

    return true;
}

bool AssetLoader::loadMesh( const std::string & filename, Mesh & mesh,
                            bool normalizeScale, float normScaleFactor )
{
    std::vector<std::shared_ptr<MeshData>> meshDatas;
    std::vector<std::shared_ptr<ImageBase>> textures;

    bool ok = loadMeshData(filename, meshDatas, textures);

    if(!ok) {
        return false;
    }

    for(auto & meshData : meshDatas) {
        mesh.vertices.insert(mesh.vertices.end(), meshData->vertices.begin(), meshData->vertices.end());
        mesh.normals.insert(mesh.normals.end(), meshData->normals.begin(), meshData->normals.end());
        mesh.textureUVCoords.insert(mesh.textureUVCoords.end(), meshData->textureUVCoords.begin(), meshData->textureUVCoords.end());
        mesh.indices.insert(mesh.indices.end(), meshData->indices.begin(), meshData->indices.end());
    }

    return true;
}

bool AssetLoader::loadMeshes( const std::string & filename,
                              std::vector<std::shared_ptr<Mesh>> & meshes,
                              std::vector<std::shared_ptr<ImageBase>> & textures,
                              bool normalizeScale,
                              float normScaleFactor )
{
    std::vector<std::shared_ptr<MeshData>> meshDatas;

    bool ok = loadMeshData(filename, meshDatas, textures);

    if(!ok) {
        return false;
    }

    for(auto & meshData : meshDatas) {
        meshes.emplace_back(std::make_shared<Mesh>());
        Mesh & mesh = *meshes.back();
        mesh.vertices        = meshData->vertices;
        mesh.normals         = meshData->normals;
        mesh.textureUVCoords = meshData->textureUVCoords;
        mesh.indices         = meshData->indices;
    }

    return true;
}


