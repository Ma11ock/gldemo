#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iostream>
#include <GL/gl.h>
#include <regex>
#include <limits>
#include "loadobj.hpp"

using namespace std::string_literals;


// For each input vertex
//     Try to find a similar ( = same for all attributes ) vertex between all those we already output
//     If found :
//         A similar vertex is already in the VBO, use it instead !
//     If not found :
//         No similar vertex found, add it to the VBO
buffers buffers::createFromData(const std::vector<glm::vec3> &verts,
                                const std::vector<glm::vec2> &uvs,
                                const std::vector<glm::vec3> &normals)
{
    auto numVerts = verts.size();
    if(numVerts != uvs.size() || numVerts != normals.size())
        throw std::invalid_argument("Vector sized not the same.");

    // Length of the out* buffers.
    std::uint32_t outBufferLen = 0;
    // Out* buffers. 
    std::vector<glm::vec3> outVerts(numVerts);
    std::vector<glm::vec2> outUVs(numVerts);
    std::vector<glm::vec3> outNormals(numVerts);
    // Index buffer.
    std::vector<std::uint32_t> indices(numVerts);
    for(std::size_t i = 0; i < numVerts; i++)
    {
        auto &curVert = verts[i];
        auto &curUV = uvs[i];
        auto &curNorm = normals[i];

        auto vit = std::find(outVerts.begin(),
                             outVerts.begin() + outBufferLen, curVert);

        std::int64_t index = (vit != outVerts.end())
            ? std::distance(outVerts.begin(), vit) : -1;
        if(index >= 0 && outUVs[index] == curUV && outNormals[index] == curNorm)
            indices[i] = static_cast<std::uint32_t>(index);
        else
        {
            outVerts[outBufferLen] = curVert;
            outUVs[outBufferLen] = curUV;
            outNormals[outBufferLen] = curNorm;
            indices[i] = outBufferLen++;
        }
    }

    outVerts.resize(outBufferLen);
    outUVs.resize(outBufferLen);
    outNormals.resize(outBufferLen);
    return buffers{outVerts, outUVs, outNormals, indices};
}

static std::tuple<int, int, int> readTVN(const std::string &str)
{
    const static std::regex FREGEX("\\/|\\/\\/");
    
    std::vector<std::string> fRegex(
        std::sregex_token_iterator(str.begin(), str.end(), FREGEX, -1),
        std::sregex_token_iterator()
        );

    int v = 0;
    int t = 0;
    int n = 0;

    switch(fRegex.size())
    {
    case 0:
        throw std::invalid_argument("\""s + str + "\" is not a valid TVN string"s);
        break;
    case 1:
        break;
    case 2:
        if(str.find("//") != std::string::npos)
        {
            v = std::stoi(fRegex[0]);
            n = std::stoi(fRegex[1]);
        }
        else
        {
            v = std::stoi(fRegex[0]);
            t = std::stoi(fRegex[1]);
        }
    case 3:
        v = std::stoi(fRegex[0]);
        t = std::stoi(fRegex[1]);
        n = std::stoi(fRegex[2]);
        break;
    default:
        std::cerr << "More arguments in \"" << str
                  << "\" than required (" << fRegex.size()
                  << "), ignoring everything after the third argument\n";
        v = std::stoi(fRegex[0]);
        t = std::stoi(fRegex[1]);
        n = std::stoi(fRegex[2]);
        break;
    }

    return std::make_tuple(v, t, n);
}

buffers loadObjFile(const std::filesystem::path &path)
{
    const static std::regex OBJREGEX("\\s+|\\t+");

    std::cout << "Opening OBJ file at " << path << '\n';
    std::vector<glm::vec3> outVertices;
    std::vector<glm::vec2> outUV;
    std::vector<glm::vec3> outNormal;
    

    std::vector<std::int32_t> vertexIndices;
    std::vector<std::int32_t> uvIndices;
    std::vector<std::int32_t> normalIndices;

    std::vector<glm::vec3> tmpVertex;
    std::vector<glm::vec2> tmpUV;
    std::vector<glm::vec3> tmpNormal;

    std::ifstream objFile;

    objFile.open(path);
    if(!objFile)
        throw std::invalid_argument("Could not open "s + path.generic_string());

    std::string line;
    while(std::getline(objFile, line))
    {
        if(line == "" ||
           line[0] == '#' ||
           line[0] == 'm' ||
           line[0] == 'g' ||
           line[0] == 's' ||
           line[0] == 'u')
            continue;

        std::vector<std::string> lineStrings(
            std::sregex_token_iterator(line.begin(), line.end(), OBJREGEX, -1),
            std::sregex_token_iterator()
            );
        std::string &header = lineStrings[0];

        if(header == "v")
        {
            if(lineStrings.size() < 4)
                throw std::invalid_argument("Could not get valid vertex data from"s +
                                            path.generic_string());

            tmpVertex.push_back({
                    std::stof(lineStrings[1]),
                    std::stof(lineStrings[2]),
                    std::stof(lineStrings[3])
                });
        }
        else if(header == "vn")
        {
            if(lineStrings.size() < 4)
                throw std::invalid_argument("Could not get valid normal data from"s +
                                            path.generic_string());
            tmpNormal.push_back({
                    std::stof(lineStrings[1]),
                    std::stof(lineStrings[2]),
                    std::stof(lineStrings[3])
                });
        }
        else if(header == "vt")
        {
            if(lineStrings.size() < 3)
                throw std::invalid_argument("Could not get valid texture data from"s +
                                            path.generic_string());
            tmpUV.push_back({
                    std::stof(lineStrings[1]),
                    std::stof(lineStrings[2]),
                });
        }
        else if(header == "f")
        {
            if(lineStrings.size() < 4)
                throw std::invalid_argument("Could not get valid normal data from"s +
                                            path.generic_string());

            auto [vt1, tex1, n1] = readTVN(lineStrings[1]);
            for(std::size_t i = 1; i < lineStrings.size() - 2; i++)
            {
                auto [vti1, texi1, ni1] = readTVN(lineStrings[i + 1]);
                auto [vti2, texi2, ni2] = readTVN(lineStrings[i + 2]);

                vertexIndices.insert(vertexIndices.end(), {vt1, vti1, vti2});
                uvIndices.insert(uvIndices.end(), {tex1, texi1, texi2});
                normalIndices.insert(normalIndices.end(), {n1, ni1, ni2});
            }
        }

    }

    for(auto vertexIndex : vertexIndices)
    {
        if(vertexIndex < 0)
            vertexIndex += vertexIndices.size() + 1;
        outVertices.push_back(tmpVertex[vertexIndex - 1]);
    }
    for(auto uvIndex : uvIndices)
    {
        if(uvIndex < 0)
            uvIndex += uvIndices.size() + 1;
        outUV.push_back(tmpUV[uvIndex - 1]);
    }
    for(auto normalIndex : normalIndices)
    {
        if(normalIndex < 0)
            normalIndex += normalIndices.size() + 1;
        outNormal.push_back(tmpNormal[normalIndex - 1]);
    }

    return buffers::createFromData(outVertices, outUV, outNormal);
}

