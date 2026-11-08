
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>


struct Vec3 {
    float x, y, z;
};

Vec3 controlPoints[4][4] = {
    {
        {-1.5f, -1.5f, 0.0f},
        {-0.5f, -1.5f, 0.0f},
        { 0.5f, -1.5f, 0.0f},
        { 1.5f, -1.5f, 0.0f}
    },
    {
        {-1.5f, -0.5f, 0.0f},
        {-0.5f, -0.5f, 1.5f},
        { 0.5f, -0.5f, 1.5f},
        { 1.5f, -0.5f, 0.0f}
    },
    {
        {-1.5f,  0.5f, 0.0f},
        {-0.5f,  0.5f, 1.5f},
        { 0.5f,  0.5f, 1.5f},
        { 1.5f,  0.5f, 0.0f}
    },
    {
        {-1.5f,  1.5f, 0.0f},
        {-0.5f,  1.5f, 0.0f},
        { 0.5f,  1.5f, 0.0f},
        { 1.5f,  1.5f, 0.0f}
    }
};

void drawControlPoints() {
    glPointSize(10.0f);

    glColor3f(1.0f, 0.8f, 0.0f);

    glBegin(GL_POINTS);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            Vec3 p = controlPoints[i][j];
            glVertex3f(p.x, p.y, p.z);
        }
    }

    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        5.0, 5.0, 7.0,  // Camera position
        0.0, 0.0, 0.0,  // Look at origin
        0.0, 0.0, 1.0   // Z is up
    );

    // Draw coordinate axes
    glBegin(GL_LINES);

    // X axis - Red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0);

    // Y axis - Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0);

    // Z axis - Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2);

    glEnd();
    
    drawControlPoints();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    if (height == 0) height = 1;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(
        45.0,
        static_cast<double>(width) / height,
        0.1,
        100.0
    );

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(
        GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH
    );

    glutInitWindowSize(900, 700);
    glutCreateWindow("Assignment 4 - Bezier Patch");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}

