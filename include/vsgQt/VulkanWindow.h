#pragma once

namespace vsg
{
    class WindowTraits;
    class Instance;
}

class QEvent;

#include <vsgQt/ProxySurface.h>
#include <vsgQt/ProxyWindow.h>
#include <vsgQt/KeyboardMap.h>

#include <QWindow>

namespace vsgQt
{

    // forward declare
    class ProxySurface;
    class ProxyWindow;

    class VSGQT_DECLSPEC VulkanWindow : public QWindow
    {
    public:
        VulkanWindow();
        virtual ~VulkanWindow();

        vsg::ref_ptr<vsg::WindowTraits> traits;
        vsg::ref_ptr<vsg::Instance> instance;
        vsg::ref_ptr<ProxySurface> proxySurface;
        vsg::ref_ptr<ProxyWindow> proxyWindow;
        vsg::ref_ptr<KeyboardMap> keyboardMap;

    protected:
        void render();

        bool event(QEvent* e) override;

        void exposeEvent(QExposeEvent*) override;
        void keyPressEvent(QKeyEvent*) override;
        void keyReleaseEvent(QKeyEvent*) override;
        void mouseMoveEvent(QMouseEvent*) override;
        void mousePressEvent(QMouseEvent*) override;
        void mouseReleaseEvent(QMouseEvent*) override;
        void resizeEvent(QResizeEvent*) override;
        void moveEvent(QMoveEvent*) override;
        void wheelEvent(QWheelEvent*) override;

    private:
        bool _initialized = false;
        QVulkanInstance* vulkanInstance = nullptr;
    };

} // namespace vsgQt
