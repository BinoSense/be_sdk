#ifndef BIONIC_EYES_EXPORT_H
#define BIONIC_EYES_EXPORT_H

// 平台检测宏
#if defined(_WIN32) || defined(_WIN64)
    #define BE_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
    #define BE_PLATFORM_MAC 1
#elif defined(__linux__)
    #define BE_PLATFORM_LINUX 1
#else
    #error "Unsupported platform"
#endif

// 编译器检测宏
#if defined(_MSC_VER)
    #define BE_COMPILER_MSVC 1
#elif defined(__GNUC__)
    #define BE_COMPILER_GCC 1
#elif defined(__clang__)
    #define BE_COMPILER_CLANG 1
#endif

// DLL导出/导入宏
#if BE_PLATFORM_WINDOWS
    #ifdef BIONIC_EYES_BUILDING_DLL
        #define BE_API __declspec(dllexport)
    #else
        #define BE_API __declspec(dllimport)
    #endif
#else
    // Linux/macOS: 使用可见性属性
    #if __GNUC__ >= 4 || defined(__clang__)
        #ifdef BIONIC_EYES_BUILDING_SHARED
            #define BE_API __attribute__((visibility("default")))
        #else
            #define BE_API
        #endif
    #else
        #define BE_API
    #endif
#endif

// 用于C接口的函数调用约定
#if BE_PLATFORM_WINDOWS
    #define BE_CALL __cdecl
#else
    #define BE_CALL
#endif

// 强制内联宏
#if defined(_MSC_VER)
    #define BE_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define BE_FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define BE_FORCE_INLINE inline
#endif

// 不返回宏（用于标记不返回的函数）
#if defined(_MSC_VER)
    #define BE_NORETURN __declspec(noreturn)
#elif defined(__GNUC__) || defined(__clang__)
    #define BE_NORETURN __attribute__((noreturn))
#else
    #define BE_NORETURN
#endif

// 对齐宏
#if defined(_MSC_VER)
    #define BE_ALIGN(x) __declspec(align(x))
#elif defined(__GNUC__) || defined(__clang__)
    #define BE_ALIGN(x) __attribute__((aligned(x)))
#else
    #define BE_ALIGN(x)
#endif

// 打包结构体宏
#if defined(_MSC_VER)
    #define BE_PACKED_BEGIN __pragma(pack(push, 1))
    #define BE_PACKED_END __pragma(pack(pop))
    #define BE_PACKED
#elif defined(__GNUC__) || defined(__clang__)
    #define BE_PACKED_BEGIN
    #define BE_PACKED_END
    #define BE_PACKED __attribute__((packed))
#else
    #define BE_PACKED_BEGIN
    #define BE_PACKED_END
    #define BE_PACKED
#endif

// 平台特定的路径分隔符
#if BE_PLATFORM_WINDOWS
    #define BE_PATH_SEPARATOR '\\'
    #define BE_PATH_SEPARATOR_STR "\\"
#else
    #define BE_PATH_SEPARATOR '/'
    #define BE_PATH_SEPARATOR_STR "/"
#endif

// 动态库扩展名
#if BE_PLATFORM_WINDOWS
    #define BE_DLL_EXT ".dll"
#elif BE_PLATFORM_MAC
    #define BE_DLL_EXT ".dylib"
#else
    #define BE_DLL_EXT ".so"
#endif

#endif // BIONIC_EYES_EXPORT_H
