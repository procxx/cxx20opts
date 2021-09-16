cmake_minimum_required(VERSION 3.14)

project(cxx20opts
    VERSION 0.0
    LANGUAGES CXX
    HOMEPAGE_URL "https://github.com/procxx/cxx20opts"
    DESCRIPTION "C++20 (17) command line parsing library"
)
include(GNUInstallDirs)

option(BUILD_TESTS "Build and Run tests." ON)
option(BUILD_WITH_SANITIZERS "" ON)

set(${PROJECT_NAME}_HDR
    include/cxx20opts/cxx20opts.hxx
)

add_custom_target(${PROJECT_NAME}_ SOURCES ${${PROJECT_NAME}_HDR})

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${${PROJECT_NAME}_HDR}>
    $<INSTALL_INTERFACE:${${PROJECT_NAME}_HDR}>
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# install headers
install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# install cmake targets
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}_Targets
)

# create ${PROJECT_NAME}Targets.cmake
install(EXPORT ${PROJECT_NAME}_Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME}
)

# create ${PROJECT_NAME}ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# create ${PROJECT_NAME}Config.cmake file
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME}
)

# create pkgconfig file
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}.pc.in" ${PROJECT_NAME}.pc @ONLY
)

install(
    FILES
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake

    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME}
)
install(
    FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig
)
