#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(8);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"Object type"<<"Radius"<<"Position"<<"Direction"<<"Color"<<"Material type"<<"Index of refraction"<<"Mesh path");
    setCentralWidget(ui->horizontalWidget);

    sc::device device{sc::gpu_selector()};
//    sc::device device = sc::host_selector{}.select_device();
    sc::queue queue(device, [](const sc::exception_list& el){for(const auto& e : el) {
        std::rethrow_exception(e);
    }});
}

MainWindow::~MainWindow()
{
    delete ui;
}

auto removeByIndex =
  []<class T>(std::vector<T> &vec, unsigned int index)
{

    vec.erase(vec.begin() + index);
};


void MainWindow::on_actionOpen_triggered()
{
    //Text file editor Open item
    QString fileName = QFileDialog::getOpenFileName(this, "Open file");
    QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly | QFile::Text)){
            QMessageBox::warning(this,"Warning", "Something went wrong");
            return;
        }
        QTextStream in(&file);
        QString text = in.readAll();
        ui->plainTextEdit->document()->setPlainText(text);
        currentFile = fileName;
        file.close();
}

void MainWindow::on_actionNew_triggered()
{
    //Text file editor New item
    currentFile.clear();
    ui->plainTextEdit->document()->setPlainText(QString());
}

void MainWindow::on_actionSave_as_triggered()
{
    //Text file editor Save As item
    QString fileName = QFileDialog::getSaveFileName(this, "Save as");
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning", "Something went wrong");
        return;
    }
    QTextStream out(&file);
    QString text = ui->plainTextEdit->toPlainText();
    out << text;
    currentFile = fileName;
    file.close();

}

void MainWindow::on_refreshJsonButton_clicked()
{
    //Text file editor refresh button
    QFile file(currentFile);
        if(!file.open(QIODevice::ReadOnly | QFile::Text)){
            QMessageBox::critical(this,"Error", "File did not load properly, please try another file");
            return;
        }
        QTextStream in(&file);
        QString text = in.readAll();
        ui->plainTextEdit->document()->setPlainText(text);
        file.close();
}

void MainWindow::on_colorEditButton_clicked()
{
    //Color selection
    //QColorDialog::getColor();
    QPalette q = ui->colorLabel->palette();
    ui->colorLabel->setAutoFillBackground(true);
     q.setColor(ui->colorLabel->backgroundRole(), QColorDialog::getColor());
    ui->colorLabel->setPalette(q);

}

void MainWindow::on_actionSave_triggered()
{
    //Text file editor Save action
    if(!currentFile.isEmpty()){
        QFile file(currentFile);
        if(!file.open(QIODevice::WriteOnly | QFile::Text)){
            QMessageBox::warning(this,"Warning", "Something went wrong");
            return;
        }
        QTextStream out(&file);
        QString text = ui->plainTextEdit->toPlainText();
        out << text;
        file.close();
    }
    else{
       this->on_actionSave_as_triggered();
    }

}



