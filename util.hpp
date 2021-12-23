/**
 * @brief Utility library, adds functions and features not present in the stdlib.
 * @author Ryan Jeffrey <ryan@ryanmj.xyz>
 * @date 08/04/21
 *
 *
 * This file is part of Char.
 *
 * Char is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Char is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Char.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef CHAR_UTIL_HPP
#define CHAR_UTIL_HPP


#include <cstdio>
#include <string>
#include <stdexcept>
#include <memory>
#include <tuple>
#include <cstring>
#include <cstdio>
#include <random>
#include <type_traits>
#include <system_error>
#include <algorithm>
#include <utility>
#include <filesystem>
#include <charconv>
#include <limits>
#include <fstream>

#define STRINGIFY(s) #s

namespace chr
{
    /// Alias for a vector of bytes.
    using buffer = std::vector<std::byte>;
    /// Rectangle type, can have any numeric underlying type.
    template<typename Tnum,
             typename = typename std::enable_if<std::is_arithmetic<Tnum>::value,
                                                Tnum>::type>
    struct rect
    {
        /// X-position.
        Tnum x;
        /// Yposition.
        Tnum y;
        /// Width.
        Tnum width;
        /// Height.
        Tnum height;

        /**
         * @brief Conversion to a rectangle with a different numeric type.
         * @return A rectangle with a different numeric type.
         */
        template<typename Tother>
        operator rect<Tother>()
        {
            return rect<Tother>{
                static_cast<Tother>(x),
                static_cast<Tother>(y),
                static_cast<Tother>(width),
                static_cast<Tother>(height),
            };
        }
    };

    /**
     * @brief Calculate the union of two rectangles.
     *
     * @param r1 The first rectangle.
     * @param r2 The second recatnle.
     * @return The resulting union of the two rectangles.
     *
     * Adapted from SDL's SDL_UnionRect.
     */
    template<typename T>
    inline rect<T> rectUnion(const rect<T> &r1, const rect<T> &r2)
    {
        T x = std::min(r1.x, r2.x);
        T y = std::min(r1.x, r2.y);
        T x2 = std::max(r1.x + r1.width, r2.x + r2.width);
        T y2 = std::max(r1.y + r1.height, r2.y + r2.height);

        return { x, y, std::max(0.f, x2 - x), std::max(0.f, y2 - y) };
    }


    /**
     * @brief Calculate the intersection of two rectangles.
     *
     * @param r1 The first rectangle.
     * @param r2 The second recatnle.
     * @return The resulting intersection of the two rectangles.
     *
     * Adapted from SDL's SDL_IntersectRect.
     */
    template<typename T>
    inline rect<T> rectIntersect(const rect<T> &r1, const rect<T> &r2)
    {
        /* Horizontal intersection. */
        rect<T> result;
        T r1min = r1.x;
        T r1max = r1min + r1.width;
        T r2min = r2.x;
        T r2max = r2min + r2.width;
        if(r2min > r1min)
	        r1min = r2min;
        result.x = r1min;
        if(r2max < r1max)
	        r1max = r2max;
        result.width = r1max - r1min > 0 ? r1max - r1min : 0;

        /* Vertical intersection. */
        r1min = r1.y;
        r1max = r1min + r1.height;
        r2min = r2.y;
        r2max = r2min + r2.height;
        if(r2min > r1min)
	        r1min = r2min;
        result.y = r1min;
        if(r2max < r1max)
	        r1max = r2max;
        result.height = r1max - r1min > 0 ? r1max - r1min : 0;

        return result;
    }

    /**
     * @brief Equality operator for two rectangles.
     *
     * @param lhs Left handle rectangle.
     * @param rhs Right hand rectangle.
     * @return True if the rectangles have the same values, false if not.
     */
    template<typename T>
    inline bool operator ==(const rect<T> &lhs, const rect<T> &rhs)
    {
        return
            lhs.x == rhs.x &&
            lhs.y == rhs.y &&
            lhs.width == rhs.width &&
            lhs.height == rhs.height;

    }
    /**
     * @brief Inequality operator for two recangles.
     *
     * @param lhs Left handle side rectangle.
     * @param rhs Right hand side rectangle.
     * @return True if the rectangles differ, false if not.
     */
    template<typename T>
    inline bool operator !=(const rect<T> &lhs, const rect<T> &rhs)
    {
        return !(lhs == rhs);
    }
    /// A color type.
    struct color
    {
        /// Red.
        std::uint8_t r;
        /// Green.
        std::uint8_t g;
        /// Blue.
        std::uint8_t b;
        /// Alpha.
        std::uint8_t a;
        /**
         * @brief Get a color as an integer.
         *

         * @return A color as an integer.
         */
        inline std::uint32_t getColor() const
        {
            return
                std::uint32_t(r) << 24 |
                std::uint32_t(g) << 16 |
                std::uint32_t(b) << 8  |
                std::uint32_t(a);
        }
    };

    /// White color.
    constexpr color WHITE = color{0xFF, 0xFF, 0xFF, 0xFF};
    /// Black color.
    constexpr color BLACK = color{0x0, 0x0, 0x0, 0xFF};
    /// Red color.
    constexpr color RED = color{0xFF, 0x0, 0x0, 0xFF};
    /// Green color.
    constexpr color GREEN = color{0x0, 0xFF, 0x0, 0xFF};
    /// Blue color.
    constexpr color BLUE = color{0x0, 0x0, 0xFF, 0xFF};
    /// Yellow color.
    constexpr color YELLOW = color{0xFF, 0xFF, 0x0, 0xFF};
    /// Teal color.
    constexpr color TEAL = color{0x0, 0xFF, 0xFF, 0xFF};


    /**
     * @brief Equality operator for colors.
     *
     * @param lhs Left hand side color.
     * @param rhs Right hand side color.
     * @return True if the colors are the same, false if they differ.
     */
    inline bool operator ==(const color &lhs, const color &rhs)
    {
        return
            lhs.r == rhs.r &&
            lhs.g == rhs.g &&
            lhs.b == rhs.b &&
            lhs.a == rhs.a;
    }
    /**
     * @brief Inequality operator for colors.
     *
     * @param lhs Left hand side color.
     * @param rhs Right hand side color.
     * @return True if the colors differ, false if they are the same.
     */
    inline bool operator !=(const color &lhs, const color &rhs)
    {
        return !(lhs == rhs);
    }

    /**
     * @brief Convert an int like an integer to a color.
     *
     * @param hexColor An integer.
     * @return A color created from a string.
     */
    inline color toColor(std::uint32_t hexColor)
    {
        return color{
            .r = static_cast<std::uint8_t>((hexColor >> 24) & 0xff),
            .g = static_cast<std::uint8_t>((hexColor >> 16) & 0xff),
            .b = static_cast<std::uint8_t>((hexColor >> 8) & 0xff),
            .a = static_cast<std::uint8_t>((hexColor & 0xff))
        };
    }
    /**
     * @brief Convert a string like an integer to a color.
     *
     * @param str A string that represents an integer.
     * @return A color created from a string.
     */
    inline color toColor(std::string_view str)
    {
        using namespace std::string_literals;
        auto num = std::stoul(std::string(str));
        if(num > std::numeric_limits<std::uint32_t>::max())
            throw std::out_of_range("The number "s + std::to_string(num) +
                                    " is out of range.\n"s);
        return toColor(static_cast<std::uint32_t>(num));
    }
    /// Cast namespace, for printf functions.
    namespace cast
    {
        /**
         * @brief Dummy function for sprintf.
         *
         * @param v A variable.
         * @return The variable.
         */
        template<typename T>
        inline T cast(T v)
        {
            return v;
        }
        /**
         * @brief Convert an std::string to a char* for the sprintf functions.
         *
         * @param str A std::string.
         * @return A C-string from str.
         */
        inline const char *cast(const std::string &str)
        {
            return str.c_str();
        }
    }

    /**
     * @brief Sprintf that returns a C++ string. A temporary measure until
     * C++ 20 fmt is implemented.
     *
     * @param formatString C format string.
     * @param  args Varargs.
     * @return A formatted C++ string.
     */
    template <typename... Args>
    inline std::string sprintf(const std::string &formatString, Args&&... args)
    {
        std::size_t strlen = static_cast<std::size_t>(
            std::snprintf(nullptr, 0, formatString.c_str(),
                          chr::cast::cast(std::forward<Args>(args))...));

        auto cString = std::make_unique<char[]>(++strlen);
        std::snprintf(cString.get(), strlen, formatString.c_str(),
                      chr::cast::cast(std::forward<Args>(args))...);
        return std::string(cString.get());
    }
    template <typename... Args>
    /**
     * @brief snprintf that can take C++ objects. Guarantees null termination.
     *
     * @param result Out variable, C-string to write to.
     * @param size The size of result.
     * @param formatString A C-format string.
     * @param  args Varargs.
     * @return The number of characters written to result
     */
    inline std::size_t snprintf(char *result, std::size_t size,
                                const std::string &formatString, Args&&... args)
    {
        return std::snprintf(result, size, formatString.c_str(),
                             chr::cast::cast(std::forward<Args>(args))...);
    }


    /**
     * @brief Guarantees that str is null-terminated. Only works for stack strings.
     *
     * @param  A stack-allocated C-string.
     */
    template <std::size_t N>
    inline void nullStr(char (&str) [N])
    {
        str[N - 1] = '\0';
    }
    /**
     * @brief Get a C++ string with errno and a string description of errno.
     *

     * @return A C++ string with errno and a string description of errno.
     */
    inline std::string errStr()
    {
        return chr::sprintf("errno: %d: %s", errno, std::strerror(errno));
    }
    /**
     * @brief Convert a std::err to a C++ string with a description.
     *
     * @param err A C++ errc.
     * @return A std::err to a C++ string with a description.
     */
    inline std::string errStr(std::errc err)
    {
        return chr::sprintf("errno: %d: %s", static_cast<int>(err),
                             std::strerror(static_cast<int>(err)));
    }

    /**
     * @brief Convert a std::error_code to a C++ string with a description.
     *
     * @param errc An error code.
     * @return A std::error_code to a C++ string with a description.
     */
    inline std::string errStr(std::error_code errc)
    {
        return chr::sprintf("Error code: %d: %s",
                             errc.value(), errc.message());
    }
