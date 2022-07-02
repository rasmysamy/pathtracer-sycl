#include <CL/sycl.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "mainwindow.h"
#include "Trace.h"
#include "scene.h"
#include <fstream>

using material::MATERIALS;

int main(int argc, char *argv[]) {
//    int width = ui->resX->value();
//    int height = ui->resY->value();
//    int reflections = 8;
//    int samples = ui->spp->value();
    int width = 1920;
    int height = 1080;
    int reflections = 6;
    int samples = 5000;


//    sc::device device{sc::cpu_selector()};
    sc::device device{sc::gpu_selector()};
//    sc::device device = sc::host_selector{}.select_device();
    sc::queue q(device, [](const sc::exception_list &el) {
        for (const auto &e : el) {
            std::rethrow_exception(e);
        }
    });

    std::cout << "Device :" << device.get_info<sc::info::device::name>() << std::endl;

//    if(ui->previewCheck->isChecked()){
//        samples=3;
//        reflections=3;
//    }
    QImage image = QImage(width, height, QImage::Format_RGB32);

    Camera camera = Camera({0, -10, -0.7}, {0, 1, 0}, 90, 0, width, height);
    auto rays = new Ray[width * height];
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            rays[x + y * width] = camera.getRay(x + 1, y + 1);
        }
    }

    std::random_device rd;
    std::mt19937 eng(rd());

    std::uniform_int_distribution<int> dist;

    auto seeds = new int[width * height];
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            seeds[x + y * width] = dist(eng);
        }
    }

    std::vector<kdTreeMesh> kVec = std::vector<kdTreeMesh>();
//    AABB bounds = AABB();
//    auto tris = readMesh(bounds, "dragon-low.obj");
//    materialBase mat = materialBase({.9, .9, .9}, 1.4, MATERIALS::Glass);
//    kVec.emplace_back(kdTreeMesh(tris, bounds, 13, 5, q, mat));
//    mat = materialBase({.8, .8, .8}, .0, MATERIALS::Diffuse);
//    tris = readMesh(bounds, "plane.obj");
//    kVec.emplace_back(kdTreeMesh(tris, bounds, 5, 20, q, mat));
    auto floatImage = new sc::float3[width * height];


    std::vector<Sphere> objects = std::vector<Sphere>();
//    auto spheres = std::vector<Sphere>();
//    spheres.push_back(Sphere({-0, -0, -4}, 1, sc::float3({255, 255, 255}) * 10, MATERIALS::Emissive, 1));//Hidden sphere
//    auto meshes = std::vector<std::tuple<kdTreeMesh, std::string>>();
    scene s{};
    s.makeScene(kVec, objects, q);

//    if (spheres.size() != 0)
//        objects = spheres;
//
//    if (meshes.size() != 0) {
//        kVec.clear();
//        for (auto t : meshes) {
//            kVec.push_back(std::get<kdTreeMesh>(t));
//        }
//    }


    sc::buffer<Ray, 1> rBuf(rays, sc::range<1>(width * height));
    sc::buffer<Sphere, 1> oBuf(objects.data(), sc::range<1>(objects.size()));
    sc::buffer<sc::float3, 1> iBuf(floatImage, sc::range<1>(width * height));
    sc::buffer<kdTreeMesh, 1> kBuf(kVec.data(), sc::range<1>(kVec.size()));
    sc::buffer<int, 1> sBuf(seeds, sc::range<1>(width * height));

    int colorR = (int) 120;
    int colorG = (int) 120;
    int colorB = (int) 255;

    sc::float3 skyColor(colorR, colorG, colorB);

    std::cout << q.get_device().get_info<sc::info::device::name>() << std::endl;
    std::vector<sc::event*> events;
    int step = 10;
    for(int i = 0; i < samples; i+=step){
    sc::event tracingEvent = q.submit([&](sc::handler &cgh) {
        trace(cgh, rBuf, oBuf, iBuf, kBuf, sBuf, width, height, reflections, samples, skyColor, step);
    });
    events.push_back(&tracingEvent);
    }
    std::cout << events.size() << std::endl;

    QApplication a(argc, argv);



//    while (events[events.size()-1]->get_info<sc::info::event::command_execution_status>() !=
//           sc::info::event_command_status::complete) {
////        QCoreApplication::processEvents(); // Let Qt use the time while we are rendering instead of hanging up the program.
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));
////            std::cout << "Rendering... " << (100.0 * (width * height - iBuf.get_access<sc::access::mode::read>().get_count()) / (width * height)) << "%" << std::endl;
//    }
//    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
//    while(true){
//        int counter = 0;
//        bool allProcessed = true;
//        for(const auto& event : events){
//            if(event->get_info<sc::info::event::command_execution_status>() != sc::info::event_command_status::complete){
//                allProcessed = false;
//            }
//            else
//                counter++;
//        }
//
//        if(allProcessed){
//            break;
//        }
//
//        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//        std::cout << "Rendering... " << (100.0 * counter/events.size()) << "%" << std::endl;
////        std::cout << "Rendering... " << (100.0 * (width * height - iBuf.get_access<sc::access::mode::read>().get_count()) / (width * height)) << "%" << std::endl;
//    }
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());

    auto imageResult = iBuf.get_access<sc::access::mode::read>();

    ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) -
         ms;
    std::cout << "GPU Time : " << ms.count() << " milliseconds " << std::endl;

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            floatImage[x + y * width] = imageResult[x + y * width]/samples;
        }
    }

    sc::float3 center = floatImage[(width / 2) + ((height / 2) * width)];

    std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            sc::float3 color = sc::clamp(floatImage[i + j * width], 0.0f, 255.0f);
            image.setPixelColor(i, j, QColor::fromRgb(abs(color.x()), abs(color.y()), abs(color.z())));
        }
    }

    //QPushButton button("Hello world!", nullptr);
    //button.resize(200, 100);
    //button.show();
    QLabel renderViewWindow{};
    renderViewWindow.clear();
    renderViewWindow.setPixmap(QPixmap::fromImage(image));
    renderViewWindow.show();
    renderViewWindow.resize(width, height);

    std::cout << "Rendering done!" << std::endl;


    std::ofstream output("/tmp/mptt.json");
    output.close();
//    QApplication a(argc, argv);

//    MainWindow w;
//    w.setGeometry(
//            QStyle::alignedRect(
//                    Qt::LeftToRight,
//                    Qt::AlignCenter,
//                    w.size(),
//                    qApp->desktop()->screenGeometry()
//            )
//    );
//
//    w.show();
//    w.setColorLabelsInitial();
//
    return QApplication::exec();
    return 0;
}
