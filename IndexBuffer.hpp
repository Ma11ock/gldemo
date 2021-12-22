#ifndef INDEX_BUFFER_HPP
#define INDEX_BUFFER_HPP

#include "Bindable.hpp"
#include <vector>
#include <cstdint>
#include <type_traits>

#include <GL/glew.h>

class IndexBuffer : public Bindable
{
public:
    IndexBuffer()
        : Bindable(),mCountIndices(0),mTypeSize(0)
    {
    }

    IndexBuffer(const std::vector<std::uint8_t> &indices)
        : Bindable(),mCountIndices(indices.size()),mTypeSize(1),
          mUnderlyingType(GL_UNSIGNED_BYTE)
    {
        GLCall(glGenBuffers(1, &mId));

        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mCountIndices,
                            indices.data(), GL_STATIC_DRAW));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    IndexBuffer(const std::vector<std::uint16_t> &indices)
        : Bindable(),mCountIndices(indices.size()),mTypeSize(sizeof(std::uint16_t)),
          mUnderlyingType(GL_UNSIGNED_SHORT)
    {
        GLCall(glGenBuffers(1, &mId));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mTypeSize * mCountIndices,
                            indices.data(), GL_STATIC_DRAW));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    IndexBuffer(const std::vector<std::uint32_t> &indices)
        : Bindable(),mCountIndices(indices.size()),mTypeSize(sizeof(std::uint32_t)),
          mUnderlyingType(GL_UNSIGNED_INT)
    {
        GLCall(glCreateBuffers(1, &mId));
        GLCall(glNamedBufferStorage(mId, mTypeSize * indices.size(), indices.data(),
                                    GL_DYNAMIC_STORAGE_BIT)); 
        
    }

    virtual void bind()
    {
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId));
    }

    virtual void unbind()
    {
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    std::size_t getNumIndices() const
    {
        return mCountIndices;
    }

    std::size_t getTypeSize() const
    {
        return mTypeSize;
    }

    GLenum getUnderlyingType() const
    {
        return mUnderlyingType;
    }

    virtual ~IndexBuffer() = default;

protected:
    std::size_t mCountIndices;
    std::size_t mTypeSize;
    GLenum mUnderlyingType;
};

#endif /* INDEX_BUFFER_HPP */
