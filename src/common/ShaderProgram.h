#pragma once

#include "GL3.h"
#include "Mat4.h"

#include <string>

class ShaderProgram {
public:
    ShaderProgram() = default;
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    bool load(const std::string& vertexFile, const std::string& fragmentFile);
    void destroy();
    void use() const;

    void setMat4(const char* name, const Mat4& m) const;
    void setMat3(const char* name, const Mat3& m) const;
    void setVec3(const char* name, Vec3 v) const;
    void setFloat(const char* name, float value) const;
    void setInt(const char* name, int value) const;

private:
    GLint location(const char* name) const;

    GLuint program_ = 0;
};

std::string shaderPath(const std::string& file);
std::string modelPath(const std::string& file);
