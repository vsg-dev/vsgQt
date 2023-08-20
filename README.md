# vsgQt
Open Source ([MIT Licensed](LICENSE.md)), cross platform C++ library providing integration of VulkanSceneGraph with Qt windowing. Supports Windows, Linux and macOS.

vsgQt provides full Vulkan support through the VulkanSceneGraph's built in Window/VkSurface support rather than the limited Vulkan support that Qt-5.10 or later provide. Using the VulkanSceneGraph for providing Vulkan support avoids the restriction that Qt's VulkanWindow has with not being able to share VkDevice between windows, and provides compatibility with Qt versions prior to it adding Vulkan support.  Sharing vsg::Device/VkDevice between Windows is crucial for providing multiple windows without blowing up GPU memory usage.

## Checking out vsgQt

~~~ sh
git clone https://github.com/vsg-dev/vsgQt.git
~~~

## Dependencies:

* [VulkanSDK](https://www.lunarg.com/vulkan-sdk/) version 1.1.70 or later
* [VulkanSceneGraph](https://github.com/vsg-dev/VulkanSceneGraph) master, 1.0.8 or later.
* [CMake](https://cmake.org/) version 3.7 or later
* [Qt](https://www.qt.io/) version 5 or later
* C++17 capable compiler

## Building vsgQt

~~~ sh
cd vsgQt
cmake .
make -j 8
~~~

## Examples

* [vsgqtviewer](examples/vsgqtviewer/main.cpp) - example of QApplication/QMainWindow usage with single vsgQt::Window.
* [vsgqtmdi](examples/vsgqtmdi/main.cpp) - example of QMdiArea usage with multiple vsgQt::Window.
* [vsgqtwindows](examples/vsgqtwindows/main.cpp) - example of multiple vsgQt::Window.
