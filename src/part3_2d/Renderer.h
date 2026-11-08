#pragma once

class Camera;

namespace Renderer {

constexpr int kResolution = 12;

bool init(const Camera* camera);
void shutdown();

void reshape(int width, int height);
void drawFrame();
void display();

bool toggleWireframe();

}
