#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H
#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <iostream>
#include <QJsonArray>



class jsonSerializer
{
public:
    jsonSerializer(){
    };

    struct polygonData{
        QString pathName;
        QString meshPath;
        int objType;
        float positionX;
        float positionY;
        float positionZ;
        float radius;
        float directionX;
        float directionY;
        float directionZ;
        int colorR;
        int colorG;
        int colorB;
        int materialType;
        float refractIndex;

        polygonData(QString pathName, QString meshPath, int objType, float positionX, float positionY, float positionZ, float radius, float directionX, float directionY, float directionZ, int colorR, int colorG, int colorB, int materialType, float refractIndex){
            this->pathName = pathName;
            this->meshPath = meshPath;
            this->objType = objType;
            this->positionX = positionX;
            this->positionY = positionY;
            this->positionZ = positionZ;
            this->radius = radius;
            this->directionX = directionX;
            this->directionY = directionY;
            this->directionZ = directionZ;
            this->colorR = colorR;
            this->colorG = colorG;
            this->colorB = colorB;
            this->materialType = materialType;
            this->refractIndex = refractIndex;
        }

        polygonData(){

        }

    };


    //std::tuple<QString, QString, int, float,float,float,float,float,float,float,int,int,int,int,float> read(const QJsonObject &json){
      polygonData read(const QJsonObject &json){

        int radius = json.value("radius").toInt();
        int objType = json.value("objType").toInt();
        float positionX = (float)json.value("positionX").toDouble();
        float positionY = (float)json.value("positionY").toDouble();
        float positionZ = (float)json.value("positionZ").toDouble();
        float directionX = (float)json.value("directionX").toDouble();
        float directionY = (float)json.value("directionY").toDouble();
        float directionZ = (float)json.value("directionZ").toDouble();
        int colorR = json.value("colorR").toInt();
        int colorG = json.value("colorG").toInt();
        int colorB = json.value("colorB").toInt();
        int materialType = json.value("materialType").toInt();
        float refractIndex = (float)json.value("refractIndex").toDouble();
        QString meshPath = json.value("meshPath").toString();

        return polygonData("",meshPath, objType, positionX, positionY, positionZ, radius, directionX, directionY, directionZ, colorR, colorG, colorB, materialType, refractIndex);
        //return std::tuple<QString, QString, int, float,float,float,float,float,float,float,int,int,int,int,float>("",meshPath, objType, positionX, positionY, positionZ, radius, directionX, directionY, directionZ, colorR, colorG, colorB, materialType, refractIndex );
    }

    std::vector<QJsonObject> deserializeJson(QString &pathName){

        std::vector<QJsonObject> jsonObjectVector = {};

        QFile saveFile(pathName);
        saveFile.open(QIODevice::ReadWrite);

        if (pathName=="") {
                qWarning("Couldn't open save file.");
                return {};
            }


        QByteArray allSaveData = saveFile.readAll();


        QJsonDocument doc = QJsonDocument::fromJson(allSaveData);
        //QJsonDocument doc = QJsonDocument::fromJson(allSaveData.toUtf8());
        QJsonArray objectArray = doc.array();
        //std::cout<<objectArray.isEmpty();



        for(int i=0; i<objectArray.size();i++){
            jsonObjectVector.push_back(objectArray[i].toObject());
        }
        std::cout<<jsonObjectVector.size();
        return jsonObjectVector;

    }


    bool write(QString &pathName, QString &meshPath, polygonData p) const {
        QFile saveFile(pathName);
        saveFile.open(QIODevice::ReadWrite);

        if (pathName=="") {
                qWarning("Couldn't open save file.");
                return false;
            }


        QString allSaveData = saveFile.readAll();


        QJsonDocument doc = QJsonDocument::fromJson(allSaveData.toUtf8());


        QJsonArray objectArray = doc.array();


        QJsonObject polygonObject;


        polygonObject.insert("radius", p.radius);
        polygonObject.insert("objType", p.objType);

        polygonObject.insert("positionX", p.positionX);
        polygonObject.insert("positionY", p.positionY);
        polygonObject.insert("positionZ", p.positionZ);

        polygonObject.insert("directionX", p.directionX);
        polygonObject.insert("directionY", p.directionY);
        polygonObject.insert("directionZ", p.directionZ);

        polygonObject.insert("colorR", p.colorR);
        polygonObject.insert("colorG", p.colorG);
        polygonObject.insert("colorB", p.colorB);

        polygonObject.insert("materialType", p.materialType);
        polygonObject.insert("refractIndex", p.refractIndex);

        if(p.objType==1){
            polygonObject.insert("meshPath", meshPath);
        }
        //std::cout<<objectArray.isEmpty();
        objectArray.append(polygonObject);
        saveFile.resize(0);
        saveFile.write(QJsonDocument(objectArray).toJson());
        saveFile.close();

        return true;

    }
    bool overwrite(QString &pathName, std::vector<QJsonObject> objList){
        QFile saveFile(pathName);
        saveFile.open(QIODevice::ReadWrite);

        if (pathName=="") {
                qWarning("Couldn't open save file.");
                return false;
            }


        //QString allSaveData = saveFile.readAll();


        //QJsonDocument doc = QJsonDocument::fromJson(allSaveData.toUtf8());


        QJsonArray objectArray;// = doc.array();
        for(QJsonObject o:objList){
            objectArray.append(o);
        }
        saveFile.resize(0);
        saveFile.write(QJsonDocument(objectArray).toJson());
        saveFile.close();
        return true;

    }


};




#endif // JSONSERIALIZER_H


