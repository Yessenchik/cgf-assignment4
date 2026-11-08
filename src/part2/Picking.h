#pragma once

class Scene;

class PickingBuffer {
public:
    PickingBuffer() = default;
    PickingBuffer(const PickingBuffer&) = delete;
    PickingBuffer& operator=(const PickingBuffer&) = delete;

    bool resize(int width, int height);
    void destroy();

    int pick(const Scene& scene, int x, int y);

    bool valid() const { return fbo_ != 0; }
    int width() const { return width_; }
    int height() const { return height_; }

private:
    void render(const Scene& scene);

    unsigned int fbo_ = 0;
    unsigned int colorTexture_ = 0;
    unsigned int depthBuffer_ = 0;
    int width_ = 0;
    int height_ = 0;
};

int encodeId(unsigned char r, unsigned char g, unsigned char b);
