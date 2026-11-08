#pragma once

class Camera;

namespace Input {

void init(Camera* camera);
void printHelp();

void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);

}
