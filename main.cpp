#include <CL/sycl.hpp>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QColor>
#include <QLabel>
#include <chrono>
#include <cmath>
#include "Camera.h"
#include "Sphere.h"

class trace;

const int width=1280;
const int height=720;

int main(int argc, char *argv[]) {
    Camera camera = Camera({0,0,0}, {0,1,0}, 90, 0, width, height);
    Sphere sphere = Sphere({0,2,-0.3}, .05);

    QImage image = QImage(width, height, QImage::Format_RGB32);

    sc::device device{sc::default_selector{}};
    //sc::device device = sc::cpu_selector{}.select_device();
    //sc::device device = sc::host_selector{}.select_device();
    sc::queue queue(device, [](const sc::exception_list& el){for(const auto& e : el) {
        std::rethrow_exception(e);
    }});

    std::cout << "Device: " << device.get_info<sycl::info::device::name>() << std::endl;

    auto rays = new Ray[width*height];
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            rays[x+y*width] = camera.getRay(x+1, y+1);
        }
    }
    auto floatImage = new sc::float3[width*height]();
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    std::vector<Sphere> objects = std::vector<Sphere>();
    objects.push_back(Sphere({0,3,0.2}, .4));
    objects.push_back(Sphere({0,3,1}, .9));

    sc::buffer<Ray, 1> rBuf(rays, sc::range<1>(width* height));
    sc::buffer<Sphere, 1> oBuf(objects.data(), sc::range<1>(objects.size()));
    sc::buffer<sc::float3, 1> iBuf(floatImage, sc::range<1>(width* height));

    const int objSize = objects.size();

    queue.submit([&](sc::handler &cgh){
        auto r_acc = rBuf.get_access<sc::access::mode::read>(cgh);
        auto o_acc = oBuf.get_access<sc::access::mode::read>(cgh);
        auto i_acc = iBuf.get_access<sc::access::mode::write>(cgh);
        cgh.parallel_for<trace>(sc::range<2>(width,height),
                [=](sc::id<2> idx){
            int x = idx[0];
            int y = idx[1];
            Ray r = r_acc[x+y*width];
            Ray reflect;
            bool intersect = false;
            float minDist = MAXFLOAT;
            int intersectIndex = 0;
            for(int i = 0; i<objSize; i++){
                Ray t_reflect;
                bool t_intersect;
                std::tie(t_reflect, t_intersect) = o_acc[i].rayIntersect(r);
                if(t_intersect) {
                    bool smallest = minDist > sc::fast_length(r.getOrigin()-t_reflect.getOrigin());
                    if (smallest) {
                        minDist = sc::fast_length(r.getOrigin()-t_reflect.getOrigin());
                        reflect = t_reflect;
                        intersect = true;
                        intersectIndex = i;
                    }
                }
            }
            sc::float3 normal = sc::float3{-255,-255,-255} * sc::normalize(reflect.getDirection());
            normal = intersect ? normal : sc::float3({0,0,0});
            i_acc[x+y*width] = sc::float3({0,0,0}) + (normal*intersect);
            //i_acc[x+y*width] = sc::float3({70,70,0}) + (normal*intersect);
        });
    });

    ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - ms;
    std::cout << "GPU Time : " << ms.count() << " milliseconds " << std::endl;

    auto imageResult = iBuf.get_access<sc::access::mode::read>();
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            floatImage[x+y*width] = imageResult[x+y*width];
        }
    }

    sc::float3 center = floatImage[(width/2)+((height/2)*width)];

    std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            sc::float3 color = floatImage[i+j*width];
            image.setPixelColor(i, j, QColor::fromRgb(abs(color.x()), abs(color.y()), abs(color.z())));
        }
    }

    QApplication a(argc, argv);
    //QPushButton button("Hello world!", nullptr);
    //button.resize(200, 100);
    //button.show();
    QLabel label;
    label.setPixmap(QPixmap::fromImage(image));
    label.show();
    return QApplication::exec();

//    return 0;
}
