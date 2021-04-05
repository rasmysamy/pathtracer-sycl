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
#include "Triangle.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

class tracer;

const int width=100;
const int height=60;
using material::MATERIALS;

int main(int argc, char *argv[]) {
    std::string inputfile = "teapot-low.obj";
    tinyobj::ObjReaderConfig reader_config;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();


    std::vector<Triangle> tObjects = std::vector<Triangle>();

// Loop over shapes
    for (const auto & shape : shapes) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            auto fv = size_t(shape.mesh.num_face_vertices[f]);

            size_t v = 0;
            tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
            tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
            tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
            tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
            sc::float3 A = {vx, vz, -vy};
            v++;
            idx = shape.mesh.indices[index_offset + v];
            vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
            vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
            vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
            sc::float3 B = {vx, vz, -vy};
            v++;
            idx = shape.mesh.indices[index_offset + v];
            vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
            vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
            vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
            sc::float3 C = {vx, vz, -vy};
            v++;

            tObjects.push_back(Triangle(A,B,C, {1.5,1.5,1.5}, MATERIALS::Diffuse));

            index_offset += fv;

            // per-face material
            shape.mesh.material_ids[f];
        }
    }

    Camera camera = Camera({0,-10,-1.5}, {0,1,0}, 90, 0, width, height);
//    Camera camera = Camera({0,-10,}, {0,1,0}, 90, 0, width, height);
    Sphere sphere = Sphere({0,2,-0.3}, .05);

    QImage image = QImage(width, height, QImage::Format_RGB32);
    //sc::device device{sc::default_selector{}};
//    sc::device device = sc::cpu_selector{}.select_device();
    sc::device device{sc::gpu_selector()};
//    sc::device device = sc::host_selector{}.select_device();
    sc::queue queue(device, [](const sc::exception_list& el){for(const auto& e : el) {
        std::rethrow_exception(e);
    }});



    std::cout << "Device: " << device.get_info<sc::info::device::name>() << std::endl;

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
    //objects.push_back(Sphere({0,3,-0.5}, .4, {.7,.7,.7}, MATERIALS::Diffuse));
    objects.push_back(Sphere({0,0, -4}, 2, sc::float3({255,255,255})*3, MATERIALS::Emissive));

//    objects.push_back(Sphere({0,3,1}, .9, {200,750,50}, MATERIALS::Emissive));
    //objects.push_back(Sphere({0,3,1}, .9, {.7, .7, .7}, MATERIALS::Diffuse));
