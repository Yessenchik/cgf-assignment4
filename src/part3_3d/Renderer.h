#pragma once

#include <string>

class Camera;

namespace Renderer {

bool init(const Camera* camera, const std::string& modelFile);
void shutdown();

void reshape(int width, int height);
void drawFrame();
void display();

bool toggleWireframe();

}
