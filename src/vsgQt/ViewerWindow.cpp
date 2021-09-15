/* <editor-fold desc="MIT License">

Copyright(c) 2021 Robert Osfield, Andre Normann

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/all.h>
#ifdef vsgXchange_FOUND
#include <vsgXchange/all.h>
#endif

#include <QPlatformSurfaceEvent>
#include <QVulkanInstance>
#include <QWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <vulkan/vulkan.h>

#include <vsgQt/ProxyWindow.h>
#include <vsgQt/ViewerWindow.h>

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
    setSurfaceType(QSurface::VulkanSurface);
    keyboardMap = vsgQt::KeyboardMap::create();
}

ViewerWindow::~ViewerWindow()
{
    std::cout << "ViewerWindow::~ViewerWindow() destrcutor" << std::endl;
    delete vulkanInstance;
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
        else
        {
            QCoreApplication::exit(0);
        }
    }
    else if (viewer)
    {
        if (viewer->advanceToNextFrame())
        {
            // std::cout << __func__ << std::endl;
            viewer->handleEvents();
            viewer->update();
            viewer->recordAndSubmit();
            viewer->present();

            // continue rendering
            requestUpdate();
        }
        else
        {
            QCoreApplication::exit(0);
        }
    }
}

bool ViewerWindow::event(QEvent* e)
{
    //std::cout << __func__ << std::endl;
    switch (e->type())
    {
    case QEvent::UpdateRequest:
        render();
        break;

    default:
        break;
    }

    return QWindow::event(e);
}

void ViewerWindow::exposeEvent(QExposeEvent* e)
{
    std::cout << "vulkanWindow.isExposed() = " << isExposed() << std::endl;
    if (!_initialized && isExposed())
    {
        _initialized = true;

        std::cout << "    initializaing ViewerWindow" << std::endl;

        const auto rect = e->region().boundingRect();
        const uint32_t width = static_cast<uint32_t>(rect.width());
        const uint32_t height = static_cast<uint32_t>(rect.height());

        traits->width = width;
        traits->height = height;
        traits->fullscreen = false;

        std::cout << "    width = " << width << ", height = " << height
                  << std::endl;

        // create instance
        vsg::Names instanceExtensions;
        vsg::Names requestedLayers;

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
        vulkanInstance = new QVulkanInstance;
        vulkanInstance->setVkInstance(*instance);

        if (vulkanInstance->create())
        {
            // set up the window for Vulkan usage
            setVulkanInstance(vulkanInstance);

            proxyWindow = ProxyWindow::create(this, traits);

            vsg::clock::time_point event_time = vsg::clock::now();
            proxyWindow->bufferedEvents.emplace_back(new vsg::ExposeWindowEvent(proxyWindow, event_time, rect.x(), rect.y(), width, height));

            if (initializeCallback) initializeCallback(*this);

            requestUpdate();
        }
    }
}

void ViewerWindow::keyPressEvent(QKeyEvent* e)
{
    if (!proxyWindow) return;

    std::cout << __func__ << std::endl;

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if (keyboardMap->getKeySymbol(e, keySymbol, modifiedKeySymbol, keyModifier))
    {
        vsg::clock::time_point event_time = vsg::clock::now();
        proxyWindow->bufferedEvents.emplace_back(new vsg::KeyPressEvent(proxyWindow, event_time, keySymbol, modifiedKeySymbol, keyModifier));
    }
}

void ViewerWindow::keyReleaseEvent(QKeyEvent* e)
{
    if (!proxyWindow) return;

    std::cout << __func__ << std::endl;

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if (keyboardMap->getKeySymbol(e, keySymbol, modifiedKeySymbol, keyModifier))
    {
        vsg::clock::time_point event_time = vsg::clock::now();
        proxyWindow->bufferedEvents.emplace_back(new vsg::KeyReleaseEvent(proxyWindow, event_time, keySymbol, modifiedKeySymbol, keyModifier));
    }
}

void ViewerWindow::mouseMoveEvent(QMouseEvent* e)
{
    if (!proxyWindow) return;

    // std::cout << __func__ << std::endl;

    vsg::clock::time_point event_time = vsg::clock::now();

    int button = 0;
    switch (e->buttons())
    {
    case Qt::LeftButton: button |= vsg::BUTTON_MASK_1; break;
    case Qt::RightButton: button |= vsg::BUTTON_MASK_3; break;
    case Qt::MiddleButton: button |= vsg::BUTTON_MASK_2; break;
    case Qt::NoButton: button = 0; break;
    default: button = 0; break;
    }

    proxyWindow->bufferedEvents.emplace_back(new vsg::MoveEvent(proxyWindow, event_time, e->x(), e->y(), (vsg::ButtonMask)button));
}

void ViewerWindow::mousePressEvent(QMouseEvent* e)
{
    if (!proxyWindow) return;

    // std::cout << __func__ << std::endl;

    vsg::clock::time_point event_time = vsg::clock::now();

    int button = 0;
    switch (e->buttons())
    {
    case Qt::LeftButton: button |= vsg::BUTTON_MASK_1; break;
    case Qt::RightButton: button |= vsg::BUTTON_MASK_3; break;
    case Qt::MiddleButton: button |= vsg::BUTTON_MASK_2; break;
    case Qt::NoButton: button = 0; break;
    default: button = 0; break;
    }

    proxyWindow->bufferedEvents.emplace_back(new vsg::ButtonPressEvent(proxyWindow, event_time, e->x(), e->y(), (vsg::ButtonMask)button, 0));
}

void ViewerWindow::mouseReleaseEvent(QMouseEvent* e)
{
    if (!proxyWindow) return;

    // std::cout << __func__ << std::endl;

    vsg::clock::time_point event_time = vsg::clock::now();

    int button = 0;
    switch (e->buttons())
    {
    case Qt::LeftButton: button |= vsg::BUTTON_MASK_1; break;
    case Qt::RightButton: button |= vsg::BUTTON_MASK_3; break;
    case Qt::MiddleButton: button |= vsg::BUTTON_MASK_2; break;
    case Qt::NoButton: button = 0; break;
    default: button = 0; break;
    }

    proxyWindow->bufferedEvents.emplace_back(new vsg::ButtonReleaseEvent(proxyWindow, event_time, e->x(), e->y(), (vsg::ButtonMask)button, 0));
}

void ViewerWindow::resizeEvent(QResizeEvent* e)
{
    if (!proxyWindow) return;

    // std::cout << __func__ << std::endl;

    vsg::clock::time_point event_time = vsg::clock::now();
    proxyWindow->bufferedEvents.emplace_back(new vsg::ConfigureWindowEvent(proxyWindow, event_time, x(), y(), static_cast<uint32_t>(e->size().width()), static_cast<uint32_t>(e->size().height())));
}

void ViewerWindow::moveEvent(QMoveEvent* e)
{
    if (!proxyWindow) return;

    // std::cout << __func__ << std::endl;

    vsg::clock::time_point event_time = vsg::clock::now();
    proxyWindow->bufferedEvents.emplace_back(new vsg::ConfigureWindowEvent(proxyWindow, event_time, e->pos().x(), e->pos().y(), static_cast<uint32_t>(size().width()), static_cast<uint32_t>(size().height())));
}

void ViewerWindow::wheelEvent(QWheelEvent* e)
{
    if (!proxyWindow) return;

    // std::cout << __func__ << std::endl;

    vsg::clock::time_point event_time = vsg::clock::now();
    proxyWindow->bufferedEvents.emplace_back(new vsg::ScrollWheelEvent(proxyWindow, event_time, e->angleDelta().y() < 0 ? vsg::vec3(0.0f, -1.0f, 0.0f) : vsg::vec3(0.0f, 1.0f, 0.0f)));
}
