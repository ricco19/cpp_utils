# Check for compiler type
set(CXX_FLAGS_STYLE_GCC OFF)
set(CXX_FLAGS_STYLE_CLANG OFF)
set(CXX_FLAGS_STYLE_MSVC OFF)
set(CXX_FLAGS_STYLE_CLANGCL OFF)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CXX_FLAGS_STYLE_GCC ON)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if ("${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
        set(CXX_FLAGS_STYLE_CLANGCL ON)
    else ()
        set(CXX_FLAGS_STYLE_CLANG ON)
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CXX_FLAGS_STYLE_MSVC ON)
else ()
    message(FATAL_ERROR "Unsupported compiler!")
endif ()

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    ## GCC Flags
    $<$<BOOL:${CXX_FLAGS_STYLE_GCC}>:
        # Debug only flags
        $<$<CONFIG:Debug>:
            "-g"
            "-D_FORTIFY_SOURCE=2"
            "-D_GLIBCXX_ASSERTIONS"
            "-fno-omit-frame-pointer"
            "-O2"
        >
        # Release flags
        $<$<CONFIG:Release>:
            "-O3"
        >
        # Almost all warnings as errors
        "-Werror"
        "-Wall"
        "-Wextra"
        # More warnings not included in all/extra
        "-Wcast-qual"
        "-Wfloat-equal"
        "-Wformat-security"
        "-Wpointer-arith"
        "-Wshadow"
        "-Wswitch-default"
        "-Wswitch-enum"
        "-Wundef"
        "-Wunreachable-code"
        "-Wwrite-strings"
        "-Wnon-virtual-dtor"
        "-Wold-style-cast"
        "-Wcast-align"
        "-Wunused"
        "-Woverloaded-virtual"
        "-Wpedantic"
        "-Wconversion"
        "-Wsign-conversion"
        "-Wmisleading-indentation"
        "-Wduplicated-cond"
        "-Wduplicated-branches"
        "-Wlogical-op"
        "-Wnull-dereference"
        "-Wuseless-cast"
        "-Wdouble-promotion"
        "-Wformat=2"
    >
    # Clang Flags
    $<$<BOOL:${CXX_FLAGS_STYLE_CLANG}>:
        # Debug only flags
        $<$<CONFIG:Debug>:
            "-g"
            "-D_FORTIFY_SOURCE=2"
            "-D_GLIBCXX_ASSERTIONS"
            "-fno-omit-frame-pointer"
        >
        # Always use O2 except for minimum size
        $<$<NOT:$<CONFIG:MinSizeRel>>:
            "-O2"
        >
        # Almost all warnings as errors
        "-Werror"
        "-Wall"
        "-Wextra"
        # More warnings not included in all/extra
        "-Wcast-qual"
        "-Wfloat-equal"
        "-Wformat-security"
        "-Wpointer-arith"
        "-Wshadow"
        "-Wswitch-default"
        "-Wswitch-enum"
        "-Wundef"
        "-Wunreachable-code"
        "-Wwrite-strings"
        "-Wnon-virtual-dtor"
        "-Wold-style-cast"
        "-Wcast-align"
        "-Wunused"
        "-Woverloaded-virtual"
        "-Wpedantic"
        "-Wconversion"
        "-Wsign-conversion"
        "-Wnull-dereference"
        "-Wdouble-promotion"
        "-Wformat=2"
        #"-Wlifetime" #clang 8.0+ only
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
