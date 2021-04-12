#include <CL/sycl.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "mainwindow.h"
//#include "Mesh.h"
#include <fstream>;

class tracer;

const int width=720;
const int height=480;
const int samples = 1;
const int reflections = 9;
using material::MATERIALS;

int main(int argc, char *argv[]) {
    std::ofstream output("/tmp/mptt.json");
    output.close();
    QApplication a(argc, argv);

    std::string inputfile = "dragon-low.obj";
//    std::string inputfile = "teapot-low.obj";

    Camera camera = Camera({0,-10,-1.5}, {0,1,0}, 90, 0, width, height);
//    Camera camera = Camera({0,-10,}, {0,1,0}, 90, 0, width, height);
    Sphere sphere = Sphere({0,2,-0.3}, .05);
    //sc::device device{sc::default_selector{}};
//    sc::device device = sc::cpu_selector{}.select_device();

    MainWindow w;
    w.setGeometry(

            QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignCenter,
                    w.size(),
                    qApp->desktop()->screenGeometry()
            )

    );
    w.show();

//    std::vector<kdTreeMesh> kVec = std::vector<kdTreeMesh>();
//    AABB bounds = AABB();
//    auto tris = readMesh(bounds, "dragon-low.obj");
////    materialBase mat = materialBase({.7, .4, .1}, .8, MATERIALS::Glossy);
//    materialBase mat = materialBase({.9, .9, .9}, 1.4, MATERIALS::Glass);
//    kVec.emplace_back(kdTreeMesh(tris, bounds, 15, 20, queue, mat));
////    tTemp = std::vector<Triangle>();
////    temp1 = MeshStage("plane.obj", &tTemp, MATERIALS::Diffuse, {.7,.7,.7});
//    mat = materialBase({.8, .8, .8}, .0, MATERIALS::Diffuse);
//    tris = readMesh(bounds, "plane.obj");
//    kVec.emplace_back(kdTreeMesh(tris, bounds, 15, 20, queue, mat));
////    kVec.emplace_back(kdTreeMesh(tTemp, temp1.boundingVolume, 18, 250, queue, mat));
//
//    std::cout << "Device: " << device.get_info<sc::info::device::name>() << std::endl;
//
//    auto rays = new Ray[width*height];
//    for (int x = 0; x < width; ++x) {
//        for (int y = 0; y < height; ++y) {
//            rays[x+y*width] = camera.getRay(x+1, y+1);
//        }
//    }
//
//    std::random_device rd;
//    std::mt19937 eng(rd());
//
//    std::uniform_int_distribution<int> dist;
//
//    auto seeds = new int[width*height];
//    for (int x = 0; x < width; ++x) {
//        for (int y = 0; y < height; ++y) {
//            seeds[x+y*width] = dist(eng);
//        }
//    }
//    auto floatImage = new sc::float3[width*height];
//    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
//    std::vector<Sphere> objects = std::vector<Sphere>();
//    //objects.push_back(Sphere({0,3,-0.5}, .4, {.7,.7,.7}, MATERIALS::Diffuse));
//    objects.push_back(Sphere({0,0, -4}, 2, sc::float3({255,255,255})*6, MATERIALS::Emissive));
//
////    objects.push_back(Sphere({0,3,1}, .9, {200,750,50}, MATERIALS::Emissive));
//    //objects.push_back(Sphere({0,3,1}, .9, {.7, .7, .7}, MATERIALS::Diffuse));
////    sc::float3 A,B,C,D;
////    A = {400, 400, 3};
////    B = {400, -400, 3};
////    C = {-400, -400, 3};
////    D = {-400, 400, 3};
//    //tObjects.push_back(Triangle(A, B, C, {.7,.7,.70}, MATERIALS::Diffuse));
//    //tObjects.push_back(Triangle({0,3,-1}, {0.8,3.1,-0.6}, {0,3,2}, {.7,.7,.70}, MATERIALS::Diffuse));
//    //tObjects.push_back(Triangle({0,3,-1}, {0.8,3.1,-0.6}, {0,3,-2}, {1500,700,2700}, MATERIALS::Emissive));
//
//
//
//    sc::buffer<Ray, 1> rBuf(rays, sc::range<1>(width* height));
//    sc::buffer<Sphere, 1> oBuf(objects.data(), sc::range<1>(objects.size()));
//    sc::buffer<sc::float3, 1> iBuf(floatImage, sc::range<1>(width* height));
////    sc::buffer<int, 1> sBuf(seeds, sc::range<1>(width* height));
////    sc::buffer<Triangle, 1> tBuf(tStack.data(), sc::range<1>(tStack.size()));
////    sc::buffer<MeshStage, 1> mBuf(mObjects.data(), sc::range<1>(mObjects.size()));
//    sc::buffer<kdTreeMesh, 1> kBuf(kVec.data(), sc::range<1>(kVec.size()));
//
////    const int objSize = objects.size();
////    const int tobjSize = tStack.size();
////
////    Mesh me = Mesh(tStack.data(), s1);
////    material::intersectReturn re = me.tBoxIntersect(rays[width/2 + (height/2)*width]);
////
////    std::cout << re.intersect << std::endl;
//
//    queue.submit([&](sc::handler &cgh){
//        auto r_acc = rBuf.get_access<sc::access::mode::read>(cgh);
//        auto o_acc = oBuf.get_access<sc::access::mode::read>(cgh);
////        auto s_acc = sBuf.get_access<sc::access::mode::read>(cgh);
//        auto i_acc = iBuf.get_access<sc::access::mode::write>(cgh);
////        auto t_acc = tBuf.get_access<sc::access::mode::read>(cgh);
////        auto m_acc = mBuf.get_access<sc::access::mode::read>(cgh);
//        auto k_acc = kBuf.get_access<sc::access::mode::read>(cgh);
//        cgh.parallel_for<tracer>(sc::range<2>(width,height),
//                [=](sc::id<2> idx){
//            int x = idx[0];
//            int y = idx[1];
//            Ray initialRay = r_acc[x+y*width];
//            Ray r;
////            Ray reflect;
//            bool intersect = false;
////            float minDist = MAXFLOAT;
//            int seed = (x*y*reflections*samples);
//            fastrand(&seed);
//            fastrand(&seed);
//            sc::float3 sum = {0,0,0};
//            bool hitMesh = false;
//            for(int i = 0; i<samples; i++) {
//                sc::float3 color = {0,0,0};
//                r = initialRay;
//////                material::intersectReturn obj{};
////                const Sphere *obj;
//                material::intersectReturn tobj;
//
//                material::intersectReturn lastIntersect = material::intersectReturn();
//                for (int j = 0; j < reflections; j++) {
//                    intersect = false;
//                    float minDist = FLT_MAX;
//                    for (int k = 0; k < o_acc.get_count(); k++) {
//                        material::intersectReturn ret = o_acc[k].mRayIntersect(r, k);
//                        if(ret==lastIntersect)
//                            continue;
//                        if (ret.intersect
//                            && ret.intersectDistance<minDist) {
//                            intersect = true;
//                            minDist = ret.intersectDistance;
//                            tobj = ret;
//                        }
//                    }
//                    for (int k = 0; k < k_acc.get_count(); ++k) {
//                        material::intersectReturn t_intersect = k_acc[k].mRayIntersect(r, lastIntersect);
//                        if (t_intersect.intersect
//                            && t_intersect.intersectDistance<minDist) {
//                            intersect = true;
//                            hitMesh = true;
//                            minDist = t_intersect.intersectDistance;
//                            tobj = t_intersect;
//                        }
//                    }
//
//                    if (intersect) {
//                        if (tobj.isEmissive()) {
//                            color = tobj.getPixelEmissive(r);
//                            if(j==0){
//                                sum = tobj.getPixelEmissive(r)*samples;
//                                goto imageWrite;
//                            }
//                            break;
//                        } else {
//                            sc::float4 randVec = sc::normalize(
//                                    sc::float4(fastrand(&seed), f
//#include "Mesh.h"astrand(&seed), fastrand(&seed), fastrand(&seed)));
//                            r = tobj.reflect(r, randVec, fastrand(&seed));
//                        }
//                    }
//
//                    else{
//                        color = r.getLuminance()*sc::float3({70,30,170});
//                        if(j==0){
//                            sum = sc::float3({70,30,170})*samples;
//                            goto imageWrite;
//                        }
//                        break;
//                    }
//                    lastIntersect=tobj;
//                }
//                sum+=color;
//            }
//            imageWrite:
//            i_acc[x+y*width] = sc::clamp((sum/samples), 0.0f, 255.0f);
//
////            i_acc[x+y*width] = sc::clamp((k_acc[0].mRayIntersect(initialRay, material::intersectReturn()).intersect * sc::float3(200,200,200)), 0.0f, 255.0f);
////            i_acc[x+y*width] = sc::clamp((ret.intersect * sc::float3(200,200,200)), 0.0f, 255.0f);
////            i_acc[x+y*width] = sc::clamp(k_acc[0].mRayIntersect(initialRay).intersect * sc::float3(50,50,50), 0.0f, 255.0f);
////            auto m = Mesh(t_acc.get_pointer(), m_acc[0]);
////            i_acc[x+y*width] = sc::clamp(m.tBoxIntersect(initialRay).intersect * sc::float3(50,50,50), 0.0f, 255.0f);
//        });
//    });
//
//    auto imageResult = iBuf.get_access<sc::access::mode::read>();
//
//    ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - ms;
//    std::cout << "GPU Time : " << ms.count() << " milliseconds " << std::endl;
//
//    for (int x = 0; x < width; ++x) {
//        for (int y = 0; y < height; ++y) {
//            floatImage[x+y*width] = imageResult[x+y*width];
//        }
//    }
//
//    sc::float3 center = floatImage[(width/2)+((height/2)*width)];
//
//    std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;
//
//    for (int i = 0; i < width; i++) {
//        for (int j = 0; j < height; j++) {
//            sc::float3 color = floatImage[i+j*width];
//            image.setPixelColor(i, j, QColor::fromRgb(abs(color.x()), abs(color.y()), abs(color.z())));
//        }
//    }
//
//    //QPushButton button("Hello world!", nullptr);
//    //button.resize(200, 100);
//    //button.show();
//    QLabel label;
//    label.setPixmap(QPixmap::fromImage(image));
//    label.show();
    return QApplication::exec();


//    return 0;
}
