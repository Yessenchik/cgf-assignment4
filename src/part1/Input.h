#pragma once

class Camera;
class BezierPatch;

namespace Input {

// Registers the objects that keyboard input mutates. The objects are owned
// by the caller (main) and must outlive the GLUT main loop.
void init(Camera* camera, BezierPatch* patch);

// GLUT callbacks.
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);

}
