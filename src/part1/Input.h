#pragma once

class Camera;
class BezierPatch;

namespace Input {

void init(Camera* camera, BezierPatch* patch);

void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);

}
