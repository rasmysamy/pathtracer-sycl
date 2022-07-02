#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Trace.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
//    auto ui = this;
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(1);
    ui->tableWidget->setHorizontalHeaderLabels(
            QStringList() << "Object description");
    setCentralWidget(ui->horizontalWidget);

//    sc::device device{sc::gpu_selector()};
    sc::device device{sc::cpu_selector()};
//    sc::device device = sc::host_selector{}.select_device();
    sc::queue queue(device, [](const sc::exception_list &el) {
        for (const auto &e : el) {
            std::rethrow_exception(e);
        }
    });

    std::cout << "Device :" << device.get_info<sc::info::device::name>() << std::endl;
}

void MainWindow::setColorLabelsInitial(){
    ui->colorLabel->setAutoFillBackground(true);
    ui->skyColorLabel->setAutoFillBackground(true);

    QPalette q = ui->colorLabel->palette();
    q.setColor(QPalette::Window, QColor::fromRgb(200,200,200));
    ui->colorLabel->setPalette(q);

    q = ui->skyColorLabel->palette();
    q.setColor(QPalette::Window, QColor::fromRgb(200,200,255));
    ui->skyColorLabel->setPalette(q);
}

//MainWindow::~MainWindow() {
//    delete ui;
//}

auto removeByIndex =
        []<class T>(std::vector<T> &vec, unsigned int index) {

            vec.erase(vec.begin() + index);
        };


void MainWindow::on_actionOpen_triggered() {
    //Text file editor Open item
    QString fileName = QFileDialog::getOpenFileName(this, "Open file", "./", "MPPT Project files (.mppt)");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Something went wrong");
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    ui->plainTextEdit->document()->setPlainText(text);
    currentFile = fileName;
    file.close();
    on_refreshJsonButton_clicked();
}

void MainWindow::on_actionNew_triggered() {
    //Text file editor New item
    currentFile = "/tmp/mppt.mppt";
    ui->plainTextEdit->document()->setPlainText(QString());
    on_refreshJsonButton_clicked();
}

void MainWindow::on_actionSave_as_triggered() {
    //Text file editor Save As item
    QString fileName = QFileDialog::getSaveFileName(this, "Save as", "./", "MPPT Project files (.mppt)");
    QFile file(fileName + QString::fromStdString(".mppt"));
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Something went wrong");
        return;
    }
    QTextStream out(&file);
    QString text = ui->plainTextEdit->toPlainText();
    out << text;
    currentFile = fileName;
    file.close();

}

void MainWindow::on_refreshJsonButton_clicked() {
    on_update_all();
    //Text file editor refresh button
    QFile file(currentFile);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, "Error", "File did not load properly, please try another file");
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    ui->plainTextEdit->document()->setPlainText(text);
    file.close();
}

void MainWindow::on_colorEditButton_clicked() {
    QPalette q = ui->colorLabel->palette();
    ui->colorLabel->setAutoFillBackground(true);
    q.setColor(ui->colorLabel->backgroundRole(), QColorDialog::getColor());
    ui->colorLabel->setPalette(q);
}

void MainWindow::on_skyColorPicker_clicked() {
    QPalette q = ui->skyColorLabel->palette();
    ui->skyColorLabel->setAutoFillBackground(true);
    q.setColor(ui->skyColorLabel->backgroundRole(), QColorDialog::getColor());
    ui->skyColorLabel->setPalette(q);
}


void MainWindow::on_actionSave_triggered() {
    //Text file editor Save action
    if (!currentFile.isEmpty()) {
        QFile file(currentFile);
        if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "Warning", "Something went wrong");
            return;
        }
        QTextStream out(&file);
        QString text = ui->plainTextEdit->toPlainText();
        out << text;
        file.close();
    } else {
        this->on_actionSave_as_triggered();
    }

}


void MainWindow::on_ObjectType_currentIndexChanged(const QString &arg1) {
    //shape selection combo box
    if (arg1 == "Sphere object") {
        //commands to enable boxes and labels associated with sphere objects only and to disable mesh-only ones
        ui->label_4->setEnabled(true);
        ui->doubleSpinBox_radius->setEnabled(true);
        ui->label_10->setEnabled(false);
        ui->label_11->setEnabled(false);
        ui->label_12->setEnabled(false);
        ui->label_13->setEnabled(false);
        ui->doubleSpinBox_sX->setEnabled(false);
        ui->doubleSpinBox_sY->setEnabled(false);
        ui->doubleSpinBox_sZ->setEnabled(false);
        ui->importMeshButton->setEnabled(false);

    }
    if (arg1 == "Mesh object") {
        //same as above but reversed
        ui->label_4->setEnabled(false);
        ui->doubleSpinBox_radius->setEnabled(false);
        ui->label_10->setEnabled(true);
        ui->label_11->setEnabled(true);
        ui->label_12->setEnabled(true);
        ui->label_13->setEnabled(true);
        ui->doubleSpinBox_sX->setEnabled(true);
        ui->doubleSpinBox_sY->setEnabled(true);
        ui->doubleSpinBox_sZ->setEnabled(true);
        ui->importMeshButton->setEnabled(true);
    }
}

