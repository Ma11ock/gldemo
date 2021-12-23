#include "util.hpp"

#include <array>
#include <limits>
#include <algorithm>
#include <string>
#include <string_view>
#include <memory>
#include <fstream>
#include <streambuf>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/resource.h>
#endif


extern "C" {
#if defined __BSD__ || defined __APPLE__
#include <sys/sysctl.h>
#elif defined _WIN32
#include <sysinfoapi.h>
#include <windows.h>
#include <malloc.h>
#include <tchar.h>
#endif /* __FreeBSD__ */
}

std::size_t chr::getMemUsage()
{
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS counters;
    if(GetProcessMemoryInfo(GetCurrentProcess(), &counters,
                            sizeof(counters)))
        return counters.PagefileUsage;
    else
        return 0;
#else /* Unix */
    rusage r;
    getrusage(RUSAGE_SELF, &r);
    return r.ru_maxrss;
#endif /* _WIN32 */
}


std::string_view chr::getCPUInfo()
{
    static std::string cpuInfo;
    if(!cpuInfo.empty())
        return cpuInfo;

// For other CPU architectures.
#ifdef _WIN32 // TODO
#elif defined __linux__
    /* There is (unfortunately) no way to get this information without reading
       /proc/ on Ganoo slash Leenoocks */
    std::ifstream procCpuInfo("/proc/cpuinfo");
    if(!procCpuInfo)
    {
        cpuInfo = "Unknown";
        return cpuInfo;
    }

    std::string line;
    while(std::getline(procCpuInfo, line)
          && line.find("model name") == std::string::npos);
    auto colon = line.find(':');
    if(colon == std::string::npos)
    {
        cpuInfo = "Unknown";
        return cpuInfo;
    }

    cpuInfo = line.substr(colon + 1);
//#elif defined __FreeBSD__ || defined __OpenBSD__ || defined __APPLE__ || defined  __NetBSD__
#elif defined __BSD__ || defined __APPLE__ // TODO test
    /* Get the CPU model. */
    int mib[2] = {CTL_HW, HW_MODEL};
    std::size_t len;

    sysctl(mib, 2, nullptr, &len, nullptr, 0);
    auto cStr = std::make_unique<char[]>(++len);
    sysctl(mib, 2, cStr.get(), &len, nullptr, 0);

    /* Get number of CPUs. */
    mib[1] = HW_NCPU;
    int numCPUs = 0;
    len = sizeof(numCPUs);
    sysctl(mib, 2, &numCPUs, &len, nullptr, 0);

    cpuInfo = chr::snprintf("%s with %d cores", cStr.get(), numCPUs);
#elif defined __BEOS__ // TODO
#endif /* WIN32 */

    return cpuInfo;
}

std::string_view chr::getCompilerInfo()
{
#define compiler_ver(major, minor, patch)                       \
    STRINGIFY(major) "." STRINGIFY(minor) "." STRINGIFY(patch)

    return
#ifdef __clang__
        "clang " compiler_ver(__clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined __GNUC__
    "gcc " compiler_ver(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined _MSC_VER
    "MSVC " STRINGIFY(_MSC_VER);
#else
    "";
#endif /* __clang__ */
}

std::string_view chr::getCompileDataAndTime()
{
    return __DATE__ "@" __TIME__;
}

std::string_view chr::getEndianessString()
{
#ifdef CHR_LITTLE_ENDIAN
    return "Little Endian";
#else /* CHR_BIG_ENDIAN */
    return "Big Endian";
#endif /* CHR_LITTLE_ENDIAN. */

}
