#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#include <cstdio>
#include <string>

#include "BezierPatch.h"
#include "Camera.h"
#include "Input.h"
#include "Renderer.h"

int main(int argc, char** argv) {
    static Camera camera;
    static BezierPatch patch;

    glutInit(&argc, argv);

    if (argc > 1) {
        std::string error;
        if (!patch.loadFromFile(argv[1], error)) {
            std::fprintf(stderr, "Failed to load control points: %s\n", error.c_str());
            return 1;
        }
        std::printf("Loaded control points from %s\n", argv[1]);
    } else {
        std::printf("Using built-in control points\n");
    }

    glutInitDisplayMode(
        GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH
    );

    glutInitWindowSize(900, 700);
    glutCreateWindow("Assignment 4 - Bezier Patch");

    Renderer::setupGL();

    Renderer::init(&camera, &patch);
    Input::init(&camera, &patch);
    Input::printHelp();

    glutDisplayFunc(Renderer::display);
    glutReshapeFunc(Renderer::reshape);

    glutKeyboardFunc(Input::keyboard);
    glutSpecialFunc(Input::specialKeys);

    glutMainLoop();
    return 0;
}
