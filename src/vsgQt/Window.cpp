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

#include <vsgQt/Window.h>

#include <iostream>

using namespace vsgQt;

Window::Window(QWindow* parent) :
    QWindow(parent),
    traits(vsg::WindowTraits::create()),
    keyboardMap(KeyboardMap::create())
{
    traits->width = width();
    traits->height = height();
}

Window::Window(vsg::ref_ptr<vsg::Viewer> in_viewer, QWindow* parent) :
    QWindow(parent),
    traits(vsg::WindowTraits::create()),
    viewer(in_viewer),
    keyboardMap(KeyboardMap::create())
{
    traits->width = width();
    traits->height = height();
}

Window::~Window()
{
    cleanup();
}

void Window::cleanup()
{
    // remove links to all the VSG related classes.
    if (windowAdapter)
    {
        // wait for all rendering to be completed before we start cleaning up resources.
        if (viewer) viewer->deviceWaitIdle();

        viewer->removeWindow(windowAdapter);
        windowAdapter->releaseWindow();
    }

    windowAdapter = {};
    viewer = {};
}

void Window::render()
{
    // vsg::info("Window::render() viewer = ", viewer);

    if (continuousUpdate) requestUpdate();

    if (!viewer) return;

    if (frameCallback)
    {
        if (frameCallback(*this))
        {
            // continue rendering
            //if (continuousUpdate) requestUpdate();
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
            //if (continuousUpdate) requestUpdate();
        }
        else if (viewer->status->cancel())
        {
            cleanup();
            QCoreApplication::exit(0);
        }
    }
}

bool Window::event(QEvent* e)
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
            vsg::debug("Window::event(QEvent* e) QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)");
            cleanup();
        }
        break;
    }

    default:
        vsg::debug("Window::event(QEvent* e) type = ", e->type(), " not handled");
        break;
    }

    return QWindow::event(e);
}

void Window::initializeWindow()
{
    if (windowAdapter) return;

    if (!traits) traits = vsg::WindowTraits::create();

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    traits->nativeWindow = reinterpret_cast<HWND>(winId());
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    traits->nativeWindow = static_cast<::Window>(winId());
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    traits->nativeWindow = static_cast<xcb_window_t>(winId());
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
    traits->nativeWindow = reinterpret_cast<NSView*>(winId()); // or NSWindow* ?
#endif

    traits->width = convert_coord(width());
    traits->height = convert_coord(height());

    windowAdapter = vsg::Window::create(traits);
}

void Window::initializeViewer()
{
    if (_initialized) return;

    _initialized = true;

    initializeWindow();

    if (initializeCallback) initializeCallback(*this, traits->width, traits->height);
}

void Window::exposeEvent(QExposeEvent* /*e*/)
{
    if (!_initialized && isExposed())
    {
        initializeViewer();
    }
    requestUpdate();
}

void Window::hideEvent(QHideEvent* /*e*/)
{
}

void Window::resizeEvent(QResizeEvent* /*e*/)
{
    if (!windowAdapter) return;

    vsg::clock::time_point event_time = vsg::clock::now();
    windowAdapter->bufferedEvents.push_back(vsg::ConfigureWindowEvent::create(windowAdapter, event_time, convert_coord(x()), convert_coord(y()), convert_coord(width()), convert_coord(height())));

    if (!continuousUpdate) requestUpdate();
}

void Window::keyPressEvent(QKeyEvent* e)
{
    if (!windowAdapter) return;

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if (keyboardMap->getKeySymbol(e, keySymbol, modifiedKeySymbol, keyModifier))
    {
        vsg::clock::time_point event_time = vsg::clock::now();
        windowAdapter->bufferedEvents.push_back(vsg::KeyPressEvent::create(windowAdapter, event_time, keySymbol, modifiedKeySymbol, keyModifier));
    }

    if (!continuousUpdate) requestUpdate();
}

void Window::keyReleaseEvent(QKeyEvent* e)
{
    if (!windowAdapter) return;

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if (keyboardMap->getKeySymbol(e, keySymbol, modifiedKeySymbol, keyModifier))
    {
        vsg::clock::time_point event_time = vsg::clock::now();
        windowAdapter->bufferedEvents.push_back(vsg::KeyReleaseEvent::create(windowAdapter, event_time, keySymbol, modifiedKeySymbol, keyModifier));
    }

    if (!continuousUpdate) requestUpdate();
}

void Window::mouseMoveEvent(QMouseEvent* e)
{
    if (!windowAdapter) return;

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons(e);
    auto [x, y] = convertMousePosition(e);

    windowAdapter->bufferedEvents.push_back(vsg::MoveEvent::create(windowAdapter, event_time, x, y, mask));

    if (!continuousUpdate) requestUpdate();
}

void Window::mousePressEvent(QMouseEvent* e)
{
    if (!windowAdapter) return;

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons(e);
    auto [x, y] = convertMousePosition(e);

    windowAdapter->bufferedEvents.push_back(vsg::ButtonPressEvent::create(windowAdapter, event_time, x, y, mask, button));

    if (!continuousUpdate) requestUpdate();
}

void Window::mouseReleaseEvent(QMouseEvent* e)
{
    if (!windowAdapter) return;

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons(e);
    auto [x, y] = convertMousePosition(e);

    windowAdapter->bufferedEvents.push_back(vsg::ButtonReleaseEvent::create(windowAdapter, event_time, x, y, mask, button));

    if (!continuousUpdate) requestUpdate();
}

void Window::wheelEvent(QWheelEvent* e)
{
    if (!windowAdapter) return;

    vsg::clock::time_point event_time = vsg::clock::now();
    windowAdapter->bufferedEvents.push_back(vsg::ScrollWheelEvent::create(windowAdapter, event_time, e->angleDelta().y() < 0 ? vsg::vec3(0.0f, -1.0f, 0.0f) : vsg::vec3(0.0f, 1.0f, 0.0f)));

    if (!continuousUpdate) requestUpdate();
}

std::pair<vsg::ButtonMask, uint32_t> Window::convertMouseButtons(QMouseEvent* e) const
{
    uint16_t mask{0};
    uint32_t button = 0;

    if (e->buttons() & Qt::LeftButton) mask = mask | vsg::BUTTON_MASK_1;
    if (e->buttons() & Qt::MiddleButton) mask = mask | vsg::BUTTON_MASK_2;
    if (e->buttons() & Qt::RightButton) mask = mask | vsg::BUTTON_MASK_3;

    switch (e->button())
    {
    case Qt::LeftButton: button = 1; break;
    case Qt::MiddleButton: button = 2; break;
    case Qt::RightButton: button = 3; break;
    default: break;
    }

    return {static_cast<vsg::ButtonMask>(mask), button};
}

std::pair<int32_t, int32_t> Window::convertMousePosition(QMouseEvent* e) const
{
#if QT_VERSION_MAJOR == 6
    return {convert_coord(e->position().x()), convert_coord(e->position().y())};
#else
    return {convert_coord(e->x()), convert_coord(e->y())};
#endif
}
