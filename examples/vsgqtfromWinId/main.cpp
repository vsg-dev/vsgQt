#include <vsg/all.h>

#ifdef vsgXchange_FOUND
#    include <vsgXchange/all.h>
#endif

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <qpa/qplatformnativeinterface.h>
#include <QtPlatformHeaders/qxcbwindowfunctions.h>

#include <vsgQt/ViewerWindow.h>

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



    // create the viewer and assign window(s) to it
    auto viewer = vsg::Viewer::create();
    auto window = vsg::Window::create(windowTraits);
    if (!window)
    {
        std::cout << "Could not create windows." << std::endl;
        return 1;
    }

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

    auto commandGraph = vsg::createCommandGraphForView(window, camera, vsg_scene);
    viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    viewer->compile();

    QApplication application(argc, argv);

    auto mainWindow = new QMainWindow();

    auto viewerWindow = QWindow::fromWinId(std::any_cast<xcb_window_t>(windowTraits->nativeWindow));

    auto widget = QWidget::createWindowContainer(viewerWindow, mainWindow);
    mainWindow->setCentralWidget(widget);

    mainWindow->resize(windowTraits->width, windowTraits->height);

    mainWindow->show();

    std::thread frameThread([&viewer](){
        // take a reference of the viewer to prevent it going out of scope while we are still running
        auto local_viewer = viewer;
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
    });


    bool result = application.exec();

    viewer->status->set(false);

    std::cout<<"Exiting "<<result<<std::endl;

    frameThread.join();

    return result;
}
