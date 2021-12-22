#ifndef CONSTANT_BUFFER_HPP
#define CONSTANT_BUFFER_HPP

#include "Bindable.hpp"
#include <vector>
#include <GL/glew.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <cstddef>

struct interleavedType
{
    glm::vec3 vertexCoords;
    glm::vec2 texCoords;
    glm::vec3 normalCoords;
};

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

        GLCall(glCreateBuffers(1, &mId));
        GLCall(glNamedBufferStorage(mId, sizeof(T) * vertices.size(),
                                    vertices.data(), GL_DYNAMIC_STORAGE_BIT));
        GLCall(glVertexArrayVertexBuffer(vaoId, index, mId, 0, sizeof(T)););

        GLCall(glEnableVertexArrayAttrib(vaoId, index));
        GLCall(glVertexArrayAttribFormat(vaoId, index, numElems, underlyingType,
                                         normalize, 0));
        GLCall(glVertexArrayAttribBinding(vaoId, index, index));
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

    ConstantBuffer(const std::vector<interleavedType> &vertices,
                   std::uint32_t index, std::uint32_t vaoId, bool normalizePos = false,
                   bool normaizeTexs = false, bool normalizeNorms = false)
    {
        std::uint32_t positionIndex = index;
        std::uint32_t texIndex = positionIndex + 1;
        std::uint32_t normalIndex = positionIndex + 2;
        
        GLCall(glCreateBuffers(1, &mId));
        GLCall(glNamedBufferStorage(mId, sizeof(interleavedType) * vertices.size(),
                                    vertices.data(), GL_DYNAMIC_STORAGE_BIT));
        // First 0, might be something else.
        GLCall(glVertexArrayVertexBuffer(vaoId, positionIndex, mId, 0, sizeof(interleavedType)));

        GLCall(glEnableVertexArrayAttrib(vaoId, positionIndex));
        GLCall(glEnableVertexArrayAttrib(vaoId, texIndex));
        GLCall(glEnableVertexArrayAttrib(vaoId, normalIndex));

        GLCall(glVertexArrayAttribFormat(vaoId,
                                         positionIndex,
                                         glm::vec3::length(),
                                         GL_FLOAT,
                                         normalizePos,
                                         offsetof(interleavedType, vertexCoords)));

        GLCall(glVertexArrayAttribFormat(vaoId, texIndex, glm::vec2::length(), GL_FLOAT,
                                         normaizeTexs, offsetof(interleavedType, texCoords)));
        GLCall(glVertexArrayAttribFormat(vaoId, normalIndex, glm::vec3::length(), GL_FLOAT,
                                         normalizeNorms, offsetof(interleavedType, normalCoords)));

        GLCall(glVertexArrayAttribBinding(vaoId, positionIndex, 0));
        GLCall(glVertexArrayAttribBinding(vaoId, texIndex, 0));
        GLCall(glVertexArrayAttribBinding(vaoId, normalIndex, 0));
    }

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
