#==============================================================================
# Build Google Benchmark
#==============================================================================

include(ExternalProject)

# Build GoogleBenchmark against the library under test (via the inherited CMAKE_CXX_FLAGS,
# which carry the -nostdinc++/-isystem/-L/-rpath flags). This target is part of the default
# 'all' target so that it is built when the standalone test-suite project is built; the
# benchmark tests (dry-run or not) link against it via %{benchmark_flags}.
ExternalProject_Add(google-benchmark
        PREFIX google-benchmark
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/benchmark"
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/google-benchmark
        CMAKE_CACHE_ARGS
          -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
          -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
          -DCMAKE_CXX_COMPILER_TARGET:STRING=${CMAKE_CXX_COMPILER_TARGET}
          -DCMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM}
          -DCMAKE_BUILD_TYPE:STRING=RELEASE
          -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
          -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
          -DBENCHMARK_USE_LIBCXX:BOOL=ON
          -DBENCHMARK_ENABLE_TESTING:BOOL=OFF)
