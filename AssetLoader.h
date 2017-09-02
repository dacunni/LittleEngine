#ifndef __ASSETLOADER__
#define __ASSETLOADER__

#include <string>

class Traceable;
class TriangleMesh;

class AssetLoader {
public:
    AssetLoader() {}
    ~AssetLoader() {}
    
    TriangleMesh * load( const std::string & filename );

};


#endif
