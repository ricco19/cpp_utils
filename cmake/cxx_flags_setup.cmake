# MSVC flags. Multi-config generator so we only need one setup
if(CMCC_IS_MSVC)
  target_compile_options(${PROJECT_NAME}
                         PRIVATE /Wx
                                 /permissive
                                 /W4
                                 /w14242
                                 /w14254
                                 /w14263
                                 /w14265
                                 /w14287
                                 /we4289
                                 /w14296
                                 /w14311
                                 /w14545
                                 /w14546
                                 /w14547
                                 /w14549
                                 /w14555
                                 /w14619
                                 /w14640
                                 /w14826
                                 /w14905
                                 /w14906
                                 /w14928)
endif()

# CLANG-CL flags (WIP)
if(CMCC_IS_CLANG-CL)
  target_compile_options(${PROJECT_NAME}
                         PRIVATE /WX
                                 /Wall
                                 -Wno-unused-command-line-argument
                                 -Wno-c++98-compat
                                 -Wno-nonportable-system-include-path)
endif()

# GCC flags
if(CMCC_IS_GCC)
  # Debug flags
  target_compile_options(${PROJECT_NAME}
                         PUBLIC $<$<CONFIG:Debug>:-g
                                -D_FORTIFY_SOURCE=2
                                -D_GLIBCXX_ASSERTIONS
                                -fno-omit-frame-pointer>)
  # Flags that we use no matter what the build type is
  target_compile_options(${PROJECT_NAME}
                         PUBLIC -O3
                                -Werror
                                -Wall
                                -Wextra
                                -Wcast-qual
                                -Wfloat-equal
                                -Wformat-security
                                -Wpointer-arith
                                -Wshadow
                                -Wswitch-default
                                -Wswitch-enum
                                -Wundef
                                -Wunreachable-code
                                -Wwrite-strings
                                -Wnon-virtual-dtor
                                -Wold-style-cast
                                -Wcast-align
                                -Wunused
                                -Woverloaded-virtual
                                -Wpedantic
                                -Wconversion
                                -Wsign-conversion
                                -Wmisleading-indentation
                                -Wduplicated-cond
                                -Wduplicated-branches
                                -Wlogical-op
                                -Wnull-dereference
                                -Wuseless-cast
                                -Wdouble-promotion
                                -Wformat=2)
  # Minimum size, overwrite O3
  target_compile_options(${PROJECT_NAME} PUBLIC $<$<CONFIG:MinSizeRel>:-Os>)
  # Minimum size with debug info
  target_compile_options(${PROJECT_NAME}
                         PUBLIC $<$<CONFIG:RelWithDebInfo>:-Os
                                -g
                                -D_FORTIFY_SOURCE=2
                                -D_GLIBCXX_ASSERTIONS
                                -fno-omit-frame-pointer>)
  # Sanitizers
  if(ENABLE_ASAN)
    target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
    target_link_libraries(${PROJECT_NAME} PUBLIC -fsanitize=address)
    message("   -> Using address sanitizer.")
  endif()
  if(ENABLE_TSAN)
    target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=thread)
    target_link_libraries(${PROJECT_NAME} PUBLIC -fsanitize=thread)
    message("   -> Using thread sanitizer.")
  endif()
  if(ENABLE_UBSAN)
    target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=undefined)
    target_link_libraries(${PROJECT_NAME} PUBLIC -fsanitize=undefined)
    message("   -> Using undefined behaviour sanitizer.")
  endif()
endif()

# CLANG flags
if(CMCC_IS_CLANG)
  # Debug flags
  target_compile_options(${PROJECT_NAME}
                         PUBLIC $<$<CONFIG:Debug>:-g
                                -D_FORTIFY_SOURCE=2
                                -D_GLIBCXX_ASSERTIONS
                                -fno-omit-frame-pointer>)
  # Flags that we use no matter what the build type is
  target_compile_options(${PROJECT_NAME}
                         PUBLIC -O3
                                -Werror
                                -Wall
                                -Wextra
                                -Wcast-qual
                                -Wfloat-equal
                                -Wformat-security
                                -Wpointer-arith
                                -Wshadow
                                -Wswitch-default
                                -Wswitch-enum
                                -Wundef
                                -Wunreachable-code
                                -Wwrite-strings
                                -Wnon-virtual-dtor
                                -Wold-style-cast
                                -Wcast-align
                                -Wunused
                                -Woverloaded-virtual
                                -Wpedantic
                                -Wconversion
                                -Wsign-conversion
                                -Wnull-dereference
                                -Wdouble-promotion
                                -Wformat=2)
  # Minimum size, overwrite O3
  target_compile_options(${PROJECT_NAME} PUBLIC $<$<CONFIG:MinSizeRel>:-Os>)
  # Minimum size with debug info
  target_compile_options(${PROJECT_NAME}
                         PUBLIC $<$<CONFIG:RelWithDebInfo>:-Os
                                -g
                                -D_FORTIFY_SOURCE=2
                                -D_GLIBCXX_ASSERTIONS
                                -fno-omit-frame-pointer>)
  # Sanitizers
  if(ENABLE_ASAN)
    target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
    target_link_libraries(${PROJECT_NAME} PUBLIC -fsanitize=address)
    message("   -> Using address sanitizer.")
  endif()
  if(ENABLE_MSAN)
    target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=memory)
    target_link_libraries(${PROJECT_NAME} PUBLIC -fsanitize=memory)
    message("   -> Using memory sanitizer.")
  endif()
  if(ENABLE_TSAN)
    target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=thread)
    target_link_libraries(${PROJECT_NAME} PUBLIC -fsanitize=thread)
    message("   -> Using thread sanitizer.")
  endif()
  if(ENABLE_UBSAN)
    target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=undefined)
    target_link_libraries(${PROJECT_NAME} PUBLIC -fsanitize=undefined)
    message("   -> Using undefined behaviour sanitizer.")
  endif()

endif()
