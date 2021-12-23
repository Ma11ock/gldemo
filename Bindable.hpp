#ifndef BINDABLE_HPP
#define BINDABLE_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <exception>
#include <stdexcept>
#include <fmt/core.h>

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

using namespace std::string_view_literals;
class OpenGLException : public std::exception
{
public:
    OpenGLException(std::string_view what, std::string_view source,
                    std::string_view type)
        : mWhat(fmt::format("{} error in {}: \"{}\"", type, source, what)) 
    {
    }

    OpenGLException(std::string_view what,  std::string_view source,
                    std::string_view type, std::string_view command,
                    std::string_view file, int line)
        : mWhat(fmt::format("{} error in {} ({}:{} \"{}\"): {}",
                            type, source, file, line, command, what))
    {
    }

    const char *what() const throw()
    {
        return mWhat.c_str();
    }

    virtual ~OpenGLException() = default;

private:
    std::string mWhat;
};

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
