# vsgQt
Open Source, cross platform C++ library providing integration of VulkanSceneGraph with Qt windowing. Supports Windows, Linux and macOS. 

Qt, and hence vsgQt, is suited for interactive, event driven applications. For real-time applications we recommend that you use the VulkanSceneGraph's native windowing support and the standard VulkanSceneGraph frame driven shown in examples found in [vsgExamples](https://github.com/vsg-dev/vsgExamples/examples/viewer/vsgviewer/vsgviewer.cpp).

## Checking out vsgQt

    git clone https://github.com/vsg-dev/vsgQt.git

## Dependencies:

* [VulkanSDK](https://www.lunarg.com/vulkan-sdk/) version 1.2.162 or later
* [VulkanSceneGraph](https://github.com/vsg-dev/VulkanSceneGraph) master recommended.
* [CMake](https://cmake.org/) version 3.7 or later
* [Qt](https://www.qt.io/) version 5.10 or later
* C++17 capable compiler

## Building vsgQt

    cd vsgQt
    cmake .
    make -j 8

## Example

* [vsgqtviewer](examples/vsgqtviewer/main.cpp) - example of QApplication/QMainWindow usage with vsgQt::ViewerWindow.