void MainWindow::on_actionAbout_triggered() {
    //about menu item
    QMessageBox::about(this, "About", "About tab");
}

void MainWindow::on_resetButton_clicked() {
    //reset for:
    //shape
    ui->ObjectType->setCurrentIndex(0);
    //radius
    ui->doubleSpinBox_radius->setValue(1.00);
    //x y z for position
    ui->doubleSpinBox_X->setValue(0.00);
    ui->doubleSpinBox_Y->setValue(0.00);
    ui->doubleSpinBox_Z->setValue(0.00);
    //x y z for normal vector
    ui->doubleSpinBox_sX->setValue(0.00);
    ui->doubleSpinBox_sY->setValue(0.00);
    ui->doubleSpinBox_sZ->setValue(0.00);
    //color
    ui->colorLabel->setPalette(palette());
    //material type
    ui->material->setCurrentIndex(0);
}


void MainWindow::on_addObjectButton_clicked() {
    int objType = ui->ObjectType->currentIndex();

    float positionX = (float) ui->doubleSpinBox_X->value();
    float positionY = (float) ui->doubleSpinBox_Y->value();
    float positionZ = (float) ui->doubleSpinBox_Z->value();

    float scaleX = (float) ui->doubleSpinBox_sX->value();
    float scaleY = (float) ui->doubleSpinBox_sY->value();
    float scaleZ = (float) ui->doubleSpinBox_sZ->value();

    float radius = (float) ui->doubleSpinBox_radius->value();

    int colorR = (int) ui->colorLabel->palette().window().color().red();
    int colorG = (int) ui->colorLabel->palette().window().color().green();
    int colorB = (int) ui->colorLabel->palette().window().color().blue();

    int materialType = (int) ui->material->currentIndex();

    float attr_2 = (float) ui->doubleSpinBox_attr2->value();

    if (objType == 1 && meshFile == "") {
        QMessageBox::critical(this, "Error", "Please select mesh");
        return;
    }

    std::vector<jsonSerializer::polygonData> t1;
    t1.emplace_back(currentFile, meshFile, objType, positionX, positionY, positionZ, radius, scaleX, scaleY, scaleZ,
                    colorR, colorG, colorB, materialType, attr_2);


    jsonSerializer serializer = jsonSerializer();

    serializer.write(currentFile, meshFile, t1[0]);


    on_refreshJsonButton_clicked();
    populateObjects();
}

void MainWindow::on_importMeshButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select 3D mesh", "./", "OBJ 3D Meshes (*.obj)");
    meshFile = fileName;
    ui->importMeshButton->setText((meshFile.toStdString().substr(meshFile.toStdString().find_last_of('/')+1) + " (click to change)").c_str());
}


void MainWindow::on_update_all() {
    jsonSerializer serializer = jsonSerializer();

    objectVector = serializer.deserializeJson(currentFile);
    ui->tableWidget->setRowCount(objectVector.size());
    for (int i = 0; i < objectVector.size(); i++) {
        QJsonObject o = objectVector[i];

        jsonSerializer::polygonData pData = serializer.read(o);

        std::string itemDescription;

        if(pData.objType==0){
            itemDescription = "Sphere of radius :" + std::to_string(pData.radius);
        }
        else{
            itemDescription = "Mesh :" + pData.meshPath.toStdString().substr(pData.meshPath.toStdString().find_last_of('/')+1);
        }

        QTableWidgetItem *it = new QTableWidgetItem(QString::fromStdString(itemDescription));

        ui->tableWidget->setItem(i, 0, it);
    }
    //on_refreshJsonButton_clicked();
}

