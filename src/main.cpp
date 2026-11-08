#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#include "Camera.h"
#include "BezierPatch.h"
#include "Input.h"
#include "Renderer.h"

int main(int argc, char** argv) {
    static Camera camera;
    static BezierPatch patch;

    glutInit(&argc, argv);

    glutInitDisplayMode(
        GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH
    );

    glutInitWindowSize(900, 700);
    glutCreateWindow("Assignment 4 - Bezier Patch");

    Renderer::setupGL();

    Renderer::init(&camera, &patch);
    Input::init(&camera, &patch);

    glutDisplayFunc(Renderer::display);
    glutReshapeFunc(Renderer::reshape);

    glutKeyboardFunc(Input::keyboard);
    glutSpecialFunc(Input::specialKeys);

    glutMainLoop();
    return 0;
}