#ifdef _WIN32
    inline std::string winErrStr()
    {
        return chr::sprintf("Windows error code: %u", GetLastError());
    }
#endif /* _WIN32. */
    /**
     * @brief Convert a std::error_condition to a C++ string with a description.
     *
     * @param errc An error code.
     * @return A std::error_condition to a C++ string with a description.
     */
    inline std::string errStr(std::error_condition errc)
    {
        return chr::sprintf("Error code: %d, string: \"%s\"",
                             errc.value(), errc.message());
    }

    /**
     * @brief Create a fully-seeded mt19937.
     *
     * @return A fully-seeded mt19937.
     */
    template<class T = std::mt19937, std::size_t N = T::state_size * sizeof(typename T::result_type)>
    inline auto seededMT () -> typename std::enable_if<N, T>::type
    {
        std::random_device source;
        std::array<std::random_device::result_type, (N - 1) / sizeof(source()) + 1> randomData;
        std::generate(std::begin(randomData), std::end(randomData), std::ref(source));
        std::seed_seq seeds(std::begin(randomData), std::end(randomData));
        return T(seeds);
    }

    /// Helper compile time constants to find out if a type exists.
    template<typename T, typename = void>
    constexpr bool isDefined = false;
    /// Helper compile time constants to find out if a type exists.
    template<typename T>
    constexpr bool isDefined<T, decltype(typeid(T), void())> = true;

    /// Random number generator for mRandom.
    inline std::mt19937 mrng;

    /// Pseudo Random number generator for pRandom.
    inline std::mt19937 prng;
    /**
     * @brief For client-side randomness like for visual effects.
     *
     * @param min Minimum value.
     * @param max Maximum value.
     * @return Random value from [min, max).
     */
    template <typename Tnum>
    inline Tnum mRandom(Tnum min, Tnum max)
    {
        static_assert(std::is_arithmetic_v<Tnum>,
                      "randomNumber template type is not a number.");


        if constexpr (std::is_integral_v<Tnum>)
        {
            std::uniform_int_distribution<Tnum> dist(min, max);
            return dist(mrng);
        }
        else if(std::is_floating_point_v<Tnum>)
        {
            std::uniform_real_distribution<Tnum> dist(min, max);
            return dist(mrng);
        }
    }

    /**
     * @brief For client and server side randomness.
     *
     * @param min Minimum value.
     * @param max Maximum value.
     * @return Random value from [min, max).
     */
    template<typename Tint>
    inline Tint pRandom(Tint min, Tint max)
    {
        static_assert(std::is_integral_v<Tint>,
                      "pRandom template type is not an integral.");

        std::uniform_int_distribution<Tint> dist(min, max);
        return dist(prng);
    }

    /**
     * @brief Check if the string str begins with prefix.
     *
     * @param str A string.
     * @param prefix A prefix to search for in str.
     * @return True if str begins with prefix, false if str does not.
     */
    inline bool startsWith(std::string_view str, std::string_view prefix)
    {
        return str.rfind(prefix, 0) == 0;
    }

    /**
     * @brief Get the English-language ordinal of a number.
     *
     * @param i The number to get an ordinal for.
     * @return i's English ordinal.
     */
    template<typename Tint>
    inline std::string ordinal(Tint i)
    {
        static_assert(std::is_integral_v<Tint>,
                      "Ordinal template type is not an integral.");

        const static std::array suffixes = {"th", "st", "nd", "rd" };
        auto lastDigit = i % 10;
        return std::to_string(i) + (lastDigit > 3 ? suffixes[0] : suffixes[lastDigit]);
    }

    /**
     * @brief Case insensitive comparison of strings.
     *
     * @param str1 A string.
     * @param str2 Another string.
     * @return True if the lower case versions of the strings are the same.
     */
    inline bool icaseComp(std::string_view str1, std::string_view str2)
    {
        return std::equal(str1.begin(), str1.end(),
                          str2.begin(), str2.end(),
                          [](char a, char b)
                          {
                              return std::tolower(a) == std::tolower(b);
                          });
    }


    /**
     * @brief  Charconv wrapper for a better interface. Returns a T converted from a
     * string and std::errc() on success.
     *
     * @param str A string that reprsents a number of base "base".
     * @param base The base of the number represented by str.
     * @return A tuple containing the converted number and an error object.
     * The errc will be errc() if there was no error.
     */
    template<typename T>
    inline std::tuple<T, std::errc> from_chars(std::string_view str, int base = 10)
    {
        T result;
        auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(),
                                       result, base);

        return std::make_tuple(result, ec);
    }

    /**
     * @brief Get the memory usage (in bytes) of the current process.
     *

     * @return  The memory usage (in bytes) of the current process.
     */
    std::size_t getMemUsage();

    /**
     * @brief Bitfield macro (16 bit).
     *
     * @param n Number of times 0x1 should be shifted (should not exceed 16).
     * @return 0x1 shifted n times.
     */
    constexpr std::uint16_t Bit16(std::uint16_t n)
    {
        return UINT16_C(0x1) << n;
    }

    /**
     * @brief Bitfield macro (32 bit).
     *
     * @param n Number of times 0x1 should be shifted (should not exceed 32).
     * @return 0x1 shifted n times.
     */
    constexpr std::uint32_t Bit(std::uint32_t n)
    {
        return UINT32_C(0x1) << n;
    }

    /**
     * @brief Bitfield macro (64 bit).
     *
     * @param n Number of times 0x1 should be shifted (should not exceed 64).
     * @return 0x1 shifted n times.
     */
    constexpr std::uint64_t Bit64(std::uint64_t n)
    {
        return UINT64_C(0x1) << n;
    }

    std::string_view getCPUInfo();

    std::string_view getCompilerInfo();

    std::string_view getCompileDataAndTime();

    std::string_view getEndianessString();

    template<typename T>
    inline chr::buffer initBuffer(std::size_t size, const T &value)
    {
        chr::buffer result(size * sizeof(T));
        for(auto i = 0; i < size; i++)
            std::memcpy(&value, result.data() + (i * sizeof(T)), sizeof(T));
        return result;
    }

    /**
     * @brief Read a file and return its contents as a chr::buffer.
     *
     * @param ifs The ifstream to read.
     * @return The file as a buffer of bytes. An empty buffer on error.
     */
    inline chr::buffer fileToBuffer(std::ifstream &ifs)
    {
        if(!ifs)
            return {};
        std::streampos fileSize;
        ifs.seekg(0, std::ios::end);
        fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        chr::buffer result(fileSize);
        ifs.read(reinterpret_cast<char*>(&result[0]), fileSize);
        return result;
    }

    /**
     * @brief Read a file and return its contents as a chr::buffer.
     *
     * @param path The path to the file.
     * @return The file as a buffer of bytes. An empty buffer on error.
     */
    inline chr::buffer fileToBuffer(const std::filesystem::path &path)
    {
        std::ifstream tmpFile(path, std::ios::binary);
        return fileToBuffer(tmpFile);
    }

    template<typename Tbyte>
    std::tuple<std::shared_ptr<Tbyte[]>, std::streampos>
    fileToSharedBuffer(std::ifstream &ifs)
    {
        static_assert(sizeof(Tbyte) == 1,
            "The size of the template should be 1 (a byte).");
        if(!ifs)
            return std::make_tuple(std::shared_ptr<Tbyte[]>(nullptr),
                                   std::streampos(0));
        ifs.seekg(0, std::ios::end);
        std::streampos fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        std::shared_ptr<Tbyte[]> result(new Tbyte[fileSize]);
        ifs.read(reinterpret_cast<char*>(&result[0]), fileSize);
        return std::make_tuple(result, fileSize);
    }

    template<typename Tbyte>
    inline std::tuple<std::shared_ptr<Tbyte[]>, std::streampos>
    fileToSharedBuffer(const std::filesystem::path &path)
    {
        std::ifstream tmpFile(path, std::ios::binary);
        return fileToSharedBuffer<Tbyte>(tmpFile);
    }

    /**
     * @brief Divide two integers and round up.
     *
     * @param numerator Division numerator.
     * @param denominator Division denominator.
     * @return The result of the division rounded up.
     */
    template<typename Tint>
    inline Tint divCeil(Tint numerator, Tint denominator)
    {
        static_assert(std::is_integral_v<Tint>,
                      "Ceil template type is not an integral.");
        if(denominator == 0)
            throw std::invalid_argument("Denominator is 0");
        return numerator / denominator + (numerator % denominator > 0);
    }
}

