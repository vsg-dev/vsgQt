/* <editor-fold desc="MIT License">

Copyright(c) 2021 Robert Osfield, Andre Normann

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#if defined(WIN32)
#    define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__APPLE__)
#    define VK_USE_PLATFORM_MACOS_MVK
#else
#    define VK_USE_PLATFORM_XCB_KHR
// #define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vsg/all.h>

#include <QPlatformSurfaceEvent>
#include <QWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <vulkan/vulkan.h>

#include <vsgQt/Renderer.h>

#include <iostream>

using namespace vsgQt;

Renderer::Renderer(vsg::ref_ptr<vsg::Viewer> in_viewer) :
    viewer(in_viewer)
{
}
void Renderer::request()
{
    ++requests;
}

void Renderer::render()
{

    if (!continuousUpdate && requests.load() == 0)
    {
        // vsg::info("render() no render : requests = ", requests.load());
        return;
    }

    if (viewer->advanceToNextFrame())
    {
        vsg::info("render() doing rendering requests = ", requests.load());
        viewer->handleEvents();
        viewer->update();
        viewer->recordAndSubmit();
        viewer->present();
    }
    else
    {
        vsg::info("render() render but viewer->advanceToNextFrame() returns false : requests = ", requests.load());
    }

    requests = 0;
}

void Renderer::setInterval(int msec)
{
    timer.setInterval(msec);
    timer.connect(&timer, &QTimer::timeout, [&](){ vsg::info("tick"); render(); } );
    timer.start();
}