void MainWindow::on_tableWidget_cellClicked(int row, int column) {
    selectedRow = row;
    QJsonObject ob = objectVector[row];

    auto position = sc::float3(ob.constFind("positionX")->toDouble(), ob.constFind("positionY")->toDouble(),
                               ob.constFind("positionZ")->toDouble());
    auto scale = sc::float3(ob.constFind("directionX")->toDouble(), ob.constFind("directionY")->toDouble(),
                            ob.constFind("directionZ")->toDouble());
    auto attr_1 = sc::float3(ob.constFind("colorR")->toDouble(), ob.constFind("colorG")->toDouble(),
                             ob.constFind("colorB")->toDouble());
    auto radius = ob.constFind("radius")->toDouble();
    auto attr_2 = ob.constFind("refractIndex")->toDouble();
    auto objType = ob.constFind("objType")->toInt();
    auto mat = ob.constFind("materialType")->toInt();
    auto mPath = ob.constFind("meshPath")->toString().toStdString();

    ui->ObjectType->setCurrentIndex(objType);

    ui->doubleSpinBox_X->setValue(position[0]);
    ui->doubleSpinBox_Y->setValue(position[1]);
    ui->doubleSpinBox_Z->setValue(position[2]);

    ui->doubleSpinBox_sX->setValue(scale[0]);
    ui->doubleSpinBox_sY->setValue(scale[1]);
    ui->doubleSpinBox_sZ->setValue(scale[2]);

    ui->doubleSpinBox_radius->setValue(radius);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor::fromRgb(attr_1[0], attr_1[1], attr_1[2]));
    ui->colorLabel->setPalette(pal);

    ui->material->setCurrentIndex(mat);

    ui->doubleSpinBox_attr2->setValue(attr_2);

    meshFile = QString::fromStdString(mPath);
    ui->importMeshButton->setText((meshFile.toStdString().substr(meshFile.toStdString().find_last_of('/')+1) + " (click to change)").c_str());


    //QMessageBox::warning(this,"Test", QString::number(row));
}

void MainWindow::on_removeObject_clicked() {
    if (!objectVector.empty()) {
        removeByIndex(objectVector, selectedRow);
        jsonSerializer serializer = jsonSerializer();
        serializer.overwrite(currentFile, objectVector);
        meshes.clear();
        spheres.clear();
    }
    on_refreshJsonButton_clicked();
}

void MainWindow::render() {
    on_update_all();
    populateObjects();

    int width = ui->resX->value();
    int height = ui->resY->value();
    int reflections = 8;
    int samples = ui->spp->value();

    if(ui->previewCheck->isChecked()){
        samples=3;
        reflections=3;
    }
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
    auto tris = readMesh(bounds, "plane.obj");
    materialBase mat = materialBase({.9, .9, .9}, 1.4, MATERIALS::Glass);
//    kVec.emplace_back(kdTreeMesh(tris, bounds, 15, 20, q, mat));
    mat = materialBase({.8, .8, .8}, .0, MATERIALS::Diffuse);
//    tris = readMesh(bounds, "plane.obj");
    kVec.emplace_back(kdTreeMesh(tris, bounds, 15, 20, q, mat));
    auto floatImage = new sc::float3[width * height];

    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
    std::vector<Sphere> objects = std::vector<Sphere>();
    objects.push_back(Sphere({-20, -20, -4}, .1, sc::float3({255, 255, 255}) * 0, MATERIALS::Emissive, 1));//Hidden sphere

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

    int colorR = (int) ui->skyColorLabel->palette().window().color().red();
    int colorG = (int) ui->skyColorLabel->palette().window().color().green();
    int colorB = (int) ui->skyColorLabel->palette().window().color().blue();

    sc::float3 skyColor(colorR, colorG, colorB);

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


    while (tracingEvent.get_info<sc::info::event::command_execution_status>() !=
           sc::info::event_command_status::complete) {
        QCoreApplication::processEvents(); // Let Qt use the time while we are rendering instead of hanging up the program.
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
    renderViewWindow.clear();
    renderViewWindow.setPixmap(QPixmap::fromImage(image));
    renderViewWindow.show();
    renderViewWindow.resize(width, height);

    std::cout << "Rendering done!" << std::endl;
}

void MainWindow::on_material_currentIndexChanged(int index) {
    if (index == 2) {
        ui->doubleSpinBox_attr2->setSingleStep(0.05);
        ui->doubleSpinBox_attr2->setMaximum(1);
        ui->doubleSpinBox_attr2->setMinimum(0);
        ui->doubleSpinBox_attr2->setValue(0.2);
        ui->iorLabel->setText("Roughness");
        ui->doubleSpinBox_attr2->setEnabled(true);
        ui->iorLabel->setEnabled(true);
    } else if (index == 3) {
        ui->doubleSpinBox_attr2->setSingleStep(0.05);
        ui->doubleSpinBox_attr2->setMinimum(1);
        ui->doubleSpinBox_attr2->setMaximum(4);
        ui->doubleSpinBox_attr2->setValue(1.4);
        ui->iorLabel->setText("Index of Refraction");
        ui->doubleSpinBox_attr2->setEnabled(true);
        ui->iorLabel->setEnabled(true);
    } else {
        ui->iorLabel->setText("Disabled");
        ui->doubleSpinBox_attr2->setDisabled(true);
        ui->iorLabel->setDisabled(true);
    }
}
//void MainWindow::render() {};