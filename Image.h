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

template <>
void RGBImage<unsigned char>::loadImage( const std::string & filename )
{
    Magick::Image magImage;

    magImage.read( filename );

    width = magImage.columns();
    height = magImage.rows();
    printf("Loading image %s %u x %u\n", filename.c_str(), width, height);

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

template <>
GLuint RGBImage<unsigned char>::uploadGL()
{
    GLuint texID = 0;
    glGenTextures( 1, &texID );
    glBindTexture( GL_TEXTURE_2D, texID );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height,
                  0, GL_RGB, GL_UNSIGNED_BYTE, data() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    return texID;
}

#endif
