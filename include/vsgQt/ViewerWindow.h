#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2021 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <QPlatformSurfaceEvent>
#include <QVulkanInstance>
#include <QWindow>

#include <vsgQt/KeyboardMap.h>
#include <vsgQt/ProxySurface.h>
#include <vsgQt/ProxyWindow.h>

namespace vsgQt
{

    class VSGQT_DECLSPEC ViewerWindow : public QWindow
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
