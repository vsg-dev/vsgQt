#pragma once

#include <vsg/viewer/Window.h>

namespace vsgQt
{

    // forward declare
    class VulkanWindow;

    class ProxyWindow : public vsg::Inherit<vsg::Window, ProxyWindow>
    {
    public:
        ProxyWindow(VulkanWindow* win, vsg::ref_ptr<vsg::WindowTraits> traits);

        ProxyWindow() = delete;
        ProxyWindow(const Window&) = delete;
        ProxyWindow operator=(const Window&) = delete;

        bool visible() const override { return _window != nullptr; }
        bool valid() const override { return _window != nullptr; }

        bool pollEvents(vsg::UIEvents& events) override;

        bool resized() const override;

        void resize() override;

        virtual const char* instanceExtensionSurfaceName() const override;

        vsg::UIEvents bufferedEvents;

    protected:
        virtual void _initSurface() override;

        virtual ~ProxyWindow() override;

        VulkanWindow* _window = nullptr;
    };

} // namespace vsgQt
