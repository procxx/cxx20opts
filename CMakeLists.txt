cmake_minimum_required(VERSION 3.5)

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


add_library(${PROJECT_NAME} INTERFACE ${${PROJECT_NAME}_HDR})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC INTERFACE
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

#install(
#TARGETS ${PROJECT_NAME}
#    EXPORT ${PROJECT_NAME}
#    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
