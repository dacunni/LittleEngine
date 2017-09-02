#include <limits>

// Using the Open Asset Import Library ("assimp" - http://assimp.sourceforge.net/ )
//#include <assimp/assimp.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "AssetLoader.h"
#include "AxisAlignedSlab.h"
#include "Mesh.h"


bool AssetLoader::loadMesh( const std::string & filename, Mesh & mesh )
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

    return true;
}
