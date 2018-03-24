//
//  Image.h
//
//

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <vector>

#define MAGICKCORE_QUANTUM_DEPTH 16
#include <Magick++.h>

#include "OpenGLUtil.h"

template <typename PIXEL>
class Image
{
public:
    Image() = default;
    virtual ~Image() = default;

    const PIXEL * data() const { return &pixels[0]; }

    std::vector< PIXEL > pixels;

    unsigned int width = 0;
    unsigned int height = 0;
};

template <typename CHANNEL_TYPE>
struct RGBPixel {
    CHANNEL_TYPE r, g, b;
};

template <typename CHANNEL_TYPE>
class RGBImage : public Image< RGBPixel<CHANNEL_TYPE> >
{
public:
    RGBImage() = default;
    virtual ~RGBImage() = default;

    void loadImage( const std::string & filename );
    GLuint uploadGL();
};

#endif
