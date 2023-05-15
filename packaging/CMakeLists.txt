# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

if (WIN32) #Starting simple with archive only packages
    set(CPACK_GENERATOR ZIP)
else ()
    set(CPACK_GENERATOR TGZ)
endif ()

set(CPACK_PACKAGE_NAME "awslabs_enhanced_cpp")
set(CPACK_PACKAGE_VENDOR "Amazon Web Services")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR ${awslabs_enhanced_cpp_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${awslabs_enhanced_cpp_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${awslabs_enhanced_cpp_VERSION_PATCH})
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_LIST_DIR}/../LICENSE)
set(CPACK_RESOURCE_FILE_README${CMAKE_CURRENT_LIST_DIR}/../README.md)
set(CPACK_MONOLITHIC_INSTALL OFF)
set(CPACK_COMPONENTS_GROUPING IGNORE)
include(CPack)
cpack_add_component(awslabs_enhanced_cpp_headers
        DISPLAY_NAME awslabs_enhanced_cpp_headers
        DESCRIPTION "High-level enhancements for AWS in C++ libraries"
        REQUIRED
        )

cpack_add_component(awslabs_enhanced_cpp_tools
        DISPLAY_NAME awslabs_enhanced_cpp_headers
        DESCRIPTION "High-level enhancements for AWS in C++ libraries' tools"
        REQUIRED
        )
