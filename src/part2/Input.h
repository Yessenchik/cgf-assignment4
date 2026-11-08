#pragma once

class Scene;
class PickingBuffer;

namespace Input {

void init(Scene* scene, PickingBuffer* picker);
void printHelp();
void shutdown();

// Picks at a window position (GLUT coordinates, origin top-left) and recolors the hit object.
int clickAt(int windowX, int windowY);

int windowWidth();
int windowHeight();
float pixelScale();

void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouse(int button, int state, int x, int y);

}
