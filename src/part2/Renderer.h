#pragma once

class Scene;

namespace Renderer {

bool init(const Scene* scene);
void shutdown();

// Framebuffer size in pixels; recreates the multisample buffers.
void resize(int pixelWidth, int pixelHeight);

// Renders one frame into the window back buffer without swapping.
void drawFrame();
void display();

bool antiAliasingSupported();
bool antiAliasingEnabled();
void setAntiAliasing(bool enabled);
int sampleCount();

}
