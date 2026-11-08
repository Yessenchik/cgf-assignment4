#include "ShaderProgram.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

#ifndef ASSIGNMENT4_SHADER_DIR
#define ASSIGNMENT4_SHADER_DIR "shaders/"
#endif

#ifndef ASSIGNMENT4_MODEL_DIR
#define ASSIGNMENT4_MODEL_DIR "models/"
#endif

namespace {

bool readFile(const std::string& path, std::string& out) {
    std::ifstream file(path);
    if (!file) {
        std::fprintf(stderr, "Cannot open %s\n", path.c_str());
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    out = buffer.str();
    return true;
}

GLuint compile(GLenum type, const std::string& source, const std::string& name) {
    GLuint shader = glCreateShader(type);
    const char* text = source.c_str();
    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length + 1);
        glGetShaderInfoLog(shader, length, nullptr, log.data());
        std::fprintf(stderr, "Compile error in %s:\n%s\n", name.c_str(), log.data());
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

}

std::string shaderPath(const std::string& file) {
    return std::string(ASSIGNMENT4_SHADER_DIR) + file;
}

std::string modelPath(const std::string& file) {
    return std::string(ASSIGNMENT4_MODEL_DIR) + file;
}

bool ShaderProgram::load(const std::string& vertexFile, const std::string& fragmentFile) {
    destroy();

    std::string vertexSource, fragmentSource;
    if (!readFile(shaderPath(vertexFile), vertexSource) ||
        !readFile(shaderPath(fragmentFile), fragmentSource)) {
        return false;
    }

    GLuint vs = compile(GL_VERTEX_SHADER, vertexSource, vertexFile);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentSource, fragmentFile);
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return false;
    }

    program_ = glCreateProgram();
    glAttachShader(program_, vs);
    glAttachShader(program_, fs);
    glLinkProgram(program_);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint length = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length + 1);
        glGetProgramInfoLog(program_, length, nullptr, log.data());
        std::fprintf(stderr, "Link error (%s, %s):\n%s\n",
                     vertexFile.c_str(), fragmentFile.c_str(), log.data());
        destroy();
        return false;
    }
    return true;
}

void ShaderProgram::destroy() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
}

void ShaderProgram::use() const {
    glUseProgram(program_);
}

GLint ShaderProgram::location(const char* name) const {
    return glGetUniformLocation(program_, name);
}

void ShaderProgram::setMat4(const char* name, const Mat4& m) const {
    glUniformMatrix4fv(location(name), 1, GL_FALSE, m.data());
}

void ShaderProgram::setMat3(const char* name, const Mat3& m) const {
    glUniformMatrix3fv(location(name), 1, GL_FALSE, m.data());
}

void ShaderProgram::setVec3(const char* name, Vec3 v) const {
    glUniform3f(location(name), v.x, v.y, v.z);
}

void ShaderProgram::setFloat(const char* name, float value) const {
    glUniform1f(location(name), value);
}

void ShaderProgram::setInt(const char* name, int value) const {
    glUniform1i(location(name), value);
}