void MainWindow::on_ObjectType_currentIndexChanged(const QString &arg1)
{
    //shape selection combo box
    if(arg1=="Sphere object"){
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
    if(arg1=="Mesh object"){
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

void MainWindow::on_actionAbout_triggered()
{
    //about menu item
    QMessageBox::about(this,"About", "About tab");
}

void MainWindow::on_resetButton_clicked()
{
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



void MainWindow::on_addObjectButton_clicked()
{
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

    if(objType==1 && meshFile=="") {
        QMessageBox::critical(this, "Error", "Please select mesh");
    }

    std::vector<jsonSerializer::polygonData> t1;
    t1.emplace_back(currentFile, meshFile, objType, positionX, positionY, positionZ, radius, scaleX, scaleY, scaleZ, colorR, colorG, colorB, materialType, attr_2);


    jsonSerializer serializer = jsonSerializer();

    serializer.write(currentFile, meshFile, t1[0]);
    on_refreshJsonButton_clicked();
    on_pushButton_6_clicked();
}

void MainWindow::on_importMeshButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file");
    meshFile = fileName;
}



void MainWindow::on_pushButton_6_clicked()
{
    QString fileName = currentFile;
    jsonSerializer serializer = jsonSerializer();

    objectVector = serializer.deserializeJson(currentFile);
    ui->tableWidget->setRowCount(objectVector.size());
    for(int i=0; i<objectVector.size(); i++){
        QJsonObject o = objectVector[i];

        jsonSerializer::polygonData pData = serializer.read(o);

        QTableWidgetItem *it = new QTableWidgetItem(QString::number(pData.objType));
        ui->tableWidget->setItem(i, 0, it);
        std::cout<<pData.objType;
    }
    on_refreshJsonButton_clicked();
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
    selectedRow = row;
    QMessageBox::warning(this,"Test", QString::number(row));
}

void MainWindow::on_removeObject_clicked()
{
    if(!objectVector.empty()){


    removeByIndex(objectVector, selectedRow);
    jsonSerializer serializer = jsonSerializer();
    serializer.overwrite(currentFile, objectVector);
    on_pushButton_6_clicked();
    }
}

//void MainWindow::render(){
//
//    int width = ui->resX->value();
//    int height = ui->resY->value();
//    const int reflections = 8;
//    const int samples = 50; //change this
//    QImage image = QImage(width, height, QImage::Format_RGB32);
//
//    Camera camera = Camera({0,-10,-1.5}, {0,1,0}, 90, 0, width, height);
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
//
//    std::vector<kdTreeMesh> kVec = std::vector<kdTreeMesh>();
//    AABB bounds = AABB();
//    auto tris = readMesh(bounds, "dragon-low.obj");
//    materialBase mat = materialBase({.9, .9, .9}, 1.4, MATERIALS::Glass);
//    kVec.emplace_back(kdTreeMesh(tris, bounds, 15, 20, q, mat));
//    mat = materialBase({.8, .8, .8}, .0, MATERIALS::Diffuse);
//    tris = readMesh(bounds, "plane.obj");
//    kVec.emplace_back(kdTreeMesh(tris, bounds, 15, 20, q, mat));
//    auto floatImage = new sc::float3[width*height];
//
//    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
//    std::vector<Sphere> objects = std::vector<Sphere>();
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
//    q.submit([&](sc::handler &cgh){
//        auto r_acc = rBuf.get_access<sc::access::mode::read>(cgh);
//        auto o_acc = oBuf.get_access<sc::access::mode::read>(cgh);
////        auto s_acc = sBuf.get_access<sc::access::mode::read>(cgh);
//        auto i_acc = iBuf.get_access<sc::access::mode::write>(cgh);
////        auto t_acc = tBuf.get_access<sc::access::mode::read>(cgh);
////        auto m_acc = mBuf.get_access<sc::access::mode::read>(cgh);
//        auto k_acc = kBuf.get_access<sc::access::mode::read>(cgh);
//        cgh.parallel_for<pathTracing>(sc::range<2>(width,height),
//                                 [=](sc::id<2> idx){
//                                     int x = idx[0];
//                                     int y = idx[1];
//                                     Ray initialRay = r_acc[x+y*width];
//                                     Ray r;
////            Ray reflect;
//                                     bool intersect = false;
////            float minDist = MAXFLOAT;
//                                     int seed = (x*y*reflections*samples);
//                                     fastrand(&seed);
//                                     fastrand(&seed);
//                                     sc::float3 sum = {0,0,0};
//                                     bool hitMesh = false;
//                                     for(int i = 0; i<samples; i++) {
//                                         sc::float3 color = {0,0,0};
//                                         r = initialRay;
//////                material::intersectReturn obj{};
////                const Sphere *obj;
//                                         material::intersectReturn tobj;
//
//                                         material::intersectReturn lastIntersect = material::intersectReturn();
//                                         for (int j = 0; j < reflections; j++) {
//                                             intersect = false;
//                                             float minDist = FLT_MAX;
//                                             for (int k = 0; k < o_acc.get_count(); k++) {
//                                                 material::intersectReturn ret = o_acc[k].mRayIntersect(r, k);
//                                                 if(ret==lastIntersect)
//                                                     continue;
//                                                 if (ret.intersect
//                                                     && ret.intersectDistance<minDist) {
//                                                     intersect = true;
//                                                     minDist = ret.intersectDistance;
//                                                     tobj = ret;
//                                                 }
//                                             }
//                                             for (int k = 0; k < k_acc.get_count(); ++k) {
//                                                 material::intersectReturn t_intersect = k_acc[k].mRayIntersect(r, lastIntersect);
//                                                 if (t_intersect.intersect
//                                                     && t_intersect.intersectDistance<minDist) {
//                                                     intersect = true;
//                                                     hitMesh = true;
//                                                     minDist = t_intersect.intersectDistance;
//                                                     tobj = t_intersect;
//                                                 }
//                                             }
//
//                                             if (intersect) {
//                                                 if (tobj.isEmissive()) {
//                                                     color = tobj.getPixelEmissive(r);
//                                                     if(j==0){
//                                                         sum = tobj.getPixelEmissive(r)*samples;
//                                                         goto imageWrite;
//                                                     }
//                                                     break;
//                                                 } else {
//                                                     sc::float4 randVec = sc::normalize(
//                                                             sc::float4(fastrand(&seed), fastrand(&seed), fastrand(&seed), fastrand(&seed)));
//                                                     r = tobj.reflect(r, randVec, fastrand(&seed));
//                                                 }
//                                             }
//
//                                             else{
//                                                 color = r.getLuminance()*sc::float3({70,30,170});
//                                                 if(j==0){
//                                                     sum = sc::float3({70,30,170})*samples;
//                                                     goto imageWrite;
//                                                 }
//                                                 break;
//                                             }
//                                             lastIntersect=tobj;
//                                         }
//                                         sum+=color;
//                                     }
//                                     imageWrite:
//                                     i_acc[x+y*width] = sc::clamp((sum/samples), 0.0f, 255.0f);
//
////            i_acc[x+y*width] = sc::clamp((k_acc[0].mRayIntersect(initialRay, material::intersectReturn()).intersect * sc::float3(200,200,200)), 0.0f, 255.0f);
////            i_acc[x+y*width] = sc::clamp((ret.intersect * sc::float3(200,200,200)), 0.0f, 255.0f);
////            i_acc[x+y*width] = sc::clamp(k_acc[0].mRayIntersect(initialRay).intersect * sc::float3(50,50,50), 0.0f, 255.0f);
////            auto m = Mesh(t_acc.get_pointer(), m_acc[0]);
////            i_acc[x+y*width] = sc::clamp(m.tBoxIntersect(initialRay).intersect * sc::float3(50,50,50), 0.0f, 255.0f);
//                                 });
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
//}
void MainWindow::render() {};