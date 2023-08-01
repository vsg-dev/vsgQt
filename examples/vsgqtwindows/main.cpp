#include <vsg/all.h>

#ifdef vsgXchange_FOUND
#    include <vsgXchange/all.h>
#endif

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>

#include <vsgQt/Window.h>
#include <iostream>

#include <iostream>

vsgQt::Window* createWindow(vsg::ref_ptr<vsgQt::Renderer> renderer, vsg::ref_ptr<vsg::WindowTraits> traits, vsg::ref_ptr<vsg::Node> vsg_scene, QWindow* parent, const QString& title = {})
{
    auto window = new vsgQt::Window(renderer, traits, parent);

    window->setTitle(title);

    window->initializeWindow();

    // if first window to be created use it's device for future window creation.
    if (!traits->device) traits->device = window->windowAdapter->getOrCreateDevice();

    // compute the bounds of the scene graph to help position camera
    vsg::ComputeBounds computeBounds;
    vsg_scene->accept(computeBounds);
    vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;
    double radius = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min) * 0.6;
    double nearFarRatio = 0.001;

    uint32_t width = window->traits->width;
    uint32_t height = window->traits->height;

    vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel(vsg_scene->getObject<vsg::EllipsoidModel>("EllipsoidModel"));
    vsg::ref_ptr<vsg::Camera> camera;
    {
        // set up the camera
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, -radius * 3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));

        vsg::ref_ptr<vsg::ProjectionMatrix> perspective;
        if (ellipsoidModel)
        {
            perspective = vsg::EllipsoidPerspective::create(
                lookAt, ellipsoidModel, 30.0,
                static_cast<double>(width) /
                    static_cast<double>(height),
                nearFarRatio, false);
        }
        else
        {
            perspective = vsg::Perspective::create(
                30.0,
                static_cast<double>(width) /
                    static_cast<double>(height),
                nearFarRatio * radius, radius * 4.5);
        }

        camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(VkExtent2D{width, height}));
    }

    auto trackball = vsg::Trackball::create(camera, ellipsoidModel);
    trackball->addWindow(*window);

    renderer->viewer->addEventHandler(trackball);

    auto commandGraph = vsg::createCommandGraphForView(*window, camera, vsg_scene);

    renderer->viewer->addRecordAndSubmitTaskAndPresentation({commandGraph});

    return window;
}

int main(int argc, char* argv[])
{
    vsg::CommandLine arguments(&argc, argv);

    // set up vsg::Options to pass in filepaths and ReaderWriter's and other IO
    // realted options to use when reading and writing files.
    auto options = vsg::Options::create();
    options->fileCache = vsg::getEnv("VSG_FILE_CACHE");
    options->paths = vsg::getEnvPaths("VSG_FILE_PATH");
#ifdef vsgXchange_FOUND
    options->add(vsgXchange::all::create());
#endif

    arguments.read(options);

    auto windowTraits = vsg::WindowTraits::create();
    windowTraits->windowTitle = "vsgQt viewer";
    windowTraits->debugLayer = arguments.read({"--debug", "-d"});
    windowTraits->apiDumpLayer = arguments.read({"--api", "-a"});
    arguments.read("--samples", windowTraits->samples);
    arguments.read({"--window", "-w"}, windowTraits->width, windowTraits->height);
    if (arguments.read({"--fullscreen", "--fs"})) windowTraits->fullscreen = true;

    bool continuousUpdate = !arguments.read({"--event-driven", "--ed"});
    auto internval = arguments.value<int>(0, "--interval");

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

    // create the viewer that will manage all the rendering of the views
    auto viewer = vsgQt::CustomViewer::create();
    auto renderer = vsgQt::Renderer::create(viewer);

    // add close handler to respond the close window button and pressing escape
    // viewer->addEventHandler(vsg::CloseHandler::create(viewer));

    // create the windows
    auto firstWindow = createWindow(renderer, windowTraits, vsg_scene, nullptr, "First Window");
    auto secondWindow = createWindow(renderer, windowTraits, vsg_scene, nullptr, "Second Window");
    auto thirdWindow = createWindow(renderer, windowTraits, vsg_scene, nullptr, "Third Window");

    firstWindow->setGeometry(0, 0, 640, 480);
    firstWindow->show();

    secondWindow->setGeometry(680, 0, 640, 480);
    secondWindow->show();

    thirdWindow->setGeometry(1360, 0, 640, 480);
    thirdWindow->show();

    if (internval >= 0) renderer->setInterval(internval);
    renderer->continuousUpdate = continuousUpdate;

    viewer->addEventHandler(vsg::CloseHandler::create(viewer));
    viewer->compile();

    renderer->setInterval(0);

    return application.exec();
}
