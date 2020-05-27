#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>
#include "Image.h"

std::shared_ptr<ImageBase> loadImage(const std::string & filename)
{
    int w = 0, h = 0, numComponents = STBI_rgb_alpha;
    unsigned char * stbiData = stbi_load(filename.c_str(), &w, &h, &numComponents, 0);

    if(!stbiData) {
        throw std::runtime_error(std::string("Error loading image: ") + filename);
    }

    printf("NEW Loading image %s %u x %u (%d comp)\n", filename.c_str(), w, h, numComponents);

    const unsigned int size = w * h;

    if(numComponents == 3) {
        auto image = std::make_shared<RGBImage<uint8_t>>();
        image->width = w;
        image->height = h;

        auto & pixels = image->pixels;
        pixels.resize(size);

        for( unsigned int i = 0 ; i < size; i++ ) {
            pixels[i].r = stbiData[i * 3 + 0];
            pixels[i].g = stbiData[i * 3 + 1];
            pixels[i].b = stbiData[i * 3 + 2];
        }

        stbi_image_free(stbiData);
        return image;
    }
    else if(numComponents == 4) {
        auto image = std::make_shared<RGBAImage<uint8_t>>();
        image->width = w;
        image->height = h;

        auto & pixels = image->pixels;
        pixels.resize(size);

        for( unsigned int i = 0 ; i < size; i++ ) {
            pixels[i].r = stbiData[i * 4 + 0];
            pixels[i].g = stbiData[i * 4 + 1];
            pixels[i].b = stbiData[i * 4 + 2];
            pixels[i].a = stbiData[i * 4 + 3];
        }

        stbi_image_free(stbiData);
        return image;
    }
    else if(numComponents == 1) {
        auto image = std::make_shared<RGBImage<uint8_t>>();
        image->width = w;
        image->height = h;

        auto & pixels = image->pixels;
        pixels.resize(size);

        // Convert to RGB
        for( unsigned int i = 0 ; i < size; i++ ) {
            auto value = stbiData[i];
            pixels[i].r = value;
            pixels[i].g = value;
            pixels[i].b = value;
        }

        stbi_image_free(stbiData);
        return image;
    }
    else {
        stbi_image_free(stbiData);
        throw std::runtime_error(std::string("Unsupported number of image channels: ") + std::to_string(numComponents));
    }
}

GLuint ImageBase::uploadGLImpl(GLint internalFormat, GLenum format, GLenum type,
                               const void * data)
{
    GLuint texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height,
                  0, format, type, data);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return texID;
}

template <>
GLuint RGBImage<unsigned char>::uploadGL()
{
    return ImageBase::uploadGLImpl(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, data());
}

template <>
GLuint RGBAImage<unsigned char>::uploadGL()
{
    return ImageBase::uploadGLImpl(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data());
}

