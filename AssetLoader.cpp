#include <limits>
#include <map>

// Using the Open Asset Import Library ("assimp" - http://assimp.sourceforge.net/ )
//#include <assimp/assimp.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "AssetLoader.h"
#include "Mesh.h"


bool AssetLoader::loadMesh( const std::string & filename, Mesh & mesh,
                            bool normalizeScale, float normScaleFactor )
{
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
    
    printf( "Loaded %s\n", filename.c_str() );
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

    //
    // Merging multipart meshes into one
    //

    // Find bounds of mesh so we can apply scaling
    float minx = std::numeric_limits<float>::max(), maxx = -std::numeric_limits<float>::max();
    float miny = std::numeric_limits<float>::max(), maxy = -std::numeric_limits<float>::max();
    float minz = std::numeric_limits<float>::max(), maxz = -std::numeric_limits<float>::max();
    float maxwidth;
    float scale = 1.0f;

    for( unsigned int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index ) {
        aiMesh * aimesh = scene->mMeshes[mesh_index];
        for( unsigned int vi = 0; vi < aimesh->mNumVertices; ++vi ) {
            const auto v = aimesh->mVertices[vi];
            minx = std::min(v.x, minx);
            miny = std::min(v.y, miny);
            minz = std::min(v.z, minz);
            maxx = std::max(v.x, maxx);
            maxy = std::max(v.y, maxy);
            maxz = std::max(v.z, maxz);
        }

        maxwidth = std::max(maxx - minx, std::max(maxy - miny, maxz - minz));
    }
    printf("Bounds: %f - %f, %f - %f, %f - %f\n", minx, maxx, miny, maxy, minz, maxz);
    if( normalizeScale ) {
        scale = normScaleFactor / maxwidth;
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

        // Where to start adding parts of this mesh
        unsigned int vertexStart = mesh.vertices.size();
        unsigned int normalStart = mesh.normals.size();
        unsigned int indexStart = mesh.indices.size();
        unsigned int uvStart = mesh.textureUVCoords.size();

        mesh.vertices.resize( mesh.vertices.size() + aimesh->mNumVertices );
        mesh.normals.resize( mesh.normals.size() + aimesh->mNumVertices );
        // FIXME: Triangles only, can we have quads?
        mesh.indices.resize( mesh.indices.size() + aimesh->mNumFaces * 3 );
        if( has_uv ) { mesh.textureUVCoords.resize( mesh.textureUVCoords.size() + aimesh->mNumVertices ); }

        for( unsigned int vi = 0; vi < aimesh->mNumVertices; ++vi ) {
            const auto v = aimesh->mVertices[vi];
            const auto n = aimesh->mNormals[vi];

            mesh.vertices[vertexStart + vi].set( v.x, v.y, v.z );
            mesh.normals[normalStart + vi].set( n.x, n.y, n.z );

            if( has_uv ) {
                const auto tc = aimesh->mTextureCoords[0][vi];
                mesh.textureUVCoords[uvStart + vi] = { tc.x, tc.y };
            }

            mesh.vertices[vertexStart + vi][0] *= scale;
            mesh.vertices[vertexStart + vi][1] *= scale;
            mesh.vertices[vertexStart + vi][2] *= scale;
        }

        uint32_t index = 0;
        for( unsigned int ti = 0; ti < aimesh->mNumFaces; ++ti ) {
            const auto t = aimesh->mFaces[ti];
            mesh.indices[indexStart + index++] = vertexStart + t.mIndices[0];
            mesh.indices[indexStart + index++] = vertexStart + t.mIndices[1];
            mesh.indices[indexStart + index++] = vertexStart + t.mIndices[2];
        }
    }

    return true;
}

