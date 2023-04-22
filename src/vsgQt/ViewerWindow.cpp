/* <editor-fold desc="MIT License">

Copyright(c) 2021 Robert Osfield, Andre Normann

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/all.h>

#include <QPlatformSurfaceEvent>
#include <QWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <vulkan/vulkan.h>

#include <vsgQt/ViewerWindow.h>

#include <vsg/app/WindowAdapter.h>

#if QT_HAS_VULKAN_SUPPORT
#    include <QVulkanInstance>
#endif

#include <iostream>

#if defined(__APPLE__)
    extern "C" { void makeViewMetalCompatible(void* handle); }
#endif

using namespace vsgQt;

const char* instanceExtensionSurfaceName()
{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    return VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
    return VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
#endif
}

ViewerWindow::ViewerWindow() :
    QWindow()
{
    keyboardMap = vsgQt::KeyboardMap::create();
}

ViewerWindow::~ViewerWindow()
{
    cleanup();
}

void ViewerWindow::cleanup()
{
    // remove links to all the VSG related classes.
    if (windowAdapter)
    {
#if QT_HAS_VULKAN_SUPPORT
        if (surfaceType() == QSurface::VulkanSurface)
        {
            windowAdapter->getSurface()->release();
        }
        else
#endif
        {
            windowAdapter->releaseWindow();
        }
    }

    windowAdapter = {};
    viewer = {};
}

void ViewerWindow::render()
{
    if (frameCallback)
    {
        if (frameCallback(*this))
        {
            // continue rendering
            requestUpdate();
        }
        else if (viewer->status->cancel())
        {
            cleanup();
            QCoreApplication::exit(0);
        }
    }
    else if (viewer)
    {
        if (viewer->advanceToNextFrame())
        {
            viewer->handleEvents();
            viewer->update();
            viewer->recordAndSubmit();
            viewer->present();

            // continue rendering
            requestUpdate();
        }
        else if (viewer->status->cancel())
        {
            cleanup();
            QCoreApplication::exit(0);
        }
    }
}

bool ViewerWindow::event(QEvent* e)
{
    switch (e->type())
    {
    case QEvent::UpdateRequest:
        render();
        break;

    case QEvent::PlatformSurface: {
        auto surfaceEvent = dynamic_cast<QPlatformSurfaceEvent*>(e);
        if (surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)
        {
            cleanup();
        }
    }

    default:
        break;
    }

    return QWindow::event(e);
}

void ViewerWindow::intializeUsingAdapterWindow(uint32_t width, uint32_t height)
{
#if QT_HAS_VULKAN_SUPPORT
    _initialized = true;

    traits->width = width;
    traits->height = height;
    traits->fullscreen = false;

    // create instance
    vsg::Names instanceExtensions;
    vsg::Names requestedLayers;

    instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instanceExtensions.push_back("VK_KHR_surface");
    instanceExtensions.push_back(instanceExtensionSurfaceName());

    if (traits->debugLayer || traits->apiDumpLayer)
    {
        instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        requestedLayers.push_back("VK_LAYER_KHRONOS_validation");
        if (traits->apiDumpLayer)
            requestedLayers.push_back("VK_LAYER_LUNARG_api_dump");
    }

    vsg::Names validatedNames =
        vsg::validateInstancelayerNames(requestedLayers);

    instance = vsg::Instance::create(instanceExtensions, validatedNames);

    // create Qt wrapper of vkInstance
    auto vulkanInstance = new QVulkanInstance;
    vulkanInstance->setVkInstance(*instance);

    if (vulkanInstance->create())
    {
        // set up the window for Vulkan usage
        setVulkanInstance(vulkanInstance);

        auto surface = vsg::Surface::create(QVulkanInstance::surfaceForWindow(this), instance);
        windowAdapter = new vsg::WindowAdapter(surface, traits);

        // vsg::clock::time_point event_time = vsg::clock::now();
        // windowAdapter->bufferedEvents.emplace_back(new vsg::ExposeWindowEvent(windowAdapter, event_time, rect.x(), rect.y(), width, height));
    }
    else
    {
        delete vulkanInstance;
    }
#else
    vsg::info("ViewerWindow::intializeUsingAdapterWindow(", width, ", ", height, ") not supported, requires Qt 5.10 or later.");
#endif
}

void ViewerWindow::intializeUsingVSGWindow(uint32_t width, uint32_t height)
{
    _initialized = true;

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    traits->nativeWindow = reinterpret_cast<HWND>(winId());
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    traits->nativeWindow = static_cast<::Window>(winId());
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    traits->nativeWindow = static_cast<xcb_window_t>(winId());
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
    #if defined(__APPLE__)
    void* handle = (void*)winId();
    makeViewMetalCompatible(handle);
    //traits->nativeWindow = reinterpret_cast<NSView*>(winId()); // or NSWindow* ?
    traits->nativeWindow = handle;
    #endif
#endif

    traits->width = width;
    traits->height = height;

    windowAdapter = vsg::Window::create(traits);
}

void ViewerWindow::exposeEvent(QExposeEvent* /*e*/)
{
    if (!_initialized && isExposed())
    {
#if QT_HAS_VULKAN_SUPPORT
        if (surfaceType() == QSurface::VulkanSurface)
        {
            vsg::info("Using QSurface");
            intializeUsingAdapterWindow(convert_coord(width()), convert_coord(height()));
        }
        else
#endif
        {
            vsg::info("Using vsg::Surface");
            intializeUsingVSGWindow(convert_coord(width()), convert_coord(height()));
        }

        if (initializeCallback) initializeCallback(*this, convert_coord(width()), convert_coord(height()));


        requestUpdate();
    }

    if (auto adapter = windowAdapter.cast<vsg::WindowAdapter>(); adapter)
    {
        adapter->windowValid = true;
        adapter->windowVisible = isExposed();
    }
}

