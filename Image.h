//
//  Image.h
//
//

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <vector>

#define MAGICKCORE_QUANTUM_DEPTH 16
#include <Magick++.h>

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
};

template <>
void RGBImage<unsigned char>::loadImage( const std::string & filename )
{
    Magick::Image magImage;

    magImage.read( filename );

    width = magImage.columns();
    height = magImage.rows();

    unsigned int size = width * height;
    pixels.resize( size );

    Magick::PixelPacket * magData = magImage.getPixels( 0, 0, width, height );

    // NOTE: We're shifting by 8 bits because we are using a 16 bit quantum with ImageMagick
    for( unsigned int i = 0 ; i < size; i++ ) {
        pixels[i].r = magData[i].red >> 8;
        pixels[i].g = magData[i].green >> 8;
        pixels[i].b = magData[i].blue >> 8;
    }
}

#endif
