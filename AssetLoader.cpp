#include <limits>

// Using the Open Asset Import Library ("assimp" - http://assimp.sourceforge.net/ )
//#include <assimp/assimp.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "AssetLoader.h"
#include "AxisAlignedSlab.h"
#include "TriangleMesh.h"


TriangleMesh * AssetLoader::load( const std::string & filename )
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
        return nullptr;
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
    
    aiMesh * mesh = meshes[mesh_index];

    printf( "Mesh[%u] Has Positions=%d(%u) Faces=%d(%u) Normals=%d Bones=%d NumUV=%d (%d)\n", mesh_index, 
            (int) mesh->HasPositions(), mesh->mNumVertices,
            (int) mesh->HasFaces(), mesh->mNumFaces,
            (int) mesh->HasNormals(),
            (int) mesh->HasBones(),
            (int) mesh->GetNumUVChannels(), (int) mesh->mNumUVComponents[0]
          );

    bool has_uv = mesh->GetNumUVChannels() > 0 &&
                  mesh->mNumUVComponents[0] >= 2;
    
    TriangleMesh * trimesh = new TriangleMesh();
    
    trimesh->vertices.resize( mesh->mNumVertices );
    trimesh->normals.resize( mesh->mNumVertices );
    trimesh->triangles.resize( mesh->mNumFaces );

    if( has_uv ) {
        trimesh->textureUVCoords.resize( mesh->mNumVertices );
    }

    float scale = 1.0f;

    float minx = std::numeric_limits<float>::max(), maxx = -std::numeric_limits<float>::max();
    float miny = std::numeric_limits<float>::max(), maxy = -std::numeric_limits<float>::max();
    float minz = std::numeric_limits<float>::max(), maxz = -std::numeric_limits<float>::max();
 
    for( unsigned int vi = 0; vi < mesh->mNumVertices; ++vi ) {
        const auto v = mesh->mVertices[vi];
        minx = std::min(v.x, minx);
        miny = std::min(v.y, miny);
        minz = std::min(v.z, minz);
        maxx = std::max(v.x, maxx);
        maxy = std::max(v.y, maxy);
        maxz = std::max(v.z, maxz);
    }

    float maxwidth = std::max(maxx - minx, std::max(maxy - miny, maxz - minz));
    scale = 1.0f / maxwidth;

    for( unsigned int vi = 0; vi < mesh->mNumVertices; ++vi ) {
        const auto v = mesh->mVertices[vi];
        const auto n = mesh->mNormals[vi];

        trimesh->vertices[vi].set( v.x, v.y, v.z );
        trimesh->normals[vi].set( n.x, n.y, n.z );

        if( has_uv ) {
            const auto tc = mesh->mTextureCoords[0][vi];
            trimesh->textureUVCoords[vi] = { tc.x, tc.y };
        }

        trimesh->vertices[vi][0] *= scale;
        trimesh->vertices[vi][1] *= scale;
        trimesh->vertices[vi][2] *= scale;
        //printf("V %f %f %f\n", v.x, v.y, v.z); // TEMP
    }

    for( unsigned int ti = 0; ti < mesh->mNumFaces; ++ti ) {
        const auto t = mesh->mFaces[ti];
        trimesh->triangles[ti].vi[0] = t.mIndices[0];
        trimesh->triangles[ti].vi[1] = t.mIndices[1];
        trimesh->triangles[ti].vi[2] = t.mIndices[2];
    }

    AxisAlignedSlab * bounds = trimesh->getAxisAlignedBounds();
    printf("Bounds : "); bounds->print();
    delete bounds;

    return trimesh;
}