void ViewerWindow::hideEvent(QHideEvent* /*e*/)
{
    if (auto adapter = windowAdapter.cast<vsg::WindowAdapter>(); adapter)
    {
        adapter->windowVisible = false;
    }
}

void ViewerWindow::resizeEvent(QResizeEvent* /*e*/)
{
    if (!windowAdapter) return;

    // WindowAdapter
    if (auto adapter = windowAdapter.cast<vsg::WindowAdapter>(); adapter)
    {
        adapter->updateExtents(convert_coord(width()), convert_coord(height()));
    }

    vsg::clock::time_point event_time = vsg::clock::now();
    windowAdapter->bufferedEvents.push_back(vsg::ConfigureWindowEvent::create(windowAdapter, event_time, convert_coord(x()), convert_coord(y()), convert_coord(width()), convert_coord(height())));
}

void ViewerWindow::keyPressEvent(QKeyEvent* e)
{
    if (!windowAdapter) return;

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if (keyboardMap->getKeySymbol(e, keySymbol, modifiedKeySymbol, keyModifier))
    {
        vsg::clock::time_point event_time = vsg::clock::now();
        windowAdapter->bufferedEvents.push_back(vsg::KeyPressEvent::create(windowAdapter, event_time, keySymbol, modifiedKeySymbol, keyModifier));
    }
}

void ViewerWindow::keyReleaseEvent(QKeyEvent* e)
{
    if (!windowAdapter) return;

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if (keyboardMap->getKeySymbol(e, keySymbol, modifiedKeySymbol, keyModifier))
    {
        vsg::clock::time_point event_time = vsg::clock::now();
        windowAdapter->bufferedEvents.push_back(vsg::KeyReleaseEvent::create(windowAdapter, event_time, keySymbol, modifiedKeySymbol, keyModifier));
    }
}

void ViewerWindow::mouseMoveEvent(QMouseEvent* e)
{
    if (!windowAdapter) return;

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons(e);
    auto [x, y] = convertMousePosition(e);

    windowAdapter->bufferedEvents.push_back(vsg::MoveEvent::create(windowAdapter, event_time, x, y, mask));
}

void ViewerWindow::mousePressEvent(QMouseEvent* e)
{
    if (!windowAdapter) return;

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons(e);
    auto [x, y] = convertMousePosition(e);

    windowAdapter->bufferedEvents.push_back(vsg::ButtonPressEvent::create(windowAdapter, event_time, x, y, mask, button));
}


void ViewerWindow::mouseReleaseEvent(QMouseEvent* e)
{
    if (!windowAdapter) return;

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons(e);
    auto [x, y] = convertMousePosition(e);

    windowAdapter->bufferedEvents.push_back(vsg::ButtonReleaseEvent::create(windowAdapter, event_time, x, y, mask, button));
}

void ViewerWindow::wheelEvent(QWheelEvent* e)
{
    if (!windowAdapter) return;

    vsg::clock::time_point event_time = vsg::clock::now();
    windowAdapter->bufferedEvents.push_back(vsg::ScrollWheelEvent::create(windowAdapter, event_time, e->angleDelta().y() < 0 ? vsg::vec3(0.0f, -1.0f, 0.0f) : vsg::vec3(0.0f, 1.0f, 0.0f)));
}

std::pair<vsg::ButtonMask, uint32_t> ViewerWindow::convertMouseButtons(QMouseEvent* e) const
{
    uint16_t mask{0};
    uint32_t button = 0;

    if (e->buttons() & Qt::LeftButton) mask = mask | vsg::BUTTON_MASK_1;
    if (e->buttons() & Qt::MiddleButton) mask = mask | vsg::BUTTON_MASK_2;
    if (e->buttons() & Qt::RightButton) mask = mask | vsg::BUTTON_MASK_3;

    switch(e->button())
    {
        case Qt::LeftButton: button = 1; break;
        case Qt::MiddleButton: button = 2; break;
        case Qt::RightButton: button = 3; break;
        default: break;
    }

    return {static_cast<vsg::ButtonMask>(mask), button};
}

std::pair<int32_t, int32_t> ViewerWindow::convertMousePosition(QMouseEvent* e) const
{
#if QT_VERSION_MAJOR == 6
    return {convert_coord(e->position().x()), convert_coord(e->position().y())};
#else
    return {convert_coord(e->x()), convert_coord(e->y())};
#endif
}
