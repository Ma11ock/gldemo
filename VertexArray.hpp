#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

#include "Drawable.hpp"
#include "IndexBuffer.hpp"
#include "ConstantBuffer.hpp"

#include <memory>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

class VertexArray : public Drawable
{
public:
    VertexArray(const std::vector<glm::vec3> &vertices,
                const std::vector<glm::vec2> &texCoords,
                const std::vector<glm::vec3> &normals,
                const std::vector<std::uint32_t> &indices)
    {
        // TODO vertex array should be bindable

        glGenVertexArrays(1, &mId);
        glBindVertexArray(mId);

        mVertexBuffer = std::make_unique<ConstantBuffer>(vertices, 0);
        mTextureBuffer = std::make_unique<ConstantBuffer>(texCoords, 1);
        mNormalBuffer = std::make_unique<ConstantBuffer>(normals, 2);
        mIndexBuffer = std::make_unique<IndexBuffer>(indices);

        glBindVertexArray(0);
    }

    VertexArray() = default;

    virtual void draw() const
    {
        glBindVertexArray(mId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                     getBindableId(mIndexBuffer.get()));
        glDrawElements(GL_TRIANGLES, mIndexBuffer->getNumIndices(),
                       GL_UNSIGNED_INT, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    virtual ~VertexArray() = default;
protected:
    std::unique_ptr<ConstantBuffer> mVertexBuffer;
    std::unique_ptr<ConstantBuffer> mTextureBuffer;
    std::unique_ptr<ConstantBuffer> mNormalBuffer;
    std::unique_ptr<IndexBuffer> mIndexBuffer;
    GLuint mId;
};

#endif /* VERTEX_ARRAY_HPP */
