#include "Picking.h"

#include "Scene.h"

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <cstdio>

int encodeId(unsigned char r, unsigned char g, unsigned char b) {
    return r | (g << 8) | (b << 16);
}

bool PickingBuffer::resize(int width, int height) {
    destroy();

    width_ = width > 0 ? width : 1;
    height_ = height > 0 ? height : 1;

    glGenTextures(1, &colorTexture_);
    glBindTexture(GL_TEXTURE_2D, colorTexture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &depthBuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width_, height_);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture_, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::fprintf(stderr, "Picking framebuffer incomplete (0x%04X)\n", status);
        destroy();
        return false;
    }
    return true;
}

void PickingBuffer::destroy() {
    if (fbo_) glDeleteFramebuffers(1, &fbo_);
    if (depthBuffer_) glDeleteRenderbuffers(1, &depthBuffer_);
    if (colorTexture_) glDeleteTextures(1, &colorTexture_);
    fbo_ = depthBuffer_ = colorTexture_ = 0;
}

void PickingBuffer::render(const Scene& scene) {
    glViewport(0, 0, width_, height_);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_DITHER);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    loadViewMatrices(scene.camera(), width_, height_);

    for (const SceneObject& object : scene.objects()) {
        glColor3ub(object.id & 0xFF, (object.id >> 8) & 0xFF, (object.id >> 16) & 0xFF);
        drawObject(object, false);
    }
}

int PickingBuffer::pick(const Scene& scene, int x, int y) {
    if (!fbo_ || x < 0 || y < 0 || x >= width_ || y >= height_) {
        return -1;
    }

    GLint previousProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_VIEWPORT_BIT |
                 GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    render(scene);

    unsigned char pixel[4] = {0, 0, 0, 0};
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glReadBuffer(GL_BACK);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
    glUseProgram(previousProgram);

    return encodeId(pixel[0], pixel[1], pixel[2]);
}
