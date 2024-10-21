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

    class VSGQT_DECLSPEC Viewer : public vsg::Inherit<vsg::Viewer, Viewer>
    {
    public:
        /// Create Viewer.
        /// If msecTimerInterval is > 0 then call setInterval(msecTimerInterval) to set up QTime to call Viewer::render() at this interval,
        /// otherwise the redraw is left application to call Viewer::render() when required, call Viewer::setInverval() to set up the timer,
        /// or to call the underlying vsg::Viewer methods for rendering a framne within the application when the viewer needs to redraw a frame.
        Viewer(int msecTimerInterval = 0);

        QTimer timer;
        std::atomic_uint requests;
        bool continuousUpdate = true;

        /// override pollEvents to prevent the window->pollEvents() from being called by vsg::Viewer
        bool pollEvents(bool discardPreviousEvents = true) override;

        /// increment the requests count to signal that a new frame should be rendered on the next timer call.
        virtual void request();

        /// called by the QTimer and will do the viewer frame calls to render all windows associated with the viewer.
        /// if continuousUpdate is false then the viewer frame calls are only done if the requests count is > 0
        virtual void render(double simulationTime = vsg::Viewer::UseTimeSinceStartPoint);

        /// set the QTimer interval in milliseconds, this controls how often the Viewer::render() is called
        void setInterval(int msecTimerInterval);
    };

} // namespace vsgQt

EVSG_type_name(vsgQt::Viewer);
