#pragma once

#include "GL3.h"

#include <vector>

struct RgbImage {
    int width = 0;
    int height = 0;
    std::vector<unsigned char> pixels;
};

RgbImage generatePatchImage(int size);

class Texture2D {
public:
    Texture2D() = default;
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    void upload(const RgbImage& image);
    void bind(int unit) const;
    void destroy();

private:
    GLuint id_ = 0;
};
