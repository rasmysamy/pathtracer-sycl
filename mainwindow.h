#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <CL/sycl.hpp>
#include <cmath>
#include <QMainWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QColor>
#include <QStyle>
#include <QDesktopWidget>
#include <random>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "jsonserializer.h"
#include "kdTreeMesh.h"
#include "Sphere.h"
#include "Camera.h"
#include <string>
#include <vector>
#include <tuple>
#include <QLabel>
#include <QColorDialog>
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace sc = cl::sycl;

class pathtracing;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    sc::queue q;
    sc::device d;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
//    void populateObjects(){
//        spheres.clear();
//        for(auto ob : objectVector){
//            sc::float3 position;
//            sc::float3 scale;
//            sc::float3 attr_1;
//            int objType;
//            int mat;
//            float radius;
//            float attr_2;
//            std::string meshPath;
//            position = sc::float3(ob.constFind("positionX")->toDouble(), ob.constFind("positionY")->toDouble(),
//                                  ob.constFind("positionZ")->toDouble());
//            scale = sc::float3(ob.constFind("directionX")->toDouble(), ob.constFind("directionY")->toDouble(),
//                               ob.constFind("directionZ")->toDouble());
//            attr_1 = sc::float3(ob.constFind("colorX")->toDouble(), ob.constFind("colorY")->toDouble(),
//                                ob.constFind("colorZ")->toDouble());
//            radius = ob.constFind("radius")->toDouble();
//            attr_2 = ob.constFind("refractIndex")->toDouble();
//            objType = ob.constFind("objType")->toInt();
//            mat = ob.constFind("materialType")->toInt();
//            meshPath = ob.constFind("meshPath")->toString().toStdString();
//
//            if(mat==MATERIALS::Emissive)
//                attr_1 *= 10;
//            //if we have a mesh, type 1, we construct a mesh unless one already exists and has the same path.
//            if(objType==1 && std::count(meshPath.begin(), meshPath.end(), meshPath)==0){
//                AABB bounds = AABB();
//                auto tris = readMesh(bounds, meshPath);
//                materialBase matB = materialBase(attr_1, attr_2, mat);
////            meshes.emplace_back(kdTreeMesh(tris, bounds, 15, 20, q, matB));
//            }//The mesh has been loaded!
//            if(objType==1){
//                spheres.emplace_back(Sphere(position, radius, attr_1, mat));
//            }
//        }
//    };

private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionSave_as_triggered();

    void on_refreshJsonButton_clicked();

    void on_colorEditButton_clicked();

    void on_actionSave_triggered();

    void on_ObjectType_currentIndexChanged(const QString &arg1);

    void on_actionAbout_triggered();

    void on_resetButton_clicked();

    void on_addObjectButton_clicked();

    void on_importMeshButton_clicked();


    void on_pushButton_6_clicked();

    void on_tableWidget_cellClicked(int row, int column);

    void on_removeObject_clicked();

    void render();

    void on_startRender_clicked(){
        render();
    }

private:
    Ui::MainWindow *ui;
    QString currentFile = "/tmp/mptt.json";
    QString meshFile = "";
    int selectedRow = -1;
    std::vector<QJsonObject> objectVector;
    std::vector<kdTreeMesh> meshes;
    std::vector<Sphere> spheres;
};


#endif // MAINWINDOW_H
