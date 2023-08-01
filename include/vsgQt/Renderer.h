#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2021 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/app/Viewer.h>

#include <vsgQt/Export.h>

#include <QTimer>

namespace vsgQt
{

    // forward declare
    class Window;

    class VSGQT_DECLSPEC CustomViewer : public vsg::Inherit<vsg::Viewer, CustomViewer>
    {
    public:
        CustomViewer() {};

        /// override pollEvents to prevent the window->pollEvents() being called by vsg::Viewer
        bool pollEvents(bool discardPreviousEvents = true) override;
    };

    class VSGQT_DECLSPEC Renderer : public vsg::Inherit<vsg::Object, Renderer>
    {
    public:

        Renderer(vsg::ref_ptr<vsg::Viewer> in_viewer);

        vsg::ref_ptr<vsg::Viewer> viewer;
        QTimer timer;
        std::atomic_uint requests;
        bool continuousUpdate = true;

        void request();

        void removeWindow(Window* window);

        void render();

        void setInterval(int msec);

    };

} // namespace vsgQt

EVSG_type_name(vsgQt::Renderer);
