#pragma once

#include <glad/glad.h>
#include <iostream>
#include <string>

// Forward declaration - we'll implement stb_image in a separate file
// Don't define STB_IMAGE_IMPLEMENTATION here
#include "stb_image.h"

class Texture {
public:
    unsigned int ID;
    int width, height, channels;

    Texture() : ID(0), width(0), height(0), channels(0) {}

    bool LoadFromFile(const std::string& path) {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        
        if (data) {
            GLenum format;
            if (channels == 1) format = GL_RED;
            else if (channels == 3) format = GL_RGB;
            else if (channels == 4) format = GL_RGBA;
            else format = GL_RGB;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            
            stbi_image_free(data);
            std::cout << "Texture loaded: " << path << " (" << width << "x" << height << ")" << std::endl;
            return true;
        } else {
            std::cout << "Failed to load texture: " << path << std::endl;
            stbi_image_free(data);
            return false;
        }
    }

    void Bind(unsigned int slot = 0) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    void Unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~Texture() {
        if (ID != 0) {
            glDeleteTextures(1, &ID);
        }
    }
};