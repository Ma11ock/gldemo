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

    template<class T>
    IndexBuffer(const std::vector<T> &indices)
        : Bindable(),mCountIndices(indices.size()),mTypeSize(sizeof(T)),
          mUnderlyingType(GL_UNSIGNED_INT)
    {
        static_assert(std::is_unsigned_v<T> && sizeof(T) <= sizeof(std::uint32_t),
                      "Index buffer type must be unsigned int and no greater than 32 bits.");

        glGenBuffers(1, &mId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(T) * mCountIndices,
                     indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    virtual void bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
    }

    virtual void unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
