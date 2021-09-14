#include <vsgQt/ProxySurface.h>
#include <vsgQt/ProxyWindow.h>
#include <vsgQt/ViewerWindow.h>

#include <QPlatformSurfaceEvent>
#include <QVulkanInstance>

using namespace vsgQt;

ProxyWindow::ProxyWindow(ViewerWindow* win,
                         vsg::ref_ptr<vsg::WindowTraits> traits) :
    Inherit(traits), _window(win)
{
    _instance = win->instance;

    if (traits->shareWindow)
    {
        share(*traits->shareWindow);
    }

    _extent2D.width = win->size().width();
    _extent2D.height = win->size().height();

    traits->nativeWindow = win;
}

bool ProxyWindow::pollEvents(vsg::UIEvents& events)
{
    if (bufferedEvents.size() > 0)
    {
        events.splice(events.end(), bufferedEvents);
        bufferedEvents.clear();
        return true;
    }

    return false;
}

bool ProxyWindow::resized() const
{
    const auto width = _window->width();
    const auto height = _window->height();
    return width != int(_extent2D.width) || height != int(_extent2D.height);
}

void ProxyWindow::resize()
{
    const auto width = _window->width();
    const auto height = _window->height();

    _extent2D.width = width;
    _extent2D.height = height;

    buildSwapchain();
}

const char* ProxyWindow::instanceExtensionSurfaceName() const
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

void ProxyWindow::_initSurface()
{
    _surface = ProxySurface::create(QVulkanInstance::surfaceForWindow(_window),
                                    _instance);
}

ProxyWindow::~ProxyWindow()
{
    clear();
}
