#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include <memory>
#include <cstdint>

#include "Bindable.hpp"


class Drawable 
{
public:
    Drawable() = default;
    Drawable(const Drawable &) = delete;
    virtual ~Drawable() = default;
    virtual void draw() const = 0;
protected:
    std::uint32_t getBindableId(const Bindable *bindable) const
    {
        return bindable->mId;
    }
};

#endif /* DRAWABLE_HPP */
