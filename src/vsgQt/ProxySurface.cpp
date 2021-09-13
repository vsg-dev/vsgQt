#include <vsgQt/ProxySurface.h>

#include <iostream>

using namespace vsgQt;

ProxySurface::ProxySurface(VkSurfaceKHR surface, vsg::Instance* instance) :
    Inherit(surface, instance)
{
    std::cout << __func__ << " vkSurface = " << surface << std::endl;
}

ProxySurface::~ProxySurface()
{
    std::cout << __func__ << " vkSurface = " << _surface << std::endl;
    // Prevent vsg::Surface destructor from calling vkDestroySurfaceKHR on this
    // surface as QtWindow owns the surface.
    _surface = 0;
}
