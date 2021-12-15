#ifndef LOADOBJ_H
#define LOADOBJ_H

#include <vector>
#include <tuple>
#include <cstdint>
#include <filesystem>
#include <glm/glm.hpp>

struct buffers
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texUVs;
    std::vector<glm::vec3> normals;
    std::vector<std::uint32_t> indices;

    static buffers createFromData(const std::vector<glm::vec3> &verts,
                                  const std::vector<glm::vec2> &uvs,
                                  const std::vector<glm::vec3> &normals);
};

buffers
loadObjFile(const std::filesystem::path &path);

#endif /* LOADOBJ_H */
