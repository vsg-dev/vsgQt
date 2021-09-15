# vsgQt
Qt integration with VulkanSceneGraph

## Checking out vsgQt

    git clone https://github.com/vsg-dev/vsgQt.git

## Required dependencies:

* [VulkanSDK](https://www.lunarg.com/vulkan-sdk/)
* [VulkanSceneGraph](https://github.com/vsg-dev/VulkanSceneGraph)
* [CMake](https://cmake.org/) minimum version 3.7
* [Qt](https://www.qt.io/) minimum version 5.10 or later
* C++17 capable compiler

## Building vsgQt

The first run of cmake will automatically checkout imgui as a submodule when required.

    cd vsgQt
    cmake .
    make -j 8

## Example

* [vsgqtviewer](examples/vsgqtviewer/main.cpp) - example of QApplication/QMainWindow usage with vsgQt::ViewerWindow.
