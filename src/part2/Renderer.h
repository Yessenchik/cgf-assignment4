#pragma once

class Scene;

namespace Renderer {

bool init(const Scene* scene);
void shutdown();

void resize(int pixelWidth, int pixelHeight);

void drawFrame();
void display();

bool antiAliasingSupported();
bool antiAliasingEnabled();
void setAntiAliasing(bool enabled);
int sampleCount();

}