bool AssetLoader::loadMeshes( const std::string & filename,
                              std::vector<std::shared_ptr<Mesh>> & meshes,
                              std::vector< RGBImage<unsigned char> > & textures,
                              bool normalizeScale,
                              float normScaleFactor )
{
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

    //
    // Load textures
    //
    std::vector< std::string > diffuseTexturePaths;
    std::map<unsigned int, GLuint> materialToDiffuseTextureID;

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
            diffuseTexturePaths.push_back( path );

            textures.emplace_back();
            RGBImage<unsigned char> & texImage = textures.back();
            texImage.loadImage( basePath + "/" + path );
            GLuint texID = texImage.uploadGL();
            printf("  texID %u\n", texID);
            materialToDiffuseTextureID[materialIndex] = texID;

            texIndex++;
        }
    }

    // Load texture images
    unsigned int materialIndex = 0;
    for( const auto & path : diffuseTexturePaths ) {
        textures.emplace_back();
        RGBImage<unsigned char> & texImage = textures.back();
        texImage.loadImage( basePath + "/" + path );
        GLuint texID = texImage.uploadGL();
        printf("  texID %u\n", texID);
        // FIXME - should really map material index -> texture index -> texture id
        //         because we could have multiple textures per material
        materialToDiffuseTextureID[materialIndex] = texID;
        materialIndex++;
    }

    // Find bounds of mesh so we can apply scaling
    float minx = std::numeric_limits<float>::max(), maxx = -std::numeric_limits<float>::max();
    float miny = std::numeric_limits<float>::max(), maxy = -std::numeric_limits<float>::max();
    float minz = std::numeric_limits<float>::max(), maxz = -std::numeric_limits<float>::max();
    float maxwidth;
    float scale = 1.0f;

    for( unsigned int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index ) {
        aiMesh * aimesh = scene->mMeshes[mesh_index];
        for( unsigned int vi = 0; vi < aimesh->mNumVertices; ++vi ) {
            const auto v = aimesh->mVertices[vi];
            minx = std::min(v.x, minx);
            miny = std::min(v.y, miny);
            minz = std::min(v.z, minz);
            maxx = std::max(v.x, maxx);
            maxy = std::max(v.y, maxy);
            maxz = std::max(v.z, maxz);
        }

        maxwidth = std::max(maxx - minx, std::max(maxy - miny, maxz - minz));
    }
    printf("Bounds: %f - %f, %f - %f, %f - %f\n", minx, maxx, miny, maxy, minz, maxz);
    if( normalizeScale ) {
        scale = normScaleFactor / maxwidth;
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

        meshes.emplace_back(std::make_shared<Mesh>());
        auto & mesh = *meshes.back();

        mesh.vertices.resize( aimesh->mNumVertices );
        mesh.normals.resize( aimesh->mNumVertices );
        mesh.indices.resize( aimesh->mNumFaces * 3 );
        if( has_uv ) {
            mesh.textureUVCoords.resize( aimesh->mNumVertices );
            mesh.setTexture( materialToDiffuseTextureID[aimesh->mMaterialIndex] );
        }

        for( unsigned int vi = 0; vi < aimesh->mNumVertices; ++vi ) {
            const auto v = aimesh->mVertices[vi];
            const auto n = aimesh->mNormals[vi];

            mesh.vertices[vi].set( v.x, v.y, v.z );
            mesh.normals[vi].set( n.x, n.y, n.z );

            if( has_uv ) {
                const auto tc = aimesh->mTextureCoords[0][vi];
                mesh.textureUVCoords[vi] = { tc.x, tc.y };
            }

            mesh.vertices[vi][0] *= scale;
            mesh.vertices[vi][1] *= scale;
            mesh.vertices[vi][2] *= scale;
        }

        uint32_t index = 0;
        for( unsigned int ti = 0; ti < aimesh->mNumFaces; ++ti ) {
            const auto t = aimesh->mFaces[ti];
            mesh.indices[index++] = t.mIndices[0];
            mesh.indices[index++] = t.mIndices[1];
            mesh.indices[index++] = t.mIndices[2];
        }
    }

    return true;
}


