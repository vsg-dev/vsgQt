#pragma once

#include <vsg/vk/Surface.h>

namespace vsgQt
{

    class ProxySurface : public vsg::Inherit<vsg::Surface, ProxySurface>
    {
    public:
        ProxySurface(VkSurfaceKHR surface, vsg::Instance* instance);

    protected:
        virtual ~ProxySurface();
    };

} // namespace vsgQt