/* The following code to create type-safe bitmasks is from
   https://wiggling-bits.net/using-enum-classes-as-type-safe-bitmasks/ */

/// A dummy struct for enabling bitwise operators for an enum class.
template<typename Enum>
struct EnableBitmaskOperators
{
    static const bool enable = false;
};


/**
 * @brief Bitwise OR for an enum class.
 *
 * @param lhs Left hand side enum value.
 * @param rhs Right hand side enum value.
 * @return lhs | rhs.
 */
template<typename Enum>
typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum>::type
operator |(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(
        static_cast<underlying>(lhs) |
        static_cast<underlying>(rhs)
        );
}



/**
 * @brief Bitwise AND for an enum class.
 *
 * @param lhs Left hand side enum value.
 * @param rhs Right hand side enum value.
 * @return lhs & rhs.
 */
template<typename Enum>
typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum>::type
operator &(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(
        static_cast<underlying>(lhs) &
        static_cast<underlying>(rhs)
        );
}


/**
 * @brief Bitwise XOR for an enum class.
 *
 * @param lhs Left hand side enum value.
 * @param rhs Right hand side enum value.
 * @return lhs ^ rhs.
 */
template<typename Enum>
typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum>::type
operator ^(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(
        static_cast<underlying>(lhs) ^
        static_cast<underlying>(rhs)
        );
}


