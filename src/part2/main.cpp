#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#include <cstring>

#include "Input.h"
#include "Picking.h"
#include "Renderer.h"
#include "Scene.h"
#include "SelfTest.h"

int main(int argc, char** argv) {
    static Scene scene;
    static PickingBuffer picker;

    glutInit(&argc, argv);
    bool selfTest = argc > 1 && std::strcmp(argv[1], "--self-test") == 0;

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Assignment 4 - Anti-aliasing and Picking");

    if (!Renderer::init(&scene)) {
        return 1;
    }

    Input::init(&scene, &picker);
    Input::printHelp();

    glutDisplayFunc(Renderer::display);
    glutReshapeFunc(Input::reshape);
    glutKeyboardFunc(Input::keyboard);
    glutSpecialFunc(Input::specialKeys);
    glutMouseFunc(Input::mouse);

    if (selfTest) {
        SelfTest::start(&scene);
    }

    glutMainLoop();
    return 0;
}
