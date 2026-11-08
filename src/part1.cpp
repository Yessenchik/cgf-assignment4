
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>

float cameraYaw = 45.0f;
float cameraPitch = 35.0f;
float cameraDistance = 10.0f;

constexpr float PI = 3.14159265359f;

int selectedPoint = 0;
const float moveStep = 0.2f;

int resolution = 10;

void moveSelectedPoint(float dx, float dy, float dz);

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            std::exit(0);
            break;

        case 'r':
        case 'R':
            cameraYaw = 45.0f;
            cameraPitch = 35.0f;
            cameraDistance = 10.0f;
            break;

        case 9: // TAB
            selectedPoint = (selectedPoint + 1) % 16;
            break;

        case 'j':
        case 'J':
            moveSelectedPoint(-moveStep, 0, 0);
            break;

        case 'l':
        case 'L':
            moveSelectedPoint(moveStep, 0, 0);
            break;

        case 'i':
        case 'I':
            moveSelectedPoint(0, moveStep, 0);
            break;

        case 'k':
        case 'K':
            moveSelectedPoint(0, -moveStep, 0);
            break;

        case 'u':
        case 'U':
            moveSelectedPoint(0, 0, moveStep);
            break;

        case 'o':
        case 'O':
            moveSelectedPoint(0, 0, -moveStep);
            break;
        
        case '+':
        case '=':
            if (resolution < 50) {
                resolution += 1;
            }
            break;

        case '-':
        case '_':
            if (resolution > 2) {
                resolution -= 1;
            }
            break;
    }

    glutPostRedisplay();
}

struct Vec3 {
    float x, y, z;
};

Vec3 subtract(Vec3 a, Vec3 b) {
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

Vec3 cross(Vec3 a, Vec3 b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vec3 normalize(Vec3 v) {
    float length = std::sqrt(
        v.x * v.x +
        v.y * v.y +
        v.z * v.z
    );

    if (length < 0.00001f) {
        return {0.0f, 0.0f, 1.0f};
    }

    return {
        v.x / length,
        v.y / length,
        v.z / length
    };
}

float calculateDiffuse(Vec3 a, Vec3 b, Vec3 c) {
    Vec3 edge1 = subtract(b, a);
    Vec3 edge2 = subtract(c, a);

    Vec3 normal = normalize(cross(edge1, edge2));

    float yaw = cameraYaw * PI / 180.0f;
    float pitch = cameraPitch * PI / 180.0f;

    Vec3 lightDir = normalize({
        cos(pitch) * cos(yaw),
        cos(pitch) * sin(yaw),
        sin(pitch)
    });

    float diffuse =
        normal.x * lightDir.x +
        normal.y * lightDir.y +
        normal.z * lightDir.z;

    return 0.2f + 0.8f * std::fmax(0.0f, diffuse);
}

void drawShadedTriangle(Vec3 a, Vec3 b, Vec3 c) {
    float intensity = calculateDiffuse(a, b, c);

    glColor3f(
        0.2f * intensity,
        0.6f * intensity,
        0.9f * intensity
    );

    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
}

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

void moveSelectedPoint(float dx, float dy, float dz) {
    int row = selectedPoint / 4;
    int col = selectedPoint % 4;

    controlPoints[row][col].x += dx;
    controlPoints[row][col].y += dy;
    controlPoints[row][col].z += dz;

    glutPostRedisplay();
}

float bernstein(int i, float t) {
    float s = 1.0f - t;

    switch (i) {
        case 0: return s * s * s;
        case 1: return 3.0f * t * s * s;
        case 2: return 3.0f * t * t * s;
        case 3: return t * t * t;
        default: return 0.0f;
    }
}

Vec3 evaluateBezier(float u, float v) {
    Vec3 result = {0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float weight =
                bernstein(i, v) *
                bernstein(j, u);

            result.x += controlPoints[i][j].x * weight;
            result.y += controlPoints[i][j].y * weight;
            result.z += controlPoints[i][j].z * weight;
        }
    }

    return result;
}

void drawControlPoints() {
    glDisable(GL_DEPTH_TEST);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int index = i * 4 + j;
            bool selected = (index == selectedPoint);

            if (selected) {
                glPointSize(16.0f);
                glColor3f(1.0f, 0.2f, 0.2f);
            } else {
                glPointSize(10.0f);
                glColor3f(1.0f, 0.8f, 0.0f);
            }

            Vec3 p = controlPoints[i][j];

            glBegin(GL_POINTS);
            glVertex3f(p.x, p.y, p.z);
            glEnd();
        }
    }

    glEnable(GL_DEPTH_TEST);
}

void drawBezierPatch() {
    glBegin(GL_TRIANGLES);

    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            float u0 = (float)j / resolution;
            float u1 = (float)(j + 1) / resolution;

            float v0 = (float)i / resolution;
            float v1 = (float)(i + 1) / resolution;

            Vec3 p00 = evaluateBezier(u0, v0);
            Vec3 p10 = evaluateBezier(u1, v0);
            Vec3 p01 = evaluateBezier(u0, v1);
            Vec3 p11 = evaluateBezier(u1, v1);

            drawShadedTriangle(p00, p10, p11);
            drawShadedTriangle(p00, p11, p01);
        }
    }

    glEnd();
}

void setupCamera() {
    float yaw = cameraYaw * PI / 180.0f;
    float pitch = cameraPitch * PI / 180.0f;

    float x = cameraDistance * cos(pitch) * cos(yaw);
    float y = cameraDistance * cos(pitch) * sin(yaw);
    float z = cameraDistance * sin(pitch);

    gluLookAt(
        x, y, z,
        0.0, 0.0, 0.0,
        0.0, 0.0, 1.0
    );
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    setupCamera();

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
    
    drawBezierPatch();
    
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

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            cameraYaw -= 5.0f;
            break;

        case GLUT_KEY_RIGHT:
            cameraYaw += 5.0f;
            break;

        case GLUT_KEY_UP:
            cameraPitch += 5.0f;
            break;

        case GLUT_KEY_DOWN:
            cameraPitch -= 5.0f;
            break;
    }

    // Prevent camera from flipping upside down
    if (cameraPitch > 85.0f) cameraPitch = 85.0f;
    if (cameraPitch < -85.0f) cameraPitch = -85.0f;

    glutPostRedisplay();
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
    
    glutKeyboardFunc(keyboard);
    
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}

