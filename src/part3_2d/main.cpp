#include "Camera.h"
#include "GL3.h"
#include "Input.h"
#include "Renderer.h"

int main(int argc, char** argv) {
    static Camera camera(-60.0f, 50.0f, 7.0f);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Assignment 4 - Part 3A: 2D Texture on Bezier Patch");

    if (!Renderer::init(&camera)) {
        return 1;
    }

    Input::init(&camera);
    Input::printHelp();

    glutDisplayFunc(Renderer::display);
    glutReshapeFunc(Renderer::reshape);
    glutKeyboardFunc(Input::keyboard);
    glutSpecialFunc(Input::specialKeys);

    glutMainLoop();
    return 0;
}
