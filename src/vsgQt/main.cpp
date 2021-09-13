#include <vsg/all.h>
#include <vsgXchange/all.h>

#include <QPlatformSurfaceEvent>
#include <QVulkanInstance>
#include <QWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <vulkan/vulkan.h>

#include "VulkanWindow.h"

int main(int argc, char* argv[])
{
    vsg::CommandLine arguments(&argc, argv);

    // set up vsg::Options to pass in filepaths and ReaderWriter's and other IO
    // realted options to use when reading and writing files.
    auto options = vsg::Options::create();
    options->fileCache = vsg::getEnv("VSG_FILE_CACHE");
    options->paths = vsg::getEnvPaths("VSG_FILE_PATH");

    // add vsgXchange's support for reading and writing 3rd party file formats
    options->add(vsgXchange::all::create());

    arguments.read(options);

    auto windowTraits = vsg::WindowTraits::create();
    windowTraits->windowTitle = "vsgviewer";
    windowTraits->debugLayer = arguments.read({"--debug", "-d"});
    windowTraits->apiDumpLayer = arguments.read({"--api", "-a"});
    if (arguments.read({"--fullscreen", "--fs"}))
        windowTraits->fullscreen = true;
    if (arguments.read({"--window", "-w"}, windowTraits->width,
                       windowTraits->height))
    {
        windowTraits->fullscreen = false;
    }
    auto numFrames = arguments.value(-1, "-f");
    auto horizonMountainHeight = arguments.value(0.0, "--hmh");

    auto useQt = arguments.read("--qt");
    if (arguments.read("--vsg"))
        useQt = false;

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

    if (useQt)
    {
        QApplication application(argc, argv);

        QMainWindow* mainWindow = new QMainWindow();

        auto* vulkanWindow = new vsgQt::VulkanWindow();
        vulkanWindow->traits = windowTraits;
        vulkanWindow->initializeCallback = [&](vsgQt::VulkanWindow& vw) {
            auto& viewer = vw.viewer;
            auto& window = vw.proxyWindow;

            if (!viewer)
            {
                viewer = vsg::Viewer::create();
                viewer->addWindow(window);
            }

            // compute the bounds of the scene graph to help position camera
            vsg::ComputeBounds computeBounds;
            vsg_scene->accept(computeBounds);
            vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;
            double radius = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min) * 0.6;
            double nearFarRatio = 0.001;

            // set up the camera
            auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, -radius * 3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));

            vsg::ref_ptr<vsg::ProjectionMatrix> perspective;
            vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel(
                vsg_scene->getObject<vsg::EllipsoidModel>("EllipsoidModel"));
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

            // add close handler to respond the close window button and pressing
            // escape
            viewer->addEventHandler(vsg::CloseHandler::create(viewer));
            viewer->addEventHandler(vsg::Trackball::create(camera, ellipsoidModel));

            auto commandGraph = vsg::createCommandGraphForView(window, camera, vsg_scene);
            viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

            viewer->compile();
        };

        auto widget = QWidget::createWindowContainer(vulkanWindow, mainWindow);
        mainWindow->setCentralWidget(widget);

        mainWindow->resize(windowTraits->width, windowTraits->height);

        mainWindow->show();

        return application.exec();
    }
    else
    {
        try
        {
            // create the viewer and assign window(s) to it
            auto viewer = vsg::Viewer::create();
            auto window = vsg::Window::create(windowTraits);
            if (!window)
            {
                std::cout << "Could not create windows." << std::endl;
                return 1;
            }

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

            // add close handler to respond the close window button and pressing
            // escape
            viewer->addEventHandler(vsg::CloseHandler::create(viewer));
            viewer->addEventHandler(vsg::Trackball::create(camera, ellipsoidModel));

            auto commandGraph = vsg::createCommandGraphForView(window, camera, vsg_scene);
            viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

            viewer->compile();

            // rendering main loop
            while (viewer->advanceToNextFrame() &&
                   (numFrames < 0 || (numFrames--) > 0))
            {
                // pass any events into EventHandlers assigned to the Viewer
                viewer->handleEvents();

                viewer->update();

                viewer->recordAndSubmit();

                viewer->present();
            }
        }
        catch (const vsg::Exception& ve)
        {
            for (int i = 0; i < argc; ++i)
                std::cerr << argv[i] << " ";
            std::cerr << "\n[Exception] - " << ve.message << " result = " << ve.result
                      << std::endl;
            return 1;
        }

        // clean up done automatically thanks to ref_ptr<>
        return 0;
    }
}
