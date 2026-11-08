#include "Renderer.h"

#include "Scene.h"

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#include <algorithm>
#include <cstdio>

namespace {

const char* kVertexShader = R"(
#version 120
varying vec3 vNormal;
varying vec3 vPosition;

void main() {
    vec4 eyePosition = gl_ModelViewMatrix * gl_Vertex;
    vPosition = eyePosition.xyz;
    vNormal = gl_NormalMatrix * gl_Normal;
    gl_Position = gl_ProjectionMatrix * eyePosition;
}
)";

// Phong shading: the normal is interpolated across each triangle and lit per fragment.
const char* kFragmentShader = R"(
#version 120
uniform vec3 uDiffuse;
uniform vec3 uLightPosition;
varying vec3 vNormal;
varying vec3 vPosition;

void main() {
    vec3 n = normalize(vNormal);
    vec3 l = normalize(uLightPosition - vPosition);
    vec3 v = normalize(-vPosition);
    vec3 h = normalize(l + v);

    float diffuse = max(dot(n, l), 0.0);
    float specular = diffuse > 0.0 ? pow(max(dot(n, h), 0.0), 48.0) : 0.0;

    vec3 color = uDiffuse * (0.15 + 0.85 * diffuse) + vec3(0.35) * specular;
    gl_FragColor = vec4(color, 1.0);
}
)";

// Light fixed in camera (eye) coordinates, slightly above and right of the eye.
const GLfloat kLightPosition[3] = {1.5f, 2.0f, 0.0f};
const GLfloat kBackground[4] = {0.08f, 0.08f, 0.10f, 1.0f};
constexpr int kPreferredSamples = 8;

const Scene* gScene = nullptr;

GLuint gProgram = 0;
GLint gDiffuseLocation = -1;
GLint gLightLocation = -1;

int gWidth = 1;
int gHeight = 1;

bool gMsaaSupported = false;
bool gAntiAliasing = true;
int gSamples = 0;
GLuint gMsaaFbo = 0;
GLuint gMsaaColor = 0;
GLuint gMsaaDepth = 0;

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::fprintf(stderr, "Shader compile error:\n%s\n", log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool buildProgram() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, kVertexShader);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, kFragmentShader);
    if (!vs || !fs) {
        return false;
    }

    gProgram = glCreateProgram();
    glAttachShader(gProgram, vs);
    glAttachShader(gProgram, fs);
    glLinkProgram(gProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = GL_FALSE;
    glGetProgramiv(gProgram, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(gProgram, sizeof(log), nullptr, log);
        std::fprintf(stderr, "Shader link error:\n%s\n", log);
        return false;
    }

    gDiffuseLocation = glGetUniformLocation(gProgram, "uDiffuse");
    gLightLocation = glGetUniformLocation(gProgram, "uLightPosition");
    return true;
}

void destroyMsaaBuffers() {
    if (gMsaaFbo) glDeleteFramebuffers(1, &gMsaaFbo);
    if (gMsaaColor) glDeleteRenderbuffers(1, &gMsaaColor);
    if (gMsaaDepth) glDeleteRenderbuffers(1, &gMsaaDepth);
    gMsaaFbo = gMsaaColor = gMsaaDepth = 0;
}

bool createMsaaBuffers(int width, int height) {
    destroyMsaaBuffers();

    glGenRenderbuffers(1, &gMsaaColor);
    glBindRenderbuffer(GL_RENDERBUFFER, gMsaaColor);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, gSamples, GL_RGBA8, width, height);

    glGenRenderbuffers(1, &gMsaaDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gMsaaDepth);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, gSamples, GL_DEPTH_COMPONENT24, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &gMsaaFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gMsaaFbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gMsaaColor);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gMsaaDepth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::fprintf(stderr, "Multisample framebuffer incomplete (0x%04X)\n", status);
        destroyMsaaBuffers();
        return false;
    }
    return true;
}

void drawObjects() {
    loadViewMatrices(gScene->camera(), gWidth, gHeight);

    glUseProgram(gProgram);
    glUniform3fv(gLightLocation, 1, kLightPosition);

    for (const SceneObject& object : gScene->objects()) {
        glUniform3f(gDiffuseLocation, object.diffuse.x, object.diffuse.y, object.diffuse.z);
        drawObject(object, true);
    }

    glUseProgram(0);
}

}

namespace Renderer {

bool init(const Scene* scene) {
    gScene = scene;

    std::printf("OpenGL %s, GLSL %s, %s\n",
                (const char*)glGetString(GL_VERSION),
                (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION),
                (const char*)glGetString(GL_RENDERER));

    if (!buildProgram()) {
        return false;
    }

    GLint maxSamples = 0;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    gSamples = std::min(kPreferredSamples, (int)maxSamples);
    gMsaaSupported = gSamples >= 2;
    gAntiAliasing = gMsaaSupported;

    if (gMsaaSupported) {
        std::printf("MSAA supported: up to %d samples, using %d\n", maxSamples, gSamples);
    } else {
        std::printf("MSAA not supported (GL_MAX_SAMPLES = %d)\n", maxSamples);
    }

    glEnable(GL_DEPTH_TEST);
    return true;
}

void shutdown() {
    destroyMsaaBuffers();
    if (gProgram) {
        glDeleteProgram(gProgram);
        gProgram = 0;
    }
}

void resize(int pixelWidth, int pixelHeight) {
    gWidth = std::max(1, pixelWidth);
    gHeight = std::max(1, pixelHeight);

    if (gMsaaSupported && !createMsaaBuffers(gWidth, gHeight)) {
        gMsaaSupported = false;
        gAntiAliasing = false;
        std::printf("Anti-aliasing disabled: multisample framebuffer unavailable\n");
    }
}

void drawFrame() {
    bool useMsaa = gAntiAliasing && gMsaaFbo != 0;

    glBindFramebuffer(GL_FRAMEBUFFER, useMsaa ? gMsaaFbo : 0);
    glViewport(0, 0, gWidth, gHeight);

    glClearColor(kBackground[0], kBackground[1], kBackground[2], kBackground[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (useMsaa) {
        glEnable(GL_MULTISAMPLE);
    } else {
        glDisable(GL_MULTISAMPLE);
    }

    drawObjects();

    if (useMsaa) {
        // Resolve the multisampled image into the GLUT window framebuffer.
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gMsaaFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, gWidth, gHeight, 0, 0, gWidth, gHeight,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void display() {
    drawFrame();
    glutSwapBuffers();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::fprintf(stderr, "OpenGL error 0x%04X\n", error);
    }
}

bool antiAliasingSupported() { return gMsaaSupported; }
bool antiAliasingEnabled() { return gAntiAliasing; }
int sampleCount() { return gSamples; }

void setAntiAliasing(bool enabled) {
    gAntiAliasing = enabled && gMsaaSupported;
}

}
