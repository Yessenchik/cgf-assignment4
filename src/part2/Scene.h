#pragma once

#include "Camera.h"
#include "Mesh.h"

#include <random>
#include <string>
#include <vector>

struct SceneObject {
    int id;
    std::string name;
    Mesh mesh;
    Vec3 position;
    float tiltDeg;
    Vec3 diffuse;
    Vec3 probePoint;
};

class Scene {
public:
    static constexpr int kBackgroundId = 0;

    Scene();

    Camera& camera() { return camera_; }
    const Camera& camera() const { return camera_; }

    const std::vector<SceneObject>& objects() const { return objects_; }
    SceneObject* findById(int id);

    Vec3 randomizeColor(SceneObject& object);

private:
    Camera camera_;
    std::vector<SceneObject> objects_;
    std::mt19937 rng_;
};

void loadViewMatrices(const Camera& camera, int width, int height);
void drawObject(const SceneObject& object, bool withNormals);
