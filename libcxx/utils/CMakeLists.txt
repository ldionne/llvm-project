add_custom_target(libcxx-generate-public-header-transitive-inclusion-tests
    COMMAND "${Python3_EXECUTABLE}" "${LIBCXX_SOURCE_DIR}/utils/generate_header_inclusion_tests.py"
    COMMENT "Generate tests checking for mandated transitive includes in public headers.")

add_custom_target(libcxx-generate-public-header-tests
    COMMAND "${Python3_EXECUTABLE}" "${LIBCXX_SOURCE_DIR}/utils/generate_header_tests.py"
    COMMENT "Generate tests for including public headers.")

add_custom_target(libcxx-generate-feature-test-macros
    COMMAND "${Python3_EXECUTABLE}" "${LIBCXX_SOURCE_DIR}/utils/generate_feature_test_macro_components.py"
    COMMENT "Generate the <version> header and tests for feature test macros.")

# This function generates a "unique" identifier based on various properties
# given as arguments. The idea is to encode all ABI-affecting properties
# in that identifier, so that we can store ABI information and associate it
# to a specific ABI configuration.
#
# Right now, this is done by using the ABI identifier as the filename containing
# the list of symbols exported by libc++ for that configuration, however we could
# make it more sophisticated if the number of ABI-affecting parameters grew.
function(cxx_abi_list_identifier result triple abi_library abi_version unstable exceptions new_delete_in_libcxx)
  set(abi_properties)

  if ("${triple}" MATCHES "darwin")
    # Ignore the major, minor, and patchlevel versions of darwin targets.
    string(REGEX REPLACE "darwin[0-9]+\\.[0-9]+\\.[0-9]+" "darwin" triple "${triple}")
  elseif("${triple}" MATCHES "freebsd")
    # Ignore the major and minor versions of freebsd targets.
    string(REGEX REPLACE "freebsd[0-9]+\\.[0-9]+" "freebsd" triple "${triple}")
  endif()
  list(APPEND abi_properties "${triple}")
  list(APPEND abi_properties "${abi_library}")
  list(APPEND abi_properties "v${abi_version}")
  if (${unstable})
    list(APPEND abi_properties "unstable")
  else()
    list(APPEND abi_properties "stable")
  endif()
  if (${exceptions})
    list(APPEND abi_properties "exceptions")
  else()
    list(APPEND abi_properties "noexceptions")
  endif()
  if (${new_delete_in_libcxx})
    list(APPEND abi_properties "new_in_libcxx")
  else()
    list(APPEND abi_properties "no_new_in_libcxx")
  endif()

  list(JOIN abi_properties "." tmp)
  set(${result} "${tmp}" PARENT_SCOPE)
endfunction()

cxx_abi_list_identifier(abi_list_identifier
  "${TARGET_TRIPLE}"
  "${LIBCXX_CXX_ABI_LIBNAME}"
  "${LIBCXX_ABI_VERSION}"
  "${LIBCXX_ABI_UNSTABLE}"
  "${LIBCXX_ENABLE_EXCEPTIONS}"
  "${LIBCXX_ENABLE_NEW_DELETE_DEFINITIONS}"
)

if (TARGET cxx_shared)
  add_custom_target(libcxx-generate-abilist
    COMMAND "${Python3_EXECUTABLE}" "${LIBCXX_SOURCE_DIR}/utils/generate_abi_list.py"
            --output "${LIBCXX_SOURCE_DIR}/test/libcxx/abi/${abi_list_identifier}.abilist"
            "$<TARGET_FILE:cxx_shared>"
    DEPENDS cxx_shared
    COMMENT "Generating the ABI list file for configuration ${abi_list_identifier}")
endif()

add_custom_target(libcxx-generate-files
    DEPENDS libcxx-generate-public-header-transitive-inclusion-tests
            libcxx-generate-public-header-tests
            libcxx-generate-feature-test-macros
            $<$<TARGET_EXISTS:libcxx-generate-abilist>:libcxx-generate-abilist>
    COMMENT "Create all the auto-generated files in libc++ and its tests.")
