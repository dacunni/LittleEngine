#include <limits>

// Using the Open Asset Import Library ("assimp" - http://assimp.sourceforge.net/ )
//#include <assimp/assimp.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "AssetLoader.h"
#include "AxisAlignedSlab.h"
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

#if 1 // Merging multipart meshes into one
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

        printf( "Mesh[%u] Has Positions=%d(%u) Faces=%d(%u) Normals=%d Bones=%d NumUV=%d (%d) Name='%s'\n", mesh_index, 
                (int) aimesh->HasPositions(), aimesh->mNumVertices,
                (int) aimesh->HasFaces(), aimesh->mNumFaces,
                (int) aimesh->HasNormals(),
                (int) aimesh->HasBones(),
                (int) aimesh->GetNumUVChannels(), (int) aimesh->mNumUVComponents[0],
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
#else

    aiMesh ** meshes = scene->mMeshes;
    // FIXME - just getting the first mesh for now
    unsigned int mesh_index = 0;
    
    aiMesh * aimesh = meshes[mesh_index];

    printf( "Mesh[%u] Has Positions=%d(%u) Faces=%d(%u) Normals=%d Bones=%d NumUV=%d (%d)\n", mesh_index, 
            (int) aimesh->HasPositions(), aimesh->mNumVertices,
            (int) aimesh->HasFaces(), aimesh->mNumFaces,
            (int) aimesh->HasNormals(),
            (int) aimesh->HasBones(),
            (int) aimesh->GetNumUVChannels(), (int) aimesh->mNumUVComponents[0]
          );

    bool has_uv = aimesh->GetNumUVChannels() > 0 &&
                  aimesh->mNumUVComponents[0] >= 2;
    
    mesh.vertices.resize( aimesh->mNumVertices );
    mesh.normals.resize( aimesh->mNumVertices );
    // FIXME: Triangles only, can we have quads?
    mesh.indices.resize( aimesh->mNumFaces * 3 );
    if( has_uv ) { mesh.textureUVCoords.resize( aimesh->mNumVertices ); }

    float scale = 1.0f;

    // Find bounds of mesh so we can apply scaling
    float minx = std::numeric_limits<float>::max(), maxx = -std::numeric_limits<float>::max();
    float miny = std::numeric_limits<float>::max(), maxy = -std::numeric_limits<float>::max();
    float minz = std::numeric_limits<float>::max(), maxz = -std::numeric_limits<float>::max();
    for( unsigned int vi = 0; vi < aimesh->mNumVertices; ++vi ) {
        const auto v = aimesh->mVertices[vi];
        minx = std::min(v.x, minx);
        miny = std::min(v.y, miny);
        minz = std::min(v.z, minz);
        maxx = std::max(v.x, maxx);
        maxy = std::max(v.y, maxy);
        maxz = std::max(v.z, maxz);
    }

    float maxwidth = std::max(maxx - minx, std::max(maxy - miny, maxz - minz));
    scale = 1.0f / maxwidth;

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
#endif

    return true;
}
