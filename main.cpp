#include <CL/sycl.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "mainwindow.h"
#include "Trace.h"
#include <fstream>

using material::MATERIALS;

int main(int argc, char *argv[]) {
//    int width = ui->resX->value();
//    int height = ui->resY->value();
//    int reflections = 8;
//    int samples = ui->spp->value();
    int width = 1280;
    int height = 720;
    int reflections = 5;
    int samples = 10;


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

    Camera camera = Camera({0, -10, -1.5}, {0, 1, 0}, 90, 0, width, height);
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
    AABB bounds = AABB();
    auto tris = readMesh(bounds, "dragon-low.obj");
    materialBase mat = materialBase({.9, .9, .9}, 1.4, MATERIALS::Glass);
    kVec.emplace_back(kdTreeMesh(tris, bounds, 15, 20, q, mat));
    mat = materialBase({.8, .8, .8}, .0, MATERIALS::Diffuse);
    tris = readMesh(bounds, "plane.obj");
    kVec.emplace_back(kdTreeMesh(tris, bounds, 15, 20, q, mat));
    auto floatImage = new sc::float3[width * height];

    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
    std::vector<Sphere> objects = std::vector<Sphere>();
    objects.push_back(Sphere({-20, -20, -4}, .1, sc::float3({255, 255, 255}) * 0, MATERIALS::Emissive, 1));//Hidden sphere
    auto spheres = std::vector<Sphere>();
    auto meshes = std::vector<std::tuple<kdTreeMesh, std::string>>();
    if (spheres.size() != 0)
        objects = spheres;

    if (meshes.size() != 0) {
        kVec.clear();
        for (auto t : meshes) {
            kVec.push_back(std::get<kdTreeMesh>(t));
        }
    }


    sc::buffer<Ray, 1> rBuf(rays, sc::range<1>(width * height));
    sc::buffer<Sphere, 1> oBuf(objects.data(), sc::range<1>(objects.size()));
    sc::buffer<sc::float3, 1> iBuf(floatImage, sc::range<1>(width * height));
    sc::buffer<kdTreeMesh, 1> kBuf(kVec.data(), sc::range<1>(kVec.size()));

    int colorR = (int) 80;
    int colorG = (int) 80;
    int colorB = (int) 160;

    sc::float3 skyColor(colorR, colorG, colorB);

    std::cout << q.get_device().get_info<sc::info::device::name>() << std::endl;

    sc::event tracingEvent = q.submit([&](sc::handler &cgh) {
        trace(cgh, rBuf, oBuf, iBuf, kBuf, width, height, reflections, samples, skyColor);
//        auto r_acc = rBuf.get_access<sc::access::mode::read>(cgh);
//        auto o_acc = oBuf.get_access<sc::access::mode::read>(cgh);
//        auto i_acc = iBuf.get_access<sc::access::mode::write>(cgh);
//        auto k_acc = kBuf.get_access<sc::access::mode::read>(cgh);
//        cgh.parallel_for<pathtracing>(sc::range<2>(width, height), [=](sc::id<2> idx) {
//            int x = idx[0];
//            int y = idx[1];
//            Ray initialRay = r_acc[x + y * width];
//            Ray r;
//            bool intersect;
//            int seed = (x * y * reflections * samples);
//            fastrand(&seed);
//            fastrand(&seed);
//            sc::float3 sum = {0, 0, 0};
//            //We initialize values necessary for the path tracing code.
//            for (int i = 0; i < samples; i++) {
//                sc::float3 color = {0, 0, 0};
//                r = initialRay;
//
//                material::intersectReturn tobj;
//
//                material::intersectReturn lastIntersect = material::intersectReturn();
//                for (int j = 0; j < reflections; j++) {
//                    intersect = false;
//                    float minDist = FLT_MAX;
//                    for (int k = 0; k < o_acc.get_count(); k++) {
//                        material::intersectReturn ret = o_acc[k].mRayIntersect(r, k);
//                        if (ret == lastIntersect)
//                            continue;
//                        if (ret.intersect
//                            && ret.intersectDistance < minDist) {
//                            intersect = true;
//                            minDist = ret.intersectDistance;
//                            tobj = ret;
//                        }
//                    }
//                    for (int k = 0; k < k_acc.get_count(); ++k) {
//                        material::intersectReturn t_intersect = k_acc[k].mRayIntersect(r, lastIntersect);
//                        if (t_intersect.intersect
//                            && t_intersect.intersectDistance < minDist) {
//                            intersect = true;
////                            hitMesh = true;
//                            minDist = t_intersect.intersectDistance;
//                            tobj = t_intersect;
//                        }
//                    }
//
//                    if (intersect) {
//                        if (tobj.isEmissive()) {
//                            color = tobj.getPixelEmissive(r);
//                            if (j == 0) {
//                                sum = tobj.getPixelEmissive(r) * samples;
//                                goto imageWrite;
//                            }
//                            break;
//                        } else {
//                            sc::float4 randVec = sc::normalize(
//                                    sc::float4(fastrand(&seed), fastrand(&seed),
//                                               fastrand(&seed), fastrand(&seed)));
//                            r = tobj.reflect(r, randVec, fastrand(&seed));
//                        }
//                    } else {
//                        color = r.getLuminance() * skyColor;
//                        if (j == 0) {
//                            sum = skyColor * samples;
//                            goto imageWrite;
//                        }
//                        break;
//                    }
//                    lastIntersect = tobj;
//                }
//                sum += color;
//            }
//            imageWrite:
//            i_acc[x + y * width] = sc::clamp((sum / samples), 0.0f, 255.0f);
//        });
    });

    QApplication a(argc, argv);



    while (tracingEvent.get_info<sc::info::event::command_execution_status>() !=
           sc::info::event_command_status::complete) {
//        QCoreApplication::processEvents(); // Let Qt use the time while we are rendering instead of hanging up the program.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "Rendering... " << (100.0 * (width * height - iBuf.get_access<sc::access::mode::read>().get_count()) / (width * height)) << "%" << std::endl;
    }

    auto imageResult = iBuf.get_access<sc::access::mode::read>();

    ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) -
         ms;
    std::cout << "GPU Time : " << ms.count() << " milliseconds " << std::endl;

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            floatImage[x + y * width] = imageResult[x + y * width];
        }
    }

    sc::float3 center = floatImage[(width / 2) + ((height / 2) * width)];

    std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            sc::float3 color = floatImage[i + j * width];
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
