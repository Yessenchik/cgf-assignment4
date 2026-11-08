#pragma once

class Camera;
class BezierPatch;

namespace Renderer {

// Registers the scene to draw. The objects are owned by the caller (main)
// and must outlive the GLUT main loop.
void init(const Camera* camera, const BezierPatch* patch);

// One-time GL state setup (depth test, clear color).
void setupGL();

// GLUT callbacks.
void display();
void reshape(int width, int height);

}
