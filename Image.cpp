#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>
#include "Image.h"

template <>
void RGBImage<unsigned char>::loadImage( const std::string & filename )
{
    int w = 0, h = 0, numComponents = 3;
    unsigned char * stbiData = stbi_load(filename.c_str(), &w, &h, &numComponents, 3);
    width = w;
    height = h;

    if(stbiData) {
        printf("Loading image %s %u x %u\n", filename.c_str(), width, height);
    }
    else {
        fprintf(stderr, "Error loading %s\n", filename.c_str());
        return; // TODO - error handling
    }

    const unsigned int size = width * height;
    pixels.resize(size);

    for( unsigned int i = 0 ; i < size; i++ ) {
        pixels[i].r = stbiData[i * 3 + 0];
        pixels[i].g = stbiData[i * 3 + 1];
        pixels[i].b = stbiData[i * 3 + 2];
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
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    return texID;
}

