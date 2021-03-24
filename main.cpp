#include <CL/sycl.hpp>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QColor>
#include <QLabel>
#include <chrono>
#include <cmath>
#include <random>
#include "Camera.h"
#include "Sphere.h"
#include "Random.h"

class tracer;

const int width=1920;
const int height=1080;
using material::MATERIALS;

int main(int argc, char *argv[]) {
    Camera camera = Camera({0,0,0}, {0,1,0}, 90, 0, width, height);
    Sphere sphere = Sphere({0,2,-0.3}, .05);

    QImage image = QImage(width, height, QImage::Format_RGB32);
    sc::device device{sc::gpu_selector()};
    //sc::device device{sc::default_selector{}};
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
    std::random_device rd;
    std::mt19937 eng(rd());

    std::uniform_int_distribution<int> dist;

    auto seeds = new int[width*height];
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            seeds[x+y*width] = dist(eng);
        }
    }

    auto floatImage = new sc::float3[width*height]();
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    std::vector<Sphere> objects = std::vector<Sphere>();
    objects.push_back(Sphere({0,3,-0.5}, .4, {.7,.7,.7}, MATERIALS::Diffuse));
    objects.push_back(Sphere({0.4,3.1,-0.3}, .4, {.7,.7,.7}, MATERIALS::Diffuse));
    objects.push_back(Sphere({0,3,1}, .9, {200,750,50}, MATERIALS::Emissive));

    sc::buffer<Ray, 1> rBuf(rays, sc::range<1>(width* height));
    sc::buffer<Sphere, 1> oBuf(objects.data(), sc::range<1>(objects.size()));
    sc::buffer<sc::float3, 1> iBuf(floatImage, sc::range<1>(width* height));
    sc::buffer<int, 1> sBuf(seeds, sc::range<1>(width* height));

    const int objSize = objects.size();

    queue.submit([&](sc::handler &cgh){
        auto r_acc = rBuf.get_access<sc::access::mode::read>(cgh);
        auto o_acc = oBuf.get_access<sc::access::mode::read>(cgh);
        auto s_acc = sBuf.get_access<sc::access::mode::read>(cgh);
        auto i_acc = iBuf.get_access<sc::access::mode::write>(cgh);
        cgh.parallel_for<tracer>(sc::range<2>(width,height),
                [=](sc::id<2> idx){
            int x = idx[0];
            int y = idx[1];
            Ray initialRay = r_acc[x+y*width];
            Ray r;
            Ray reflect;
            bool intersect = false;
            float minDist = MAXFLOAT;
            int seed = s_acc[x+y*width];
            sc::float3 sum = {0,0,0};
            int samples = 300;
            for(int i = 0; i<samples; i++) {
                sc::float3 color = {0,0,0};
                r = initialRay;
                const Sphere* obj;
                for (int j = 0; j < 4; j++) {
                    intersect = false;
                    minDist = MAXFLOAT;
                    //RNG.setSeed(RNG.getSeed()*(x+1)/(y+1));
                    for (int k = 0; k < objSize; k++) {
                        Ray t_reflect;
                        bool t_intersect;
                        std::tie(t_reflect, t_intersect) = o_acc[k].rayIntersect(r);

//                        float hitDistance = o_acc[k].fRayIntersect(r);
//
//                        if(hitDistance!=-1 && minDist>hitDistance){
//                            minDist = hitDistance;
//                            float dist = sc::fast_length(r.getOrigin() - t_reflect.getOrigin());
//                            sc::float3 hitPoint = r.getOrigin() + (hitDistance * sc::normalize(r.getDirection()));
//                            sc::float3 normal = sc::normalize(hitPoint - o_acc[k].getCenter());
//                            reflect = Ray(hitPoint, normal);
//                            intersect = true;
//                            obj = &o_acc[k];
//                        }

                        if (t_intersect) {
                            float dist = sc::fast_length(r.getOrigin() - t_reflect.getOrigin());
                            if (minDist > dist) {
                                minDist = dist;
                                reflect = t_reflect;
                                intersect = true;
                                obj = &o_acc[k];
                                //-normal = sc::normalize(o_acc[k].getCenter()-t_reflect.getOrigin());
                            }
                        }
                    }
                    if (intersect) {
                        if (obj->isEmissive()) {
                            color = obj->getPixelEmissive(r);
                            break;
                        } else {
                            sc::float4 randVec = sc::normalize(sc::float4(fastrand(&seed), fastrand(&seed), fastrand(&seed), fastrand(&seed)));
                            r = obj->reflect(r, reflect, BoxMueller3(randVec));
                            //r = obj->reflect(r, reflect, reflect.getDirection());
                            //r=reflect;
                        }
                    }
                    else{
                        color = r.getLuminance()*sc::float3({70,30,130});
                    }
                }
                sum+=color;
                //sum+= normal*255;
            }
//            sc::float3 normal = sc::float3{-255,-255,-255} * sc::normalize(reflect.getDirection());
//            normal = intersect ? normal : sc::float3({0,0,0});
//            i_acc[x+y*width] = sc::float3({0,0,0}) + (normal*intersect);

            i_acc[x+y*width] = sc::clamp(sum/samples, 0, 255);
            //i_acc[x+y*width] = sum/samples;
//                    RRand = Random(RRand.state);
//                    float xr = RRand.value;
//                    RRand = Random(RRand.state);
//                    float yr = RRand.value;
//                    RRand = Random(RRand.state);
//                    float zr = RRand.value;
//                    i_acc[x+y*width] = sc::float3({xr,yr,zr})*255;
        });
    });

    auto imageResult = iBuf.get_access<sc::access::mode::read>();

    ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - ms;
    std::cout << "GPU Time : " << ms.count() << " milliseconds " << std::endl;

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
