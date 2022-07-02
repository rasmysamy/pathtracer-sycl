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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    sc::queue q;
    sc::device d;
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() { };
    void populateObjects(){
        spheres.clear();
        meshes.clear();
        for(auto ob : objectVector){
            sc::float3 position;
            sc::float3 scale;
            sc::float3 attr_1;
            int objType;
            int mat;
            float radius;
            float attr_2;
            std::string meshPath;
            position = sc::float3(ob.constFind("positionX")->toDouble(), ob.constFind("positionY")->toDouble(),
                                  -ob.constFind("positionZ")->toDouble());
            scale = sc::float3(ob.constFind("directionX")->toDouble(), ob.constFind("directionY")->toDouble(),
                               ob.constFind("directionZ")->toDouble());
            attr_1 = sc::float3(ob.constFind("colorR")->toDouble(), ob.constFind("colorG")->toDouble(),
                                ob.constFind("colorB")->toDouble());
            radius = ob.constFind("radius")->toDouble();
            attr_2 = ob.constFind("refractIndex")->toDouble();
            objType = ob.constFind("objType")->toInt();
            mat = ob.constFind("materialType")->toInt();
            meshPath = ob.constFind("meshPath")->toString().toStdString();

            bool pathSeenAlready=false;

            if(mat==MATERIALS::Emissive)
                attr_1 *= 100;
            else
                attr_1 *= (1.0f/255);
            //if we have a mesh, type 1, we construct a mesh unless one already exists and has the same path.
            int i = 0;
            for(auto k : meshes){
                if (std::get<std::string>(k) == meshPath) {
                    auto kM = std::get<kdTreeMesh>(k);
                    if (vectorEquals(kM.getM().attr_1, attr_1) && kM.getM().attr_2 == attr_2 &&
                        kM.getM().material == mat) {
                        meshes.erase(meshes.begin() + i);
                        pathSeenAlready = true;
                    }
                }
                ++i;
            }
            if(objType==1 && !pathSeenAlready){
                AABB bounds = AABB();
                auto tris = readMesh(bounds, meshPath);
                sc::float3 shiftVec = position - bounds.getCenter();
                for (int j = 0; j < tris.size(); ++j) {
                    tris[j].v0 = (tris[j].v0 * scale) + shiftVec;
                    tris[j].v1 = (tris[j].v1 * scale) + shiftVec;
                    tris[j].v2 = (tris[j].v2 * scale) + shiftVec;
                }
                bounds.bounds[0] *= scale;
                bounds.bounds[1] *= scale;
                bounds.bounds[0] += shiftVec;
                bounds.bounds[1] += shiftVec;
                //This way the center of the mesh is shifter to the position specified by the user, and it is scaled by the factors needed.
                materialBase matB = materialBase(attr_1, attr_2, mat);
                meshes.emplace_back(std::tuple<kdTreeMesh, std::string>(
                        kdTreeMesh(tris, bounds, 13, 20, q, matB), meshPath));
            }//The mesh has been loaded!
            if(objType==0){
                spheres.emplace_back(Sphere(position, radius, attr_1, mat, attr_2));
            }
        }
    };

    void on_update_all();

    void setColorLabelsInitial();

private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionSave_as_triggered();

    void on_refreshJsonButton_clicked();

    void on_colorEditButton_clicked();

    void on_skyColorPicker_clicked();

    void on_actionSave_triggered();

    void on_ObjectType_currentIndexChanged(const QString &arg1);

    void on_material_currentIndexChanged(int index);

    void on_actionAbout_triggered();

    void on_resetButton_clicked();

    void on_addObjectButton_clicked();

    void on_importMeshButton_clicked();

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
    QLabel renderViewWindow;
    int selectedRow = -1;
    std::vector<QJsonObject> objectVector;
    std::vector<std::tuple<kdTreeMesh, std::string>> meshes;
    std::vector<Sphere> spheres;
};


#endif // MAINWINDOW_H
