#include "Camera.h"
#include "GL3.h"
#include "Input.h"
#include "Renderer.h"
#include "ShaderProgram.h"

#include <string>

int main(int argc, char** argv) {
    static Camera camera(-70.0f, 15.0f, 3.4f);

    glutInit(&argc, argv);
    std::string modelFile = argc > 1 ? argv[1] : modelPath("bound-bunny_200.smf");

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Assignment 4 - Part 3B: 3D Procedural Texture");

    if (!Renderer::init(&camera, modelFile)) {
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
