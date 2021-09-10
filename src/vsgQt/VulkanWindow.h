#pragma once


#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QVulkanInstance>
#include <QWindow>
#include <QPlatformSurfaceEvent>

#include <vulkan/vulkan.h>

#include "ProxySurface.h"
#include "ProxyWindow.h"

namespace vsgQt
{

class VulkanWindow : public QWindow
{
public:

    VulkanWindow();
    virtual ~VulkanWindow();

    vsg::ref_ptr<vsg::WindowTraits> traits;
    vsg::ref_ptr<vsg::Instance> instance;
    vsg::ref_ptr<ProxySurface> proxySurface;
    vsg::ref_ptr<ProxyWindow> proxyWindow;

protected:

    bool event(QEvent *e) override { return QWindow::event(e); } // QEvent::UpdateRequest -> render winndow?

    void exposeEvent(QExposeEvent *e) override;
    void keyPressEvent(QKeyEvent *) override {};
    void keyReleaseEvent(QKeyEvent *) override {};
    void mouseMoveEvent(QMouseEvent *) override {};
    void mousePressEvent(QMouseEvent *) override {};
    void mouseReleaseEvent(QMouseEvent *) override {};
    void resizeEvent(QResizeEvent *) override {};
    void moveEvent(QMoveEvent *) override {};
    void wheelEvent(QWheelEvent *) override {};

private:

    bool _initialized = false;
    QVulkanInstance* vulkanInstance = nullptr;
};

}