//    sc::float3 A,B,C,D;
//    A = {400, 400, 3};
//    B = {400, -400, 3};
//    C = {-400, -400, 3};
//    D = {-400, 400, 3};
    //tObjects.push_back(Triangle(A, B, C, {.7,.7,.70}, MATERIALS::Diffuse));
    //tObjects.push_back(Triangle({0,3,-1}, {0.8,3.1,-0.6}, {0,3,2}, {.7,.7,.70}, MATERIALS::Diffuse));
    tObjects.push_back(Triangle({0,3,-1}, {0.8,3.1,-0.6}, {0,3,-2}, {1500,700,2700}, MATERIALS::Emissive));

    sc::buffer<Ray, 1> rBuf(rays, sc::range<1>(width* height));
    sc::buffer<Sphere, 1> oBuf(objects.data(), sc::range<1>(objects.size()));
    sc::buffer<sc::float3, 1> iBuf(floatImage, sc::range<1>(width* height));
    sc::buffer<int, 1> sBuf(seeds, sc::range<1>(width* height));
    sc::buffer<Triangle, 1> tBuf(tObjects.data(), sc::range<1>(tObjects.size()));

    const int objSize = objects.size();
    const int tobjSize = tObjects.size();

    queue.submit([&](sc::handler &cgh){
        auto r_acc = rBuf.get_access<sc::access::mode::read>(cgh);
        auto o_acc = oBuf.get_access<sc::access::mode::read>(cgh);
        auto s_acc = sBuf.get_access<sc::access::mode::read>(cgh);
        auto i_acc = iBuf.get_access<sc::access::mode::write>(cgh);
        auto t_acc = tBuf.get_access<sc::access::mode::read>(cgh);
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
            int samples = 1;
            for(int i = 0; i<samples; i++) {
                sc::float3 color = {0,0,0};
                r = initialRay;
//                material::intersectReturn obj{};
                const Sphere *obj;
                const Triangle *tobj;
                bool isTriangle;
                for (int j = 0; j < 3; j++) {
                    intersect = false;
                    minDist = 99999999990;
                    //RNG.setSeed(RNG.getSeed()*(x+1)/(y+1));
                    for (int k = 0; k < o_acc.get_count(); k++) {
//                        material::intersectReturn t = o_acc[k].srayIntersect(r);
//                        Ray t_reflect = Ray(t.hitpoint, t.normal);
//                        bool t_intersect = t.intersect;

                        Ray t_reflect = r;
                        bool t_intersect;

                        //o_acc[k].pRayIntersect(r, &t_reflect, &t_intersect);

                        float dist = o_acc[k].fRayIntersect(r);
                        t_intersect = dist!=-1;
//                        r = Ray(r.getOrigin()+(r.getDirection()*dist),
//                                r.getOrigin()+(r.getDirection()*dist) - o_acc[k].getCenter());

                        t_intersect=false;

                        //std::tie(t_reflect, t_intersect) = o_acc[k].rayIntersect(r);

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
                            //float dist = sc::fast_length(r.getOrigin() - t_reflect.getOrigin());
                            if (minDist > dist) {
                                minDist = dist;
                                reflect = t_reflect;
                                intersect = true;
                                //obj = t;
                                obj = &o_acc[k];
                                isTriangle = false;
                                //-normal = sc::normalize(o_acc[k].getCenter()-t_reflect.getOrigin());
                            }
                        }
                    }

                    for (int k = 0; k < t_acc.get_count(); k++) {
                        material::intersectReturn t = t_acc[k].rayIntersect(r);
                        Ray t_reflect = Ray(t.hitpoint, t.normal);
                        bool t_intersect = t.intersect;

                        if (t_intersect) {
                            float dist = sc::fast_length(r.getOrigin() - t_reflect.getOrigin());
                            if (minDist > dist) {
                                minDist = dist;
                                reflect = t_reflect;
                                intersect = true;
                                tobj = &t_acc[k];
                                isTriangle = true;
                            }
                        }
                    }

//                    if (intersect) {
//                        if (obj.isEmissive()) {
//                            color = obj.getPixelEmissive(r);
//                        } else {
//                            sc::float4 randVec = sc::normalize(sc::float4(fastrand(&seed), fastrand(&seed), fastrand(&seed), fastrand(&seed)));
//                            r = obj.reflect(r, BoxMueller3(randVec));
//                            //r = obj->reflect(r, reflect, reflect.getDirection());
//                            //r=reflect;
//                        }
////                        color = {100,100,100};
//                    }

                    if (intersect) {
                        if(!isTriangle) {
                            if (obj->isEmissive()) {
                                color = obj->getPixelEmissive(r);
                                j=10;
                            } else {
//                                sc::float4 randVec = sc::normalize(
//                                        sc::float4(fastrand(&seed), fastrand(&seed), fastrand(&seed), fastrand(&seed)));
//                                r = obj->reflect(r, reflect, BoxMueller3(randVec));
                                //r = obj->reflect(r, reflect, reflect.getDirection());
                                //r=reflect;
                            }
                        }
                        else{
                            if (tobj->isEmissive()) {
                                color = tobj->getPixelEmissive(r);
                                j=10;
                            } else {
                                color={100,100,100};
//                                sc::float4 randVec = sc::normalize(
//                                        sc::float4(fastrand(&seed), fastrand(&seed), fastrand(&seed), fastrand(&seed)));
//                                r = tobj->reflect(r, reflect, BoxMueller3(randVec));
//                                //r = obj->reflect(r, reflect, reflect.getDirection());
//                                //r=reflect;
                            }
                        }
                        j=10;
                    }

                    else{
                        color = r.getLuminance()*sc::float3({70,30,130});
                        j=10;
                    }
                }
                sum+=color;
                //sum+= normal*255;
            }
//            sc::float3 normal = sc::float3{-255,-255,-255} * sc::normalize(reflect.getDirection());
//            normal = intersect ? normal : sc::float3({0,0,0});
//            i_acc[x+y*width] = sc::float3({0,0,0}) + (normal*intersect);

            //i_acc[x+y*width] = (sum/samples).x() > 255 ? sc::float3({255, 255, 255}) : sum/samples;
            i_acc[x+y*width] = sc::clamp(sum/samples, 0.0f, 255.0f);
//            i_acc[x+y*width] = sum/samples;
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
