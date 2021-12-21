#ifndef CONSTANT_BUFFER_HPP
#define CONSTANT_BUFFER_HPP

#include "Bindable.hpp"
#include <vector>
#include <GL/glew.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <tuple>

// Constant buffer for vertices, texture coordinates, and normals.
class ConstantBuffer : public Bindable
{
public:
    template<class T>
    ConstantBuffer(const std::vector<T> &vertices, std::uint32_t index,
                   std::uint32_t vaoId, std::int32_t numElems, 
                   GLenum underlyingType = GL_FLOAT, bool normalize = false)
        : Bindable(),mCountVertices(vertices.size()),mTypeSize(sizeof(T))
    {

        glCreateBuffers(1, &mId);
        glNamedBufferStorage(mId, sizeof(T) * vertices.size(),
                             vertices.data(), GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(vaoId, index, mId, 0, sizeof(T)); 

        glEnableVertexArrayAttrib(vaoId, index);
        glVertexArrayAttribFormat(vaoId, index, numElems, underlyingType,
                                  normalize, 0);
        glVertexArrayAttribBinding(vaoId, index, index);
    }
    
    ConstantBuffer(const std::vector<glm::vec4> &vertices, std::uint32_t index,
                   std::uint32_t vaoId)
        : ConstantBuffer(vertices, index, vaoId, 4)
    {
    }

    ConstantBuffer(const std::vector<glm::vec3> &vertices, std::uint32_t index,
                   std::uint32_t vaoId)
        : ConstantBuffer(vertices, index, vaoId, 3)
    {
    }

    ConstantBuffer(const std::vector<glm::vec2> &vertices, std::uint32_t index,
                   std::uint32_t vaoId)
        : ConstantBuffer(vertices, index, vaoId, 2)
    {
    }
    
    ConstantBuffer(const std::vector<float> &vertices, std::uint32_t index,
                   std::int32_t numElems, std::uint32_t vaoId, bool normalize = false)
        : ConstantBuffer(vertices, index, numElems, vaoId, GL_FLOAT, normalize)
    {
    }

    ConstantBuffer(const std::vector<std::tuple<glm::vec3, glm::vec2, glm::vec3>> &vertices,
                   std::uint32_t positionIndex, std::uint32_t texIndex,
                   std::uint32_t normalIndex, bool normalizePos = false,
                   bool normaizeTexs = false, bool normalizeNorms = false)
    {
        glGenBuffers(1, &mId);
        glEnableVertexAttribArray(positionIndex);
        glEnableVertexAttribArray(texIndex);
        glEnableVertexAttribArray(normalIndex);

        static const GLvoid *vertexPositionOffset = nullptr;
        static const GLvoid *vertexTexOffset = reinterpret_cast<GLvoid*>(3 * sizeof(float));
        static const GLvoid *vertexNormalOffset = reinterpret_cast<GLvoid*>(5 * sizeof(float));

        glBindBuffer(GL_ARRAY_BUFFER, mId);

        glVertexAttribPointer(positionIndex, 3, GL_FLOAT, normalizePos,
                              0, vertexPositionOffset);
        glVertexAttribPointer(texIndex, 2, GL_FLOAT, normaizeTexs,
                              0, vertexTexOffset);
        glVertexAttribPointer(normalIndex, 3, GL_FLOAT, normalizeNorms,
                              0, vertexNormalOffset);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() *
                     sizeof(std::tuple<glm::vec3, glm::vec2, glm::vec3>),
                     vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // ConstantBuffer(const std::vector<std::tuple<glm::vec3, glm::vec2, glm::vec3>> &attributes,
    //                std::uint32_t vaoID, std::uint32_t vertexIndex,
    //                std::uint32_t texIndex, std::uint32_t normalIndex,
    //                bool normalized = false)
    // {
        
    // }

    virtual void bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, mId);
    }

    virtual void unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    virtual ~ConstantBuffer() = default;

protected:
    std::uint32_t mCountVertices;
    std::uint32_t mTypeSize;
};

#endif /* CONSTANT_BUFFER_HPP */
