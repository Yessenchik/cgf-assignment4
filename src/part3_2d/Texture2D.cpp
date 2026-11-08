#include "Texture2D.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr float PI = 3.14159265359f;

struct Color { float r, g, b; };

Color mix(Color a, Color b, float t) {
    return {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t};
}

float coverage(float d) {
    return std::clamp(0.5f - d, 0.0f, 1.0f);
}

}

RgbImage generatePatchImage(int size) {
    RgbImage image;
    image.width = image.height = size;
    image.pixels.resize(size * size * 3);

    const Color deepBlue = {0.10f, 0.20f, 0.45f};
    const Color teal = {0.10f, 0.55f, 0.60f};
    const Color violet = {0.45f, 0.20f, 0.55f};
    const Color cream = {0.98f, 0.93f, 0.80f};
    const Color orange = {0.95f, 0.55f, 0.15f};
    const Color crimson = {0.80f, 0.12f, 0.22f};
    const Color corners[4] = {
        {0.90f, 0.15f, 0.15f},
        {0.15f, 0.80f, 0.25f},
        {0.20f, 0.35f, 0.95f},
        {0.98f, 0.85f, 0.10f},
    };

    float px = 1.0f / size;

    for (int y = 0; y < size; y++) {
        float v = (y + 0.5f) * px;
        for (int x = 0; x < size; x++) {
            float u = (x + 0.5f) * px;

            Color c = mix(mix(deepBlue, teal, u), mix(violet, deepBlue, u), v);

            float gu = std::fabs(u * 8.0f - std::round(u * 8.0f)) / 8.0f;
            float gv = std::fabs(v * 8.0f - std::round(v * 8.0f)) / 8.0f;
            float grid = coverage((std::min(gu, gv) - 1.5f * px) / px);
            c = mix(c, cream, 0.55f * grid);

            float dx = u - 0.5f, dy = v - 0.5f;
            float r = std::sqrt(dx * dx + dy * dy);
            float angle = std::atan2(dy, dx);
            float petal = 0.30f + 0.06f * std::cos(8.0f * angle);
            float inside = coverage((r - petal) / px);
            float band = 0.5f + 0.5f * std::cos(r * 2.0f * PI * 9.0f);
            Color rosette = mix(mix(crimson, orange, band), cream, coverage((r - 0.06f) / px));
            c = mix(c, rosette, inside);
            c = mix(c, cream, coverage((std::fabs(r - petal) - 1.5f * px) / px));

            for (int k = 0; k < 4; k++) {
                float cu = (k & 1) ? 0.9f : 0.1f;
                float cv = (k & 2) ? 0.9f : 0.1f;
                float d = std::sqrt((u - cu) * (u - cu) + (v - cv) * (v - cv));
                c = mix(c, cream, coverage((d - 0.065f) / px));
                c = mix(c, corners[k], coverage((d - 0.05f) / px));
            }

            unsigned char* out = &image.pixels[(y * size + x) * 3];
            out[0] = (unsigned char)std::lround(std::clamp(c.r, 0.0f, 1.0f) * 255.0f);
            out[1] = (unsigned char)std::lround(std::clamp(c.g, 0.0f, 1.0f) * 255.0f);
            out[2] = (unsigned char)std::lround(std::clamp(c.b, 0.0f, 1.0f) * 255.0f);
        }
    }

    return image;
}

void Texture2D::upload(const RgbImage& image) {
    destroy();

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image.width, image.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image.pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::bind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture2D::destroy() {
    if (id_) {
        glDeleteTextures(1, &id_);
        id_ = 0;
    }
}
