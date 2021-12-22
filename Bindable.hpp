#ifndef BINDABLE_HPP
#define BINDABLE_HPP

#include <cstdint>
#include <string>
#include <string_view>

#ifdef DEBUG

inline int curLine = 0;
inline std::string_view curFile;
inline std::string_view curGlCommand;

#define GLCall(line) line;                      \
    curLine = __LINE__;                         \
    curGlCommand = #line;                       \
    curFile = __FILE__; 
#else
#define GLCall(line) line;
#endif // DEBUG

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
