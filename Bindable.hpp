#ifndef BINDABLE_HPP
#define BINDABLE_HPP

#include <cstdint>

class Bindable
{
    friend class Drawable;
public:
    Bindable() : mId(0)
    {
    }
    virtual ~Bindable() = default;

    virtual void bind() = 0;
    virtual void unbind() = 0;
protected:
    std::uint32_t mId;
};

#endif /* BINDABLE_HPP */
