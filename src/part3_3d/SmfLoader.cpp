#include "SmfLoader.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace {

long toZeroBased(long index, std::size_t vertexCount) {
    if (index > 0) return index - 1;
    if (index < 0) return (long)vertexCount + index;
    return -1;
}

void computeBounds(SmfModel& model) {
    Vec3 lo = model.vertices[0].position;
    Vec3 hi = lo;
    for (const MeshVertex& v : model.vertices) {
        lo = {std::min(lo.x, v.position.x), std::min(lo.y, v.position.y), std::min(lo.z, v.position.z)};
        hi = {std::max(hi.x, v.position.x), std::max(hi.y, v.position.y), std::max(hi.z, v.position.z)};
    }
    model.center = scale(add(lo, hi), 0.5f);

    model.radius = 0.0f;
    for (const MeshVertex& v : model.vertices) {
        Vec3 d = subtract(v.position, model.center);
        model.radius = std::max(model.radius, std::sqrt(dot(d, d)));
    }
    if (model.radius < 1e-6f) model.radius = 1.0f;
}

void computeVertexNormals(SmfModel& model) {
    for (MeshVertex& v : model.vertices) v.normal = {0.0f, 0.0f, 0.0f};

    for (std::size_t t = 0; t + 2 < model.indices.size(); t += 3) {
        MeshVertex& a = model.vertices[model.indices[t]];
        MeshVertex& b = model.vertices[model.indices[t + 1]];
        MeshVertex& c = model.vertices[model.indices[t + 2]];
        Vec3 n = cross(subtract(b.position, a.position), subtract(c.position, a.position));
        a.normal = add(a.normal, n);
        b.normal = add(b.normal, n);
        c.normal = add(c.normal, n);
    }

    for (MeshVertex& v : model.vertices) {
        if (dot(v.normal, v.normal) < 1e-20f) {
            v.normal = subtract(v.position, model.center);
        }
        v.normal = normalize(v.normal);
    }
}

}

bool loadSmf(const std::string& path, SmfModel& model, std::string& error) {
    std::ifstream file(path);
    if (!file) {
        error = "cannot open " + path;
        return false;
    }

    model = SmfModel();
    std::vector<std::vector<long>> faces;
    std::string line;
    int lineNumber = 0;
    int warnings = 0;

    while (std::getline(file, line)) {
        lineNumber++;
        std::istringstream in(line);
        std::string type;
        if (!(in >> type) || type[0] == '#') {
            continue;
        }

        if (type == "v") {
            Vec3 p;
            if (in >> p.x >> p.y >> p.z) {
                model.vertices.push_back({p, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f});
            } else {
                std::fprintf(stderr, "%s:%d: malformed vertex ignored\n", path.c_str(), lineNumber);
                warnings++;
            }
        } else if (type == "f") {
            std::vector<long> face;
            long index;
            while (in >> index) face.push_back(index);
            if (face.size() >= 3) {
                faces.push_back(face);
            } else {
                std::fprintf(stderr, "%s:%d: face with fewer than 3 indices ignored\n", path.c_str(), lineNumber);
                warnings++;
            }
        }
    }

    for (const std::vector<long>& face : faces) {
        std::vector<unsigned int> ids;
        for (long raw : face) {
            long id = toZeroBased(raw, model.vertices.size());
            if (id < 0 || id >= (long)model.vertices.size()) {
                ids.clear();
                break;
            }
            ids.push_back((unsigned int)id);
        }
        if (ids.empty()) {
            std::fprintf(stderr, "%s: face with out-of-range index ignored\n", path.c_str());
            warnings++;
            continue;
        }
        for (std::size_t k = 1; k + 1 < ids.size(); k++) {
            model.indices.insert(model.indices.end(), {ids[0], ids[k], ids[k + 1]});
        }
    }

    if (model.vertices.empty() || model.indices.empty()) {
        error = path + " contains no usable geometry";
        return false;
    }

    computeBounds(model);
    computeVertexNormals(model);

    std::printf("Loaded %s: %zu vertices, %zu triangles%s\n",
                path.c_str(), model.vertices.size(), model.indices.size() / 3,
                warnings ? " (with warnings)" : "");
    return true;
}
