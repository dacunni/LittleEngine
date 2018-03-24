#ifndef __ASSETLOADER__
#define __ASSETLOADER__

#include <string>

#include "Image.h"

class Traceable;
class Mesh;

class AssetLoader {
public:
    AssetLoader() {}
    ~AssetLoader() {}
    
    bool loadMesh( const std::string & filename, Mesh & mesh,
                   bool normalizeScale = true,
                   float normScaleFactor = 1.0 );

    bool loadMeshes( const std::string & filename,
                     std::vector<Mesh> & meshes,
                     std::vector< RGBImage<unsigned char> > & textures,
                     bool normalizeScale = true,
                     float normScaleFactor = 1.0 );

};


#endif
