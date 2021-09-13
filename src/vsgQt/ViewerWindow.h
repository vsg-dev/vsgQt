#pragma once

#include <QPlatformSurfaceEvent>
#include <QVulkanInstance>
#include <QWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <vulkan/vulkan.h>

#include "KeyboardMap.h"
#include "ProxySurface.h"
#include "ProxyWindow.h"

namespace vsgQt
{

    class ViewerWindow : public QWindow
    {
    public:
        ViewerWindow();
        virtual ~ViewerWindow();

        vsg::ref_ptr<vsg::WindowTraits> traits;
        vsg::ref_ptr<vsg::Instance> instance;
        vsg::ref_ptr<vsg::Viewer> viewer;

        vsg::ref_ptr<ProxySurface> proxySurface;
        vsg::ref_ptr<ProxyWindow> proxyWindow;
        vsg::ref_ptr<KeyboardMap> keyboardMap;

        using InitialCallback = std::function<void(ViewerWindow&)>;
        InitialCallback initializeCallback;

        using FrameCallback = std::function<bool(ViewerWindow&)>;
        FrameCallback frameCallback;

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
