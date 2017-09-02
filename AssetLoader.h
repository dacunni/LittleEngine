#ifndef __ASSETLOADER__
#define __ASSETLOADER__

#include <string>

class Traceable;
class Mesh;

class AssetLoader {
public:
    AssetLoader() {}
    ~AssetLoader() {}
    
    bool loadMesh( const std::string & filename, Mesh & mesh,
                   bool normalizeScale = true,
                   float normScaleFactor = 1.0 );

};


#endif
