#ifndef __ASSETLOADER__
#define __ASSETLOADER__

#include <string>

#include "Image.h"
#include "Mesh.h"

class AssetLoader {
public:
    AssetLoader() {}
    ~AssetLoader() {}
    
    bool loadMesh( const std::string & filename, Mesh & mesh,
                   bool normalizeScale = true,
                   float normScaleFactor = 1.0 );

    bool loadMeshes( const std::string & filename,
                     std::vector<std::shared_ptr<Mesh>> & meshes,
                     std::vector<RGBImage<unsigned char>> & textures,
                     bool normalizeScale = true,
                     float normScaleFactor = 1.0 );

    bool loadMeshData(const std::string & filename,
                      std::vector<std::shared_ptr<MeshData>> & meshDatas,
                      std::vector<RGBImage<unsigned char>> & textures);
};


#endif
