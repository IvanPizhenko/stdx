cmake_minimum_required(VERSION 3.28)

project(stdx VERSION 0.0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(stdx)

target_include_directories(stdx
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

file(GLOB_RECURSE STDX_SOURCES CONFIGURE_DEPENDS src/main/*.cpp)
file(GLOB_RECURSE STDX_HEADERS CONFIGURE_DEPENDS include/*.h include/*.hpp)

target_sources(stdx PRIVATE ${STDX_SOURCES})

set_target_properties(stdx PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${STDX_HEADERS}"
)

target_compile_options(stdx PRIVATE -Wall -Wextra -Wpedantic -Werror)

if(EXISTS "/opt/boost-1.92.0-custom/include")
    target_include_directories(stdx PUBLIC /opt/boost-1.92.0-custom/include)
endif()

include(GNUInstallDirs)
install(TARGETS stdx
    EXPORT stdxTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/stdx
)

install(EXPORT stdxTargets
    FILE stdxTargets.cmake
    NAMESPACE stdx::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stdx
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/stdx)

option(STDX_ENABLE_TESTING "Enable testing for stdx library" ON)
if(STDX_ENABLE_TESTING)
    enable_testing()
    #add_subdirectory(tests)
endif()

# Generate a CMake package configuration file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/stdxConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/stdxConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/stdxConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stdx
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/stdxConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/stdxConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stdx
)

add_executable(unique_queue_test src/test/unique_queue_test.cpp)
target_link_libraries(unique_queue_test PRIVATE stdx)
target_include_directories(unique_queue_test PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_executable(ptr_unique_queue_test src/test/ptr_unique_queue_test.cpp)
target_link_libraries(ptr_unique_queue_test PRIVATE stdx)
target_include_directories(ptr_unique_queue_test PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(STDX_ENABLE_TESTING)
    add_test(NAME unique_queue_test COMMAND unique_queue_test)
    add_test(NAME ptr_unique_queue_test COMMAND ptr_unique_queue_test)
endif()
