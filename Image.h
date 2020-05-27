//
//  Image.h
//
//

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <vector>

#include "OpenGLUtil.h"

class ImageBase
{
public:
    ImageBase() = default;
    virtual ~ImageBase() = default;

    virtual GLuint uploadGL() = 0;

    unsigned int width = 0;
    unsigned int height = 0;

protected:
    GLuint uploadGLImpl(GLint internalFormat, GLenum format, GLenum type,
                        const void * data);
};

std::shared_ptr<ImageBase> loadImage(const std::string & filename);

template <typename PIXEL>
class Image : public ImageBase
{
public:
    Image() = default;
    virtual ~Image() = default;

    const PIXEL * data() const { return pixels.data(); }

    std::vector< PIXEL > pixels;
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

    virtual GLuint uploadGL() override;
};

template <typename CHANNEL_TYPE>
struct RGBAPixel {
    CHANNEL_TYPE r, g, b, a;
};

template <typename CHANNEL_TYPE>
class RGBAImage : public Image< RGBAPixel<CHANNEL_TYPE> >
{
public:
    RGBAImage() = default;
    virtual ~RGBAImage() = default;

    virtual GLuint uploadGL() override;
};



#endif
