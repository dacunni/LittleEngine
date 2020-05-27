#ifndef __ASSETLOADER__
#define __ASSETLOADER__

#include <string>

#include "Image.h"
#include "Mesh.h"

class AssetLoader {
public:
    AssetLoader() = default;
    ~AssetLoader() = default;
    
    bool loadMesh( const std::string & filename, Mesh & mesh,
                   bool normalizeScale = true,
                   float normScaleFactor = 1.0 );

    bool loadMeshes( const std::string & filename,
                     std::vector<std::shared_ptr<Mesh>> & meshes,
                     std::vector<std::shared_ptr<ImageBase>> & textures,
                     bool normalizeScale = true,
                     float normScaleFactor = 1.0 );

    bool loadMeshData(const std::string & filename,
                      MeshDataGroup & meshDatas,
                      std::vector<std::shared_ptr<ImageBase>> & textures);

    MeshDataCache meshDataCache;
};


#endif
