# This file defines an interface target named libcxx::compiler-runtime that contains
# the flags necessary to link against the library (or libraries) that contain various
# functions implementing compiler builtins. Its behavior can be customized with the
# LIBCXX_COMPILER_RUNTIME variable:
#
#   'libgcc':
#     Will instruct to use libgcc or libgcc_s, whichever is present. When
#     libgcc is used, we also link against libatomic if it is found, since
#     libgcc does not contain all the necessary atomic operations.
#
#   'compiler-rt':
#     Will instruct to use LLVM's own compiler-rt library. When that is used,
#     there is no need to link against a separate library for atomic operations,
#     since compiler-rt implements those.



if (LIBCXX_BUILTINS_LIBRARY STREQUAL "compiler-rt")
    find_compiler_rt_library(builtins LIBCXX_BUILTINS_LIBRARY)
    if (LIBCXX_BUILTINS_LIBRARY)
        target_link_libraries(${target} PRIVATE "${LIBCXX_BUILTINS_LIBRARY}")
    endif()
elseif(LIBCXX_BUILTINS_LIBRARY STREQUAL "libgcc")




if (LIBCXX_USE_COMPILER_RT)
    find_compiler_rt_library(builtins LIBCXX_BUILTINS_LIBRARY)
    if (LIBCXX_BUILTINS_LIBRARY)
      target_link_libraries(${target} PRIVATE "${LIBCXX_BUILTINS_LIBRARY}")
    endif()
elseif (LIBCXX_HAS_GCC_LIB)
    target_link_libraries(${target} PRIVATE gcc)
elseif (LIBCXX_HAS_GCC_S_LIB)
    target_link_libraries(${target} PRIVATE gcc_s)
endif()

  if (LIBCXX_HAS_ATOMIC_LIB)
    target_link_libraries(${target} PRIVATE atomic)
  endif()
