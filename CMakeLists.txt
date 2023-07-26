cmake_minimum_required(VERSION 3.7)

project(vsgQt
    VERSION 0.1.0
    DESCRIPTION "Qt integration with VulkanSceneGraph"
    LANGUAGES CXX
)
set(VSGQT_SOVERSION 1)
set(VSGQT_RELEASE_CANDIDATE 0)

set(VSGQT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Root source directory of vsgQt")
set(VSGQT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "Root binary directory of vsgQt")

# set the use of C++17 globally as all examples require it
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_PACKAGE_NAME Qt5 CACHE STRING "Set Qt package name, i.e. Qt5 or Qt6.")

find_package(vsg 1.0.8)

vsg_setup_dir_vars()
vsg_setup_build_vars()

find_package(vsgXchange) # only used by exanples

# if Qt5 then we need 5.10 or later
find_package(${QT_PACKAGE_NAME} COMPONENTS Widgets REQUIRED)

vsg_add_target_clang_format(
    FILES
        include/*/*.h
        src/*/*.cpp
)
vsg_add_target_clobber()
vsg_add_target_cppcheck(
    FILES
        include/*/*.h
        src/*/*.cpp
)
vsg_add_target_docs(
    FILES
        src
        include/*/*.h
        src/*/*.cpp
)
vsg_add_target_uninstall()

# only provide custom targets if not building as a submodule/FetchContent
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

    vsg_add_option_maintainer(
        PREFIX v
        RCLEVEL ${VSGQT_RELEASE_CANDIDATE}
    )

endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
    add_definitions(-DNOMINMAX)
    set(MODE WIN32)
endif()

add_subdirectory(src/vsgQt)
add_subdirectory(examples)

vsg_add_feature_summary()
