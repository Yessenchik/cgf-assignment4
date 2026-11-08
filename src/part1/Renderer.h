#pragma once

class Camera;
class BezierPatch;

namespace Renderer {

void init(const Camera* camera, const BezierPatch* patch);

void setupGL();

void display();
void reshape(int width, int height);

}
