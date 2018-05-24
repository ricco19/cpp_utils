# Check for compiler type
set(CXX_FLAGS_STYLE_GNU OFF)
set(CXX_FLAGS_STYLE_MSVC OFF)
set(CXX_FLAGS_STYLE_CLANGCL OFF)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CXX_FLAGS_STYLE_GNU ON)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if ("${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
        set(CXX_FLAGS_STYLE_CLANGCL ON)
    else ()
        set(CXX_FLAGS_STYLE_GNU ON)
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CXX_FLAGS_STYLE_MSVC ON)
else ()
    message(FATAL_ERROR "Unsupported compiler!")
endif ()

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    ## GCC/Clang Flags
    $<$<BOOL:${CXX_FLAGS_STYLE_GNU}>:
        # Debug settings
        $<$<CONFIG:Debug>:
            "-g"
            "-D_FORTIFY_SOURCE=2"
            "-D_GLIBCXX_ASSERTIONS"
            "-fno-omit-frame-pointer"
        >
        # Always use O3 except for minimum size
        $<$<NOT:$<CONFIG:MinSizeRel>>:
            "-O3"
        >
        # Almost all warnings as errors
        "-Werror"
        "-pedantic"
        "-pedantic-errors"
        "-Wall"
        "-Wextra"
        # More warnings not included in all/extra
        "-Wcast-align"
        "-Wcast-qual"
        "-Wconversion"
        "-Wfloat-equal"
        "-Wformat-security"
        "-Wpointer-arith"
        "-Wshadow"
        "-Wswitch-default"
        "-Wswitch-enum"
        "-Wundef"
        "-Wunreachable-code"
        "-Wwrite-strings"
    >
    ## MSVC flags
    $<$<BOOL:${CXX_FLAGS_STYLE_MSVC}>:
        # Optimization
        #"/O3"
        # Lots of warnings as errors, /Wall just doesnt work
        "/WX"
        "/W4"
    >
    ## CLANG-CL flags
    $<$<BOOL:${CXX_FLAGS_STYLE_CLANGCL}>:
        # Optimization
        #"/O3"
        # Almost all warnings as errors
        "/WX"
        "/Wall"
        # Disable some warnings
        "-Wno-unused-command-line-argument"
        "-Wno-c++98-compat"
        "-Wno-nonportable-system-include-path"
    >
)
