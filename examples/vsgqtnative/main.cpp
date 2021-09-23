#include <vsg/all.h>

#ifdef vsgXchange_FOUND
#    include <vsgXchange/all.h>
#endif

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <qpa/qplatformnativeinterface.h>
#include <QtPlatformHeaders/qxcbwindowfunctions.h>

#include <vsgQt/ViewerWindow.h>

#include "ViewerWindow2.h"

int main(int argc, char* argv[])
{
    vsg::CommandLine arguments(&argc, argv);

    // set up vsg::Options to pass in filepaths and ReaderWriter's and other IO
    // realted options to use when reading and writing files.
    auto options = vsg::Options::create();
    options->fileCache = vsg::getEnv("VSG_FILE_CACHE");
    options->paths = vsg::getEnvPaths("VSG_FILE_PATH");

#ifdef vsgXchange_all
    // add vsgXchange's support for reading and writing 3rd party file formats
    options->add(vsgXchange::all::create());
#endif

    arguments.read(options);

    auto windowTraits = vsg::WindowTraits::create();
    windowTraits->windowTitle = "vsgQt viewer";
    windowTraits->debugLayer = arguments.read({"--debug", "-d"});
    windowTraits->apiDumpLayer = arguments.read({"--api", "-a"});
    if (arguments.read({"--fullscreen", "--fs"})) windowTraits->fullscreen = true;
    if (arguments.read({"--window", "-w"}, windowTraits->width,windowTraits->height))
    {
        windowTraits->fullscreen = false;
    }
    auto horizonMountainHeight = arguments.value(0.0, "--hmh");

    if (arguments.errors())
        return arguments.writeErrorMessages(std::cerr);

    if (argc <= 1)
    {
        std::cout << "Please specify a 3d model or image file on the command line."
                  << std::endl;
        return 1;
    }

    vsg::Path filename = arguments[1];

    auto vsg_scene = vsg::read_cast<vsg::Node>(filename, options);
    if (!vsg_scene)
    {
        std::cout << "Failed to load a valid scenene graph, Please specify a 3d "
                     "model or image file on the command line."
                  << std::endl;
        return 1;
    }


    QApplication application(argc, argv);

    auto mainWindow = new QMainWindow();


    //auto viewerWindow = new QWindow();
    auto viewerWindow = new vsgQt::ViewerWindow2();

    windowTraits->systemConnection = static_cast<xcb_connection_t *>(application.platformNativeInterface()->nativeResourceForIntegration(QByteArrayLiteral("connection")));
    windowTraits->nativeWindow = static_cast<xcb_window_t>(viewerWindow->winId());

    auto widget = QWidget::createWindowContainer(viewerWindow, mainWindow);
    mainWindow->setCentralWidget(widget);

    mainWindow->resize(windowTraits->width, windowTraits->height);

    mainWindow->show();

    // create the viewer and assign window(s) to it
    auto viewer = vsg::Viewer::create();
    auto window = vsg::Window::create(windowTraits);
    if (!window)
    {
        std::cout << "Could not create windows." << std::endl;
        return 1;
    }

    //viewerWindow->viewer = viewer;
    viewerWindow->window = window;

    std::cout << "Created VSG window." <<window<< std::endl;

    viewer->addWindow(window);

    // compute the bounds of the scene graph to help position camera
    vsg::ComputeBounds computeBounds;
    vsg_scene->accept(computeBounds);
    vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;
    double radius = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min) * 0.6;
    double nearFarRatio = 0.001;

    // set up the camera
    auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, -radius * 3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));

    vsg::ref_ptr<vsg::ProjectionMatrix> perspective;
    vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel(vsg_scene->getObject<vsg::EllipsoidModel>("EllipsoidModel"));
    if (ellipsoidModel)
    {
        perspective = vsg::EllipsoidPerspective::create(
            lookAt, ellipsoidModel, 30.0,
            static_cast<double>(window->extent2D().width) /
                static_cast<double>(window->extent2D().height),
            nearFarRatio, horizonMountainHeight);
    }
    else
    {
        perspective = vsg::Perspective::create(
            30.0,
            static_cast<double>(window->extent2D().width) /
                static_cast<double>(window->extent2D().height),
            nearFarRatio * radius, radius * 4.5);
    }

    auto camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(window->extent2D()));

    // add close handler to respond the close window button and pressing escape
    viewer->addEventHandler(vsg::CloseHandler::create(viewer));

    // add trackball to enable mouse driven camera view control.
    viewer->addEventHandler(vsg::Trackball::create(camera, ellipsoidModel));

    viewer->assignRecordAndSubmitTaskAndPresentation({vsg::createCommandGraphForView(window, camera, vsg_scene)});

    viewer->compile();

    qDebug()<<"C mainWindow->winId() = "<<mainWindow->winId();
    qDebug()<<"D viewerWindow->winId() = "<<viewerWindow->winId();

    // quick test of QWindow::fromWinId functionality.
    //auto qtWindow = QWindow::fromWinId(std::any_cast<xcb_window_t>(windowTraits->nativeWindow));
    //qDebug()<<"qtWindow = "<< qtWindow;

    auto frameLoop = [](vsg::ref_ptr<vsg::Viewer> local_viewer)
    {
        while (local_viewer->advanceToNextFrame())
        {
            std::cout<<"Frame"<<std::endl;

            // pass any events into EventHandlers assigned to the Viewer
            local_viewer->handleEvents();

            local_viewer->update();

            local_viewer->recordAndSubmit();

            local_viewer->present();
        }
        std::cout<<"Exciting frameloop"<<std::endl;
    };

    std::thread frameThread(frameLoop, viewer);

    vsg_scene = {};
    viewer = {};
    window = {};

    bool result = application.exec();

    //viewer->status->set(false);
    //viewer->close();

    std::cout<<"Exiting A "<<result<<std::endl;


    std::cout<<"Exiting B "<<result<<std::endl;

    //frameThread.join();

    std::cout<<"Exiting C "<<result<<std::endl;

    return result;
}
