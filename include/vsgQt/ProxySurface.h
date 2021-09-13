#pragma once

#include <vsg/vk/Surface.h>

#include <vsgQt/Export.h>

namespace vsgQt
{

    class VSGQT_DECLSPEC ProxySurface : public vsg::Inherit<vsg::Surface, ProxySurface>
    {
    public:
        ProxySurface(VkSurfaceKHR surface, vsg::Instance* instance);

    protected:
        virtual ~ProxySurface();
    };

} // namespace vsgQt
