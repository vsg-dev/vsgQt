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

#include "ViewerWindow2.h"

using namespace vsgQt;

ViewerWindow2::ViewerWindow2() :
    QWindow()
{
    keyboardMap = vsgQt::KeyboardMap::create();
}

ViewerWindow2::~ViewerWindow2()
{
    std::cout << "ViewerWindow2::~ViewerWindow2() destrcutor" << std::endl;
    window = {};
}

void ViewerWindow2::render()
{
    std::cout<<"ViewerWindow2::render()"<<std::endl;

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
#if 0
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
#endif
}

bool ViewerWindow2::event(QEvent* e)
{
    switch (e->type())
    {
    case QEvent::UpdateRequest:
        render();
        break;

    case QEvent::PlatformSurface:
    {
        auto surfaceEvent = dynamic_cast<QPlatformSurfaceEvent*>(e);
        if (surfaceEvent->surfaceEventType()==QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)
        {
            std::cout<<"Need to clean up"<<std::endl;
            window = {};
        }
        std::cout<<"QEvent::PlatformSurface: "<<surfaceEvent<< " "<<surfaceEvent->surfaceEventType()<<std::endl;
    }
    default:
        break;
    }

    return QWindow::event(e);
}

void ViewerWindow2::exposeEvent(QExposeEvent* /*e*/)
{
    std::cout << "vulkanWindow.isExposed() = " << isExposed() << std::endl;
    if (!_initialized && isExposed())
    {
        _initialized = true;

        std::cout << "    initializaing ViewerWindow2" << std::endl;

        if (initializeCallback) initializeCallback(*this);

        requestUpdate();
    }
}

void ViewerWindow2::keyPressEvent(QKeyEvent* e)
{
    if (!window) return;

    std::cout << __func__ << std::endl;

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if (keyboardMap->getKeySymbol(e, keySymbol, modifiedKeySymbol, keyModifier))
    {
        vsg::clock::time_point event_time = vsg::clock::now();
        window->bufferedEvents.emplace_back(new vsg::KeyPressEvent(window, event_time, keySymbol, modifiedKeySymbol, keyModifier));
    }
}

void ViewerWindow2::keyReleaseEvent(QKeyEvent* e)
{
    if (!window) return;

    std::cout << __func__ << std::endl;

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if (keyboardMap->getKeySymbol(e, keySymbol, modifiedKeySymbol, keyModifier))
    {
        vsg::clock::time_point event_time = vsg::clock::now();
        window->bufferedEvents.emplace_back(new vsg::KeyReleaseEvent(window, event_time, keySymbol, modifiedKeySymbol, keyModifier));
    }
}

void ViewerWindow2::mouseMoveEvent(QMouseEvent* e)
{
    if (!window) return;

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

    window->bufferedEvents.emplace_back(new vsg::MoveEvent(window, event_time, e->x(), e->y(), (vsg::ButtonMask)button));
}

void ViewerWindow2::mousePressEvent(QMouseEvent* e)
{
    if (!window) return;

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

    window->bufferedEvents.emplace_back(new vsg::ButtonPressEvent(window, event_time, e->x(), e->y(), (vsg::ButtonMask)button, 0));
}

void ViewerWindow2::mouseReleaseEvent(QMouseEvent* e)
{
    if (!window) return;

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

    window->bufferedEvents.emplace_back(new vsg::ButtonReleaseEvent(window, event_time, e->x(), e->y(), (vsg::ButtonMask)button, 0));
}

void ViewerWindow2::resizeEvent(QResizeEvent* e)
{
    if (!window) return;

    // std::cout << __func__ << std::endl;

    vsg::clock::time_point event_time = vsg::clock::now();
    window->bufferedEvents.emplace_back(new vsg::ConfigureWindowEvent(window, event_time, x(), y(), static_cast<uint32_t>(e->size().width()), static_cast<uint32_t>(e->size().height())));
}

void ViewerWindow2::moveEvent(QMoveEvent* e)
{
    if (!window) return;

    // std::cout << __func__ << std::endl;

    vsg::clock::time_point event_time = vsg::clock::now();
    window->bufferedEvents.emplace_back(new vsg::ConfigureWindowEvent(window, event_time, e->pos().x(), e->pos().y(), static_cast<uint32_t>(size().width()), static_cast<uint32_t>(size().height())));
}

void ViewerWindow2::wheelEvent(QWheelEvent* e)
{
    if (!window) return;

    // std::cout << __func__ << std::endl;

    vsg::clock::time_point event_time = vsg::clock::now();
    window->bufferedEvents.emplace_back(new vsg::ScrollWheelEvent(window, event_time, e->angleDelta().y() < 0 ? vsg::vec3(0.0f, -1.0f, 0.0f) : vsg::vec3(0.0f, 1.0f, 0.0f)));
}
