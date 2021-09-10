#include <vsg/all.h>
#include <vsgXchange/all.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QVulkanInstance>
#include <QWindow>
#include <QPlatformSurfaceEvent>


#include <vulkan/vulkan.h>

#include "ProxyWindow.h"
#include "VulkanWindow.h"

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

VulkanWindow::VulkanWindow() : QWindow()
{
    setSurfaceType(QSurface::VulkanSurface);
}

VulkanWindow::~VulkanWindow()
{
    std::cout<<"VulkanWindow::~VulkanWindow() destrcutor"<<std::endl;
    delete vulkanInstance;
}

void VulkanWindow::exposeEvent(QExposeEvent* e)
{
    std::cout<<"vulkanWindow.isExposed() = "<<isExposed()<<std::endl;
    if (!_initialized && isExposed())
    {
        _initialized = true;


        std::cout<<"    initializaing VulkanWindow"<<std::endl;

        const auto rect = e->region().boundingRect();
        const uint32_t width = static_cast<uint32_t>(rect.width());
        const uint32_t height = static_cast<uint32_t>(rect.height());

        traits->width = width;
        traits->height = height;
        traits->fullscreen = false;


        std::cout<<"    width = "<<width<<", height = "<<height<<std::endl;

        // create instance
        vsg::Names instanceExtensions;
        vsg::Names requestedLayers;

        instanceExtensions.push_back("VK_KHR_surface");
        instanceExtensions.push_back(instanceExtensionSurfaceName());

        if (traits->debugLayer || traits->apiDumpLayer)
        {
            instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            requestedLayers.push_back("VK_LAYER_KHRONOS_validation");
            if (traits->apiDumpLayer) requestedLayers.push_back("VK_LAYER_LUNARG_api_dump");
        }

        vsg::Names validatedNames = vsg::validateInstancelayerNames(requestedLayers);

        instance = vsg::Instance::create(instanceExtensions, validatedNames);

        // create Qt wrapper of vkInstance
        vulkanInstance = new QVulkanInstance;
        vulkanInstance->setVkInstance(*instance);

        if (vulkanInstance->create())
        {
            // set up the window for Vulkan usage
            setVulkanInstance(vulkanInstance);

            proxyWindow = ProxyWindow::create(this, traits);
        }
    }
}