/**
 * @brief Bitwise NOT for an enum class.
 *
 * @param e Enum value.
 * @return ~e.
 */
template<typename Enum>
typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum>::type
operator ~(Enum e)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(~static_cast<underlying>(e));
}



/**
 * @brief Bitwise OR= for an enum class.
 *
 * @param lhs Left hand side enum value.
 * @param rhs Right hand side enum value.
 * @return lhs | rhs, and setting lhs.
 */
template<typename Enum>
typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum>::type&
operator |=(Enum &lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    lhs =  static_cast<Enum>(
        static_cast<underlying>(lhs) |
        static_cast<underlying>(rhs)
        );

    return lhs;
}


/**
 * @brief Bitwise AND= for an enum class.
 *
 * @param lhs Left hand side enum value.
 * @param rhs Right hand side enum value.
 * @return lhs & rhs, and setting lhs.
 */
template<typename Enum>
typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum>::type&
operator &=(Enum &lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    lhs = static_cast<Enum>(
        static_cast<underlying>(lhs) &
        static_cast<underlying>(rhs)
        );

    return lhs;
}



/**
 * @brief Bitwise XOR= for an enum class.
 *
 * @param lhs Left hand side enum value.
 * @param rhs Right hand side enum value.
 * @return lhs ^ rhs, and setting lhs.
 */
template<typename Enum>
typename std::enable_if<EnableBitmaskOperators<Enum>::enable, Enum>::type&
operator ^=(Enum &lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    lhs = static_cast<Enum>(
        static_cast<underlying>(lhs) ^
        static_cast<underlying>(rhs)
        );

    return lhs;
}


/**
 * @brief Simple macro for making bitwise operators for an enum.
 * This macro MUST be called in the global namespace!
 */
#define ENABLE_BITMASK_OPERATORS(x)             \
    template<>                                  \
    struct EnableBitmaskOperators<x>       \
    {                                           \
        static const bool enable = true;        \
    };


#endif /* CHAR_UTIL_HPP */
